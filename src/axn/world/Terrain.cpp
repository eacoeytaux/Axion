#include "axn/world/Terrain.hpp"

#include "axn/world/Object.hpp"
#include "axn/world/Room.hpp"

#if defined( AXN_DEBUG )
section( DEBUG_OVERLAY )
{
    var_const( Planc GROUND_WIDTH = ( 1.5 ) );
    var_const( Planc GROUND_VERTEX_WIDTH = ( 2.5 ) );
    var_const( Planc NORMAL_LENGTH = ( 10.0 ) );
}
#endif

Terrain::~Terrain( )
{
    for_each( edges, m_edges ) { safe_delete( edges ); }
    m_edges.clear( );

    for_each( vertices, m_vertices ) { safe_delete( vertices ); }
    m_vertices.clear( );
}

Terrain::Terrain( ptr<Room> in_room, vvarray<Point> cref in_vertices ) : m_room( in_room )
{
    bool first = true;
    for_each( vertices, in_vertices )
    {
        make( vertices, first ? DIRECTION_DOWN : NO_DIRECTION );
        first = false;
    }
}

void Terrain::make( varray<Point> cref in_positions, direction in_bound_direction )
{
    return_if( in_positions.size( ) <= 1 );

    bool loop = ( in_bound_direction == NO_DIRECTION );

    varray<ptr<Terrain::Vertex>> ref new_vertices = m_vertices.insert_back( varray<ptr<Terrain::Vertex>>( ) );
    varray<ptr<Terrain::Edge>> ref new_edges = m_edges.insert_back( varray<ptr<Terrain::Edge>>( ) );

    ptr<Terrain::Vertex> first = nullptr;
    ptr<Terrain::Vertex> last = nullptr;
    ptr<Terrain::Vertex> previous = nullptr;

    lambdaf( make_edge )( ptr<Terrain::Vertex> in_v1, ptr<Terrain::Vertex> in_v12 )
    {
        ptr<Terrain::Edge> edge = new Terrain::Edge( this, in_v1, in_v12 );

        room( )->object_grid( ).insert_terrain( edge );

        new_edges.insert_back( edge );
    };

    for_range( i, in_positions.size( ) )
    {
        Point cref pt = in_positions[ i ];
        ptr<Terrain::Vertex> vertex = new Terrain::Vertex( this, pt );

        new_vertices.insert_back( vertex );

        room( )->object_grid( ).insert_terrain( vertex );

        if( i )
        {
            make_edge( previous, vertex );
        }

        previous = vertex;

        if( loop )
        {
            if( !i )
            {
                first = vertex;
            }
            else if( i == ( in_positions.size( ) - 1 ) )
            {
                last = vertex;
            }
        }
    }

    if( loop )
    {
        if( !is_null( first ) && !is_null( last ) )
        {
            make_edge( last, first );
        }

        m_terrain.insert_back( Polygon( in_positions ) );
    }
    else
    {
        Point pt1 = in_positions.back( );
        Point pt2 = in_positions.front( );

        if( room( ) )
        {
            FixedRectangle bounds = room( )->bounds( );

            // TODO these need to extend if terrain bounds go beyond room bounds

            if( in_bound_direction == DIRECTION_UP )
            {
                pt1 = bounds.top_left( );
                pt2 = bounds.top_right( );
            }
            else if( in_bound_direction == DIRECTION_DOWN )
            {
                pt1 = bounds.bottom_right( );
                pt2 = bounds.bottom_left( );
            }
            else if( in_bound_direction == DIRECTION_LEFT )
            {
                pt1 = bounds.top_left( );
                pt2 = bounds.bottom_left( );
            }
            else if( in_bound_direction == DIRECTION_RIGHT )
            {
                pt1 = bounds.bottom_right( );
                pt2 = bounds.top_right( );
            }

            m_terrain.insert_back( Polygon( in_positions + pt1 + pt2 ) );
        }
    }
}

vvarray<ptr<Terrain::Vertex>> cref Terrain::vertices( ) const { return( m_vertices ); }

vvarray<ptr<Terrain::Edge>> cref Terrain::edges( ) const { return( m_edges ); }

varray<Polygon> cref Terrain::terrain( ) const { return( m_terrain ); }

void Terrain::fill_terrain( Color cref in_color ) { for_each( polygon, terrain( ) ) { draw( in_color, polygon ); } }
void Terrain::crop_terrain( ) { for_each( polygon, terrain( ) ) { add_bounds( polygon ); } }

uint Terrain::type( ptr<const Terrain::Node> in_node ) const { return( m_terrain_type_map.contains( in_node ) ? m_terrain_type_map.at( in_node ) : 0 ); }

void Terrain::typeset( ptr<const Terrain::Node> in_node, uint type ) { m_terrain_type_map[ in_node ] = type; }

void Terrain::traverse_x( Span<Planc> cref in_distance_x, function<void( Point cref in_pos, ptr<const Terrain::Edge> in_edge )> cref in_func ) const
{
    return_if( !( edges( ).size( ) ) );

    for_each( edges, edges( ) )
    {
        continue_if( !edges.size( ) );

        ptr<Terrain::Edge> edge = edges.front( );
        Planc x = edge->vertex1( )->position( ).x( ) + Random::rPlanc( in_distance_x.range( ) );

        while( edge )
        {
            Planc y = edge->line( ).y( x );
            in_func( Point( x, y ), edge );

            Planc dx = Random::rPlanc( in_distance_x );

            x += dx;
            while( edge && ( x > edge->vertex2( )->position( ).x( ) ) )
            {
                edge = edge->vertex2( )->edge2( );
            }
        }
    }
};

#if defined( AXN_DEBUG )
Drawing Terrain::debug_overlay( ) const
{
    Drawing debug_overlay;

    if( Settings::get( Settings::DEBUG_TERRAIN ) )
    {
        dec in_zoom = room( )->camera( )->zoom( );

        lambdaf( draw_normal )( Point cref in_base, Angle cref in_normal, bool in_left, bool in_right )
        {
            Planc length = ( NORMAL_LENGTH / in_zoom );
            Point end = in_base + VectorA( in_normal.flipped( ), length );
            Vector offset = VectorA( in_normal + RIGHT, half( half( length ) ) );

            debug_overlay.draw( draw_debug_color, Line( in_base, end ), GROUND_WIDTH, true );

            if( in_left ) { debug_overlay.draw( draw_debug_color, Line( end, end + offset ), GROUND_WIDTH, true ); }
            if( in_right ) { debug_overlay.draw( draw_debug_color, Line( end, end - offset ), GROUND_WIDTH, true ); }
        };

        for_each( edges, edges( ) )
        {
            for_each( edge, edges )
            {
                debug_overlay.draw( draw_debug_color, edge->line( ), GROUND_WIDTH, true );
                draw_normal( midpoint( edge->vertex1( )->position( ), edge->vertex2( )->position( ) ), edge->normal( ), true, true );
            }
        }

        for_each( vertices, vertices( ) )
        {
            for_each( vertex, vertices )
            {
                debug_overlay.draw( draw_debug_color, Polygon::circle( GROUND_VERTEX_WIDTH / in_zoom, vertex->position( ) ), FILLED, true );
                draw_normal( vertex->position( ), vertex->normal( ), !is_null( vertex->edge1( ) ), !is_null( vertex->edge2( ) ) );
            }
        }
    }

    return( debug_overlay );
}
#endif

Terrain::Vertex::Vertex( ptr<const Terrain> parent, Point cref in_position ) : Terrain::Node( parent ) { m_position = in_position; }

Point cref Terrain::Vertex::position( ) const { return( m_position ); }

ptr<Terrain::Edge> Terrain::Vertex::edge1( ) const { return( m_e1 ); }
ptr<Terrain::Edge> Terrain::Vertex::edge2( ) const { return( m_e2 ); }

void Terrain::Vertex::edge1( ptr<Terrain::Edge> in_e1 ) { Assert( is_null( m_e1 ), "edge 1 has already been set" ); m_e1 = in_e1; }
void Terrain::Vertex::edge2( ptr<Terrain::Edge> in_e2 ) { Assert( is_null( m_e2 ), "edge 2 has already been set" ); m_e2 = in_e2; }

Angle Terrain::Vertex::normal( ) const
{
    if( m_e1 && m_e2 )
    {
        return( half( m_e1->normal( ) + m_e2->normal( ) ) );
    }
    else if( m_e1 )
    {
        return( m_e1->normal( ) );
    }
    else if( m_e2 )
    {
        return( m_e2->normal( ) );
    }

    return( Angle( RIGHT ) ); // default to up
}

FixedRectangle Terrain::Vertex::bounding_box( ) const { return( FixedRectangle( 0.0, 0.0, position( ) ) ); }

Terrain::Bumper Terrain::Vertex::bumpers( Hitbox cref in_hitbox ) const
{
    if( in_hitbox.polygon( ).sides( ) )
    {
        if( edge1( ) && edge2( ) )
        {
            varray<Point> bumpers;

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

            Polygon polygon = in_hitbox.polygon( false ).inverted( );
            varray<Line> perimeter = polygon.perimeter( ).lines( );
            varray<Point> pts = polygon.points( );

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
                bumpers.insert_back( perimeter[ ( index + min_index ) ].pt2( ) + position( ) );
            }

            return( Terrain::Bumper( this, Path( bumpers ) ) );
        }
        else
        {
            // todo?
        }
    }

    return( Terrain::Bumper( this, ( in_hitbox.polygon( false ).inverted( ) + position( ) ).perimeter( ) ) );
}

Terrain::Edge::Edge( ptr<const Terrain> in_parent, ptr<Terrain::Vertex> in_v1, ptr<Terrain::Vertex> in_v2, bool in_passable ) : Terrain::Node( in_parent ),
m_v1( in_v1 ),
m_v2( in_v2 )
{
    Assert( !is_null( m_v1 ) && !is_null( m_v2 ) );

    m_v1->edge2( this );
    m_v2->edge1( this );
}

Line Terrain::Edge::line( ) const { return( Line( m_v1->position( ), m_v2->position( ) ) ); }

Vector Terrain::Edge::vector( ) const { return( Vector( m_v1->position( ), m_v2->position( ) ) ); }

ptr<Terrain::Vertex> Terrain::Edge::vertex1( ) const { return( m_v1 ); }
ptr<Terrain::Vertex> Terrain::Edge::vertex2( ) const { return( m_v2 ); }

Angle Terrain::Edge::normal( ) const { return( vector( ).angle( ) + RIGHT ); }

FixedRectangle Terrain::Edge::bounding_box( ) const { return( FixedRectangle( m_v1->position( ), m_v2->position( ) ) ); }

Terrain::Bumper Terrain::Edge::bumpers( Hitbox cref in_hitbox ) const
{
    if( in_hitbox.polygon( ).sides( ) )
    {
        Line edge = line( ).flipped( );
        Angle edge_angle = edge.angle( ).truncate( );

        uint min_i = 0;
        Angle min_angle;

        uint max_i = 0;
        Angle max_angle;

        uint max_below_i = 0;
        Angle max_below_angle;

        Polygon polygon = in_hitbox.polygon( false ).inverted( );
        varray<Line> perimeter = polygon.perimeter( ).lines( );
        varray<Point> pts = polygon.points( );

        for_range( i, perimeter.size( ) )
        {
            Line line = perimeter[ i ];
            Angle line_angle = line.angle( ).truncate( );

            if( ( !i ) || ( line_angle < min_angle ) )
            {
                min_angle = line_angle;
                min_i = i;
            }

            if( ( !i ) || ( line_angle > max_angle ) )
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

        return( Terrain::Bumper( this, Path( edge + perimeter[ ( ( edge_angle < min_angle ) ? max_i : max_below_i ) ].pt2( ) ) ) );
    }

    return( Terrain::Bumper( this, Path( line( ) ) ) );
}
