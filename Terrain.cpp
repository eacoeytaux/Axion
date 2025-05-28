#include "Terrain.hpp"

Terrain::~Terrain( )
{
    for_each( edges, m_edges )
    {
        for_each( edge, edges )
        {
            safe_delete( edge );
        }
    }
    m_edges.clear( );

    for_each( vertices, m_vertices )
    {
        for_each( vertex, vertices )
        {
            safe_delete( vertex );
        }
    }
    m_vertices.clear( );
}

Terrain::Terrain( Room * room, const varray<varray<Coordinate>> & _vertices ) : Object( room )
{
    #if defined ( AXN_DEBUG )
    draw_debug = true;
    #endif

    no_gravity( );

    bool first = true;
    for_each( vertices, _vertices )
    {
        make( vertices, !first );
        first = false;
    }
}

void Terrain::make( varray<Coordinate> cref _positions, cbool _loop )
{
    if( _positions.size( ) <= 1 )
    {
        return;
    }

    m_vertices.insert_back( varray<TerrainVertex *>( ) );
    m_edges.insert_back( varray<TerrainEdge *>( ) );

    TerrainVertex * first = nullptr;
    TerrainVertex * last = nullptr;
    TerrainVertex * previous = nullptr;
    for_range( i, _positions.size( ) )
    {
        Coordinate cref coordinate = _positions[ i ];
        TerrainVertex * vertex = new TerrainVertex( coordinate );
        m_vertices.back( ).insert_back( vertex );
        room( )->object_grid( ).insert( vertex );

        if( i )
        {
            Assert( !is_null( previous ) );
            TerrainEdge * edge = new TerrainEdge( previous, vertex );
            m_edges.back( ).insert_back( edge );
            room( )->object_grid( ).insert( edge );
        }

        previous = vertex;

        if( _loop )
        {
            if( !i )
            {
                first = vertex;
            }
            else if( i == ( _positions.size( ) - 1 ) )
            {
                last = vertex;
            }
        }
    }

    if( _loop )
    {
        if( first && last )
        {
            TerrainEdge * edge = new TerrainEdge( last, first );
            m_edges.back( ).insert_back( edge );
            room( )->object_grid( ).insert( edge );
        }
    }
}

const varray<varray<TerrainVertex *>> & Terrain::vertices( ) const { return m_vertices; }

const varray<varray<TerrainEdge *>> & Terrain::edges( ) const { return m_edges; }

void Terrain::traverse_x( const Span<Planc> & _distance_x, const function<void( Coordinate cref, const TerrainEdge * )> & f ) const
{
    if( !( edges( ).size( ) ) )
        return;

    for_each( edges, edges( ) )
    {
        if( !edges.size( ) )
        {
            continue;
        }

        TerrainEdge * edge = edges.front( );
        Planc x = edge->vertex1( )->position( ).x( ) + Random::rPlanc( _distance_x.range( ) );

        while( edge )
        {
            Planc y = edge->line( ).y( x );
            f( Coordinate( x, y ), edge );

            Planc dx = Random::rPlanc( _distance_x );

            x += dx;
            while( edge && ( x > edge->vertex2( )->position( ).x( ) ) )
            {
                edge = edge->vertex2( )->edge2( );
            }
        }
    }

};

#if defined ( AXN_DEBUG )
Drawing Terrain::debug_overlay( ) const
{
    cPlanc GROUND_WIDTH = 1.5;
    cPlanc GROUND_VERTEX_WIDTH = 2.5;
    cPlanc NORMAL_LENGTH = 10.0;
    cColor COLOR = CYAN;

    Drawing debug_overlay;

    if( Settings::get( Settings::DEBUG_PHYSICS ) )
    {
        cdec _zoom = room( )->camera( )->zoom( );
        
        auto draw_normal = [ & ] ( Coordinate cref base, Angle cref normal, bool left, bool right )
        {
            Planc length = ( NORMAL_LENGTH / _zoom );
            Coordinate end = base + VectorA( normal.flipped( ), length );
            Vector offset = VectorA( normal + RIGHT, half( half( length ) ) );
            
            debug_overlay.draw( COLOR, Line( base, end ), GROUND_WIDTH, true );
            if( left ) { debug_overlay.draw( COLOR, Line( end, end + offset ), GROUND_WIDTH, true ); }
            if( right ) { debug_overlay.draw( COLOR, Line( end, end - offset ), GROUND_WIDTH, true ); }
        };
        
        for_each( edges, edges( ) )
        {
            for_each( edge, edges )
            {
                debug_overlay.draw( COLOR, edge->line( ), GROUND_WIDTH, true );
                draw_normal( midpoint( edge->vertex1( )->position( ), edge->vertex2( )->position( ) ), edge->normal( ), true, true );
            }
        }

        for_each( vertices, vertices( ) )
        {
            for_each( vertex, vertices )
            {
                debug_overlay.draw( COLOR, Polygon::circle( GROUND_VERTEX_WIDTH / _zoom, vertex->position( ) ), FILLED, true );
                draw_normal( vertex->position( ), vertex->normal( ), !is_null( vertex->edge1( ) ), !is_null( vertex->edge2( ) ) );
            }
        }
    }

    return debug_overlay;
}
#endif

TerrainVertex::TerrainVertex( Coordinate cref _position ) { m_position = _position; }

Coordinate cref TerrainVertex::position( ) const { return m_position; }

TerrainEdge * TerrainVertex::edge1( ) const { return m_e1; }
TerrainEdge * TerrainVertex::edge2( ) const { return m_e2; }

void TerrainVertex::edge1( TerrainEdge * e1 )
{
    Assert( is_null( m_e1 ), "edge 1 has already been set" );

    m_e1 = e1;
}

void TerrainVertex::edge2( TerrainEdge * e2 )
{
    Assert( is_null( m_e2 ), "edge 2 has already been set" );

    m_e2 = e2;
}

Angle TerrainVertex::normal( ) const
{
    if( m_e1 && m_e2 )
    {
        return half( m_e1->normal( ) + m_e2->normal( ) );
    }
    else if( m_e1 )
    {
        return m_e1->normal( );
    }
    else if( m_e2 )
    {
        return m_e2->normal( );
    }

    return Angle( RIGHT );
}

dec TerrainVertex::resistance( ) const { return m_resistance; }

FixedRectangle TerrainVertex::bounding_box( ) const { return FixedRectangle( 0.0, 0.0, position( ) ); }

TerrainEdge::TerrainEdge( TerrainVertex * _v1, TerrainVertex * _v2, cdec _resistance ) : m_v1( _v1 ), m_v2( _v2 ), m_resistance( _resistance )
{
    Assert( !is_null( m_v1 ) && !is_null( m_v2 ) );

    m_v1->edge2( this );
    m_v2->edge1( this );
}

TerrainEdge::TerrainEdge( TerrainEdge cref _edge )
{
    m_v1 = _edge.vertex1( );
    m_v2 = _edge.vertex2( );
}

Line TerrainEdge::line( ) const { return Line( m_v1->position( ), m_v2->position( ) ); }

Vector TerrainEdge::vector( ) const { return Vector( m_v1->position( ), m_v2->position( ) ); }

TerrainVertex * TerrainEdge::vertex1( ) const { return m_v1; }
TerrainVertex * TerrainEdge::vertex2( ) const { return m_v2; }

Angle TerrainEdge::normal( ) const { return vector( ).angle( ) + RIGHT; }

dec TerrainEdge::resistance( ) const { return m_resistance; }

FixedRectangle TerrainEdge::bounding_box( ) const { return FixedRectangle( m_v1->position( ), m_v2->position( ) ); }
