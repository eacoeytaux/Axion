#include "Terrain.hpp"

Terrain::~Terrain( )
{
    for_each( edge, m_edges ) safe_delete( edge );
    m_edges.clear( );

    for_each( vertex, m_vertices ) safe_delete( vertex );
    m_vertices.clear( );
}

Terrain::Terrain( World * world, const varray<Coordinate> & _vertices ) : Object( world )
{
    gravity_ratio( 0.0 );

    for_each( v, _vertices )
    {
        make_vertex( v );
    }

    if( m_vertices.size( ) )
    {
        for_range( i, m_vertices.size( ) - 1 )
        {
            make_edge( m_vertices[ i ], m_vertices[ i + 1 ] );
        }
    }
}

TerrainVertex * Terrain::make_vertex( const Coordinate & _position )
{
    TerrainVertex * vertex( new TerrainVertex( _position ) );
    m_vertices.insert_back( vertex );
    return vertex;
}

TerrainEdge * Terrain::make_edge( TerrainVertex * v1, TerrainVertex * v2 )
{
    TerrainEdge * edge( new TerrainEdge( v1, v2 ) );
    m_edges.insert_back( edge );
    v1->edge2( edge );
    v2->edge1( edge );
    return edge;
}
const varray<TerrainVertex *> & Terrain::vertices( ) const { return m_vertices; }

const varray<TerrainEdge *> & Terrain::edges( ) const { return m_edges; }

void Terrain::traverse_x( const Span<Planc> & _distance_x, const function<void( const Coordinate &, const TerrainEdge * )> & f ) const
{
    if( !( edges( ).size( ) ) )
        return;

    TerrainEdge * edge = edges( ).front( );
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
};

#ifdef AXN_DEBUG
Drawing Terrain::debug_overlay( ) const
{
    Drawing debug_overlay;

    for_each( edge, edges( ) )
    {
        debug_overlay.draw( MAGENTA, edge->line( ), 1.0 );
    }

    return debug_overlay;
}
#endif
