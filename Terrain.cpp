#include "Terrain.hpp"

#include "Room.hpp"
#include "Object.hpp"

#include "Player.hpp" // todo this is temp

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

Terrain::Terrain( Room * room, const varray<varray<Coordinate>> & _vertices ) : m_room( room )
{
    bool first = true;
    for_each( vertices, _vertices )
    {
        make( vertices, first ? DIRECTION_DOWN : NO_DIRECTION );
        first = false;
    }
}

void Terrain::make( varray<Coordinate> cref _positions, Direction _bound_direction )
{
    if( _positions.size( ) <= 1 )
    {
        return;
    }

    bool loop = ( _bound_direction == NO_DIRECTION );

    varray<Terrain::Vertex *> & new_vertices = m_vertices.insert_back( varray<Terrain::Vertex *>( ) );
    varray<Terrain::Edge *> & new_edges = m_edges.insert_back( varray<Terrain::Edge *>( ) );

    Terrain::Vertex * first = nullptr;
    Terrain::Vertex * last = nullptr;
    Terrain::Vertex * previous = nullptr;

    for_range( i, _positions.size( ) )
    {
        Coordinate cref coordinate = _positions[ i ];
        Terrain::Vertex * vertex = new Terrain::Vertex( this, coordinate );

        new_vertices.insert_back( vertex );

        room( )->object_grid( ).insert( vertex );

        if( i )
        {
            Assert( !is_null( previous ) );
            Terrain::Edge * edge = new Terrain::Edge( this, previous, vertex );

            new_edges.insert_back( edge );

            room( )->object_grid( ).insert( edge );
        }

        previous = vertex;

        if( loop )
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

    if( loop )
    {
        if( first && last )
        {
            Terrain::Edge * edge = new Terrain::Edge( this, last, first );

            new_edges.insert_back( edge );

            room( )->object_grid( ).insert( edge );
        }

        m_terrain.insert_back( Polygon( _positions ) );
    }
    else
    {
        Coordinate c1 = _positions.back( );
        Coordinate c2 = _positions.front( );

        if( room( ) )
        {
            FixedRectangle bounds = room( )->bounds( );

            // TODO these need to extend if terrain bounds go beyond room bounds

            if( _bound_direction == DIRECTION_UP )
            {
                c1 = bounds.top_left( );
                c2 = bounds.top_right( );
            }
            else if( _bound_direction == DIRECTION_DOWN )
            {
                c1 = bounds.bottom_right( );
                c2 = bounds.bottom_left( );
            }
            else if( _bound_direction == DIRECTION_LEFT )
            {
                c1 = bounds.top_left( );
                c2 = bounds.bottom_left( );
            }
            else if( _bound_direction == DIRECTION_RIGHT )
            {
                c1 = bounds.bottom_right( );
                c2 = bounds.top_right( );
            }

            m_terrain.insert_back( Polygon( _positions + c1 + c2 ) );
        }
    }
}

const varray<varray<Terrain::Vertex *>> & Terrain::vertices( ) const { return m_vertices; }

const varray<varray<Terrain::Edge *>> & Terrain::edges( ) const { return m_edges; }

const varray<Polygon> & Terrain::terrain( ) const { return m_terrain; }

void Terrain::crop_terrain( ) { for_each( polygon, terrain( ) ) { crop( polygon ); } }
void Terrain::fill_terrain( Color cref _color ) { for_each( polygon, terrain( ) ) { draw( _color, polygon ); } }

uint Terrain::type( const Terrain::Node * t ) const { return ( m_terrain_type_map.contains( t ) ? m_terrain_type_map.at( t ) : 0 ); }

void Terrain::typeset( const Terrain::Node * t, uint type ) { m_terrain_type_map[ t ] = type; }

void Terrain::traverse_x( const Span<Planc> & _distance_x, const function<void( Coordinate cref, const Terrain::Edge * )> & f ) const
{
    return_if( !( edges( ).size( ) ) );

    for_each( edges, edges( ) )
    {
        if( !edges.size( ) )
        {
            continue;
        }

        Terrain::Edge * edge = edges.front( );
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

    Drawing debug_overlay;

    if( Settings::get( Settings::DEBUG_PHYSICS_TERRAIN ) )
    {

        // TODO delete
        Polygon hitbox = room( )->player( )->terrain_hitbox( );


        cdec _zoom = room( )->camera( )->zoom( );

        auto draw_normal = [ & ] ( Coordinate cref base, Angle cref normal, bool left, bool right )
        {
            Planc length = ( NORMAL_LENGTH / _zoom );
            Coordinate end = base + VectorA( normal.flipped( ), length );
            Vector offset = VectorA( normal + RIGHT, half( half( length ) ) );

            debug_overlay.draw( draw_debug_color, Line( base, end ), GROUND_WIDTH, true );

            if( left ) { debug_overlay.draw( draw_debug_color, Line( end, end + offset ), GROUND_WIDTH, true ); }
            if( right ) { debug_overlay.draw( draw_debug_color, Line( end, end - offset ), GROUND_WIDTH, true ); }
        };

        for_each( edges, edges( ) )
        {
            for_each( edge, edges )
            {
                //debug_overlay.draw( draw_debug_color, edge->line( ), GROUND_WIDTH, true );
                //draw_normal( midpoint( edge->vertex1( )->position( ), edge->vertex2( )->position( ) ), edge->normal( ), true, true );

                varray<Line> bumper_lines = edge->bumpers( hitbox ).path( ).lines( );
                for_each( bumper, bumper_lines )
                {
                    debug_overlay.draw( draw_debug_color, bumper, GROUND_WIDTH, true );
                }
            }
        }

        for_each( vertices, vertices( ) )
        {
            for_each( vertex, vertices )
            {
                //debug_overlay.draw( draw_debug_color, Polygon::circle( GROUND_VERTEX_WIDTH / _zoom, vertex->position( ) ), FILLED, true );
                //draw_normal( vertex->position( ), vertex->normal( ), !is_null( vertex->edge1( ) ), !is_null( vertex->edge2( ) ) );

                varray<Line> bumper_lines = vertex->bumpers( hitbox ).path( ).lines( );
                for_each( bumper, bumper_lines )
                {
                    debug_overlay.draw( draw_debug_color, bumper, GROUND_WIDTH, true );
                }
            }
        }
    }

    return debug_overlay;
}
#endif

Terrain::Vertex::Vertex( const Terrain * parent, Coordinate cref _position ) : Terrain::Node( parent ) { m_position = _position; }

Coordinate cref Terrain::Vertex::position( ) const { return m_position; }

Terrain::Edge * Terrain::Vertex::edge1( ) const { return m_e1; }
Terrain::Edge * Terrain::Vertex::edge2( ) const { return m_e2; }

void Terrain::Vertex::edge1( Terrain::Edge * e1 ) { Assert( is_null( m_e1 ), "edge 1 has already been set" ); m_e1 = e1; }
void Terrain::Vertex::edge2( Terrain::Edge * e2 ) { Assert( is_null( m_e2 ), "edge 2 has already been set" ); m_e2 = e2; }

Angle Terrain::Vertex::normal( ) const
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

    return Angle( RIGHT ); // default to up
}

FixedRectangle Terrain::Vertex::bounding_box( ) const { return FixedRectangle( 0.0, 0.0, position( ) ); }

Terrain::Bumper Terrain::Vertex::bumpers( Polygon cref _polygon ) const
{
    if( _polygon.sides( ) )
    {
        if( edge1( ) && edge2( ) )
        {
            varray<Coordinate> bumpers;

            Line e1 = edge1( )->line( ).flipped( );
            Line e2 = edge2( )->line( ).flipped( );
            Angle angle1 = e1.angle( ).truncate( );
            Angle angle2 = e2.angle( ).truncate( );

            uint min_i = 0;
            Angle min_angle;

            uint max_i = 0;
            Angle max_angle;

            uint max_below_i1 = 0;
            Angle max_below_angle1;

            uint max_below_i2 = 0;
            Angle max_below_angle2;

            Polygon polygon = _polygon.inverted( );
            varray<Line> perimeter = polygon.perimeter( ).lines( );
            varray<Coordinate> coordinates = polygon.coordinates( );

            for_range( i, perimeter.size( ) )
            {
                Line line = perimeter[ i ];
                Angle line_angle = line.angle( ).truncate( );

                if( !i || ( line_angle < min_angle ) )
                {
                    min_angle = line_angle;
                    min_i = i;
                }

                if( !i || ( line_angle > max_angle ) )
                {
                    max_angle = line_angle;
                    max_i = i;
                }

                if( ( line_angle <= angle1 ) && ( line_angle > max_below_angle1 ) )
                {
                    max_below_angle1 = line_angle;
                    max_below_i1 = i;
                }

                if( ( line_angle <= angle2 ) && ( line_angle > max_below_angle2 ) )
                {
                    max_below_angle2 = line_angle;
                    max_below_i2 = i;
                }
            }

            uint index1 = ( ( angle1 < min_angle ) ? max_i : max_below_i1 );
            uint index2 = ( ( angle2 < min_angle ) ? max_i : max_below_i2 );

            uint min_index = min( index1, index2 );
            uint max_index = max( index1, index2 );

            for_range( index, ( max_index - min_index ) + 1 )
            {
                bumpers.insert_back( perimeter[ ( index + min_index ) ].c2( ) + position( ) );
            }

            return Terrain::Bumper( this, Path( bumpers ) );
        }
    }

    return Terrain::Bumper( this, Path( varray<Coordinate>( { position( ) } ) ) );
}

Terrain::Edge::Edge( const Terrain * parent, Terrain::Vertex * _v1, Terrain::Vertex * _v2, cbool _passable ) : Terrain::Node( parent ),
m_v1( _v1 ),
m_v2( _v2 )
{
    Assert( !is_null( m_v1 ) && !is_null( m_v2 ) );

    m_v1->edge2( this );
    m_v2->edge1( this );
}

Line Terrain::Edge::line( ) const { return Line( m_v1->position( ), m_v2->position( ) ); }

Vector Terrain::Edge::vector( ) const { return Vector( m_v1->position( ), m_v2->position( ) ); }

Terrain::Vertex * Terrain::Edge::vertex1( ) const { return m_v1; }
Terrain::Vertex * Terrain::Edge::vertex2( ) const { return m_v2; }

Angle Terrain::Edge::normal( ) const { return vector( ).angle( ) + RIGHT; }

FixedRectangle Terrain::Edge::bounding_box( ) const { return FixedRectangle( m_v1->position( ), m_v2->position( ) ); }

Terrain::Bumper Terrain::Edge::bumpers( Polygon cref _polygon ) const
{
    if( _polygon.sides( ) )
    {
        Line edge = line( ).flipped( );
        Angle edge_angle = edge.angle( ).truncate( );

        uint min_i = 0;
        Angle min_angle;

        uint max_i = 0;
        Angle max_angle;

        uint max_below_i = 0;
        Angle max_below_angle;

        Polygon polygon = _polygon.inverted( );
        varray<Line> perimeter = polygon.perimeter( ).lines( );
        varray<Coordinate> coordinates = polygon.coordinates( );

        for_range( i, perimeter.size( ) )
        {
            Line line = perimeter[ i ];
            Angle line_angle = line.angle( ).truncate( );

            if( !i || ( line_angle < min_angle ) )
            {
                min_angle = line_angle;
                min_i = i;
            }

            if( !i || ( line_angle > max_angle ) )
            {
                max_angle = line_angle;
                max_i = i;
            }

            if( ( line_angle < edge_angle ) && ( line_angle > max_below_angle ) )
            {
                max_below_angle = line_angle;
                max_below_i = i;
            }
        }

        return Terrain::Bumper( this, Path( edge + perimeter[ ( ( edge_angle < min_angle ) ? max_i : max_below_i ) ].c2( ) ) );
    }

    return Terrain::Bumper( this, Path( line( ) ) );
}
