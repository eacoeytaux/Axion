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

section( BUMPERS )
{
    // C! index of the (inverted) hitbox point that rests against in_edge, the bumper is the edge offset by this point
    // C! when a side of the hitbox is parallel to the edge both of its points rest against it, the one furthest along the edge wins
    // C! edges and vertices both use this so their bumpers always meet end to end
    uint support_index( varray<Point> cref in_pts, ptr<const Terrain::Edge> in_edge )
    {
        Vector normal = VectorA( in_edge->normal( ), 1.0 );
        Vector direction = in_edge->vector( );

        uint support = 0;

        for_range( i, in_pts.size( ) )
        {
            dec depth = Vector( ORIGIN, in_pts[ i ] ).dot( normal );
            dec support_depth = Vector( ORIGIN, in_pts[ support ] ).dot( normal );

            if( equal( depth, support_depth ) )
            {
                if( Vector( ORIGIN, in_pts[ i ] ).dot( direction ) > Vector( ORIGIN, in_pts[ support ] ).dot( direction ) )
                {
                    support = i;
                }
            }
            else if( depth > support_depth )
            {
                support = i;
            }
        }

        return( support );
    }
}

varray<Point> Terrain::Node::bumper_polygon( Hitbox cref in_hitbox )
{
    // C! the hitbox turned inside out (every point negated), same as polygon( false ).inverted( ) without building a second polygon
    varray<Point> poly = in_hitbox.polygon( false ).points( );
    for_each( pt, poly ) { pt = -pt; }

    return( poly );
}

Angle Terrain::Node::bumper_normal( Line cref in_bumper_line ) const
{
    // C! bumper lines are the node pushed out by the hitbox, so they always face away from the node
    Angle normal = in_bumper_line.angle( ) + RIGHT;

    Vector normal_vector = VectorA( normal, 1.0 );
    Vector outward( center( ), midpoint( in_bumper_line.pt1( ), in_bumper_line.pt2( ) ) );

    dec facing = ( normal_vector.dx( ) * outward.dx( ) ) + ( normal_vector.dy( ) * outward.dy( ) );

    return_if( is_neg( facing ), normal.flipped( ) );
    return( normal );
}

Terrain::Vertex::Vertex( ptr<const Terrain> parent, Point cref in_position ) : Terrain::Node( parent ) { m_position = in_position; }

Point cref Terrain::Vertex::position( ) const { return( m_position ); }

ptr<Terrain::Edge> Terrain::Vertex::edge1( ) const { return( m_e1 ); }

void Terrain::Vertex::edge1( ptr<Terrain::Edge> in_e1 ) { Assert( ( is_null( m_e1 ) ), "edge 1 has already been set" ); m_e1 = in_e1; }

ptr<Terrain::Edge> Terrain::Vertex::edge2( ) const { return( m_e2 ); }

void Terrain::Vertex::edge2( ptr<Terrain::Edge> in_e2 ) { Assert( ( is_null( m_e2 ) ), "edge 2 has already been set" ); m_e2 = in_e2; }

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

Terrain::Bumper Terrain::Vertex::bumpers( varray<Point> cref in_poly ) const
{
    if( !is_empty( in_poly ) )
    {
        if( edge1( ) && edge2( ) )
        {
            // C! the hitbox rolls around the vertex from the point resting on edge1 to the point resting on edge2
            // C! this includes any hitbox sides in between, so a side of the hitbox can sit on the vertex not just a corner
            varray<Point> cref pts = in_poly;
            uint pt_count = pts.size( );

            // C! the resting point follows the normal around the hitbox, so walk the hitbox the way the normal turns
            Angle turn = edge1( )->normal( ).min_distance( edge2( )->normal( ) );

            // C! a concave corner (a valley) has no bumper of its own: the edges' bumpers cross and that is the corner,
            // C! an arc here would be inside the terrain and objects landing on it would sink in
            if( is_pos( turn ) )
            {
                return( Terrain::Bumper( this, Path( ) ) );
            }

            uint index1 = support_index( pts, edge1( ) );
            uint index2 = support_index( pts, edge2( ) );

            bool forward = clockwise( pts );

            varray<Point> bumpers;
            uint i = index1;

            bumpers.insert_back( pts[ i ] + position( ) );

            while_max( ( i != index2 ), pt_count )
            {
                i = ( forward ? ( ( i + 1 ) % pt_count ) : ( ( i + pt_count - 1 ) % pt_count ) );
                bumpers.insert_back( pts[ i ] + position( ) );
            }

            return( Terrain::Bumper( this, Path( bumpers ) ) );
        }
    }

    // C! only one edge (the end of an open terrain), the hitbox can rest against the vertex from any side
    varray<Point> around;
    for_each_const( pt, in_poly ) { around.insert_back( pt + position( ) ); }

    return( Terrain::Bumper( this, Path( around, true ) ) );
}

Terrain::Edge::Edge( ptr<const Terrain> in_parent, ptr<Terrain::Vertex> in_v1, ptr<Terrain::Vertex> in_v2, bool in_passable ) : Terrain::Node( in_parent, in_passable ),
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

Terrain::Bumper Terrain::Edge::bumpers( varray<Point> cref in_poly ) const
{
    if( !is_empty( in_poly ) )
    {
        return( Terrain::Bumper( this, Path( line( ).flipped( ) + in_poly[ support_index( in_poly, this ) ] ) ) );
    }

    return( Terrain::Bumper( this, Path( line( ) ) ) );
}

Terrain::~Terrain( )
{
    for_each( edges, m_edges ) { safe_delete( edges ); }
    m_edges.clear( );

    for_each( vertices, m_vertices ) { safe_delete( vertices ); }
    m_vertices.clear( );
}

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

vvarray<ptr<Terrain::Vertex>> cref Terrain::vertices( ) const { return( m_vertices ); }

vvarray<ptr<Terrain::Edge>> cref Terrain::edges( ) const { return( m_edges ); }

varray<Polygon> cref Terrain::terrain( ) const { return( m_terrain ); }

uint Terrain::type( ptr<const Terrain::Node> in_node ) const { return( m_terrain_type_map.contains( in_node ) ? m_terrain_type_map.at( in_node ) : 0 ); }

void Terrain::fill_color( Color cref in_color, bool in_fill ) { m_fill_color = in_color; if( in_fill ) { fill( ); } }

void Terrain::traverse_x( span<Planc> cref in_distance_x, function<void( Point cref in_pos, ptr<const Terrain::Edge> in_edge )> cref in_func ) const
{
    return_if( is_empty( edges( ) ) );

    for_each( edges, edges( ) )
    {
        continue_if( is_empty( edges ) );

        ptr<Terrain::Edge> edge = edges.front( );
        Planc x = edge->vertex1( )->position( ).x( ) + Random::rPlanc( in_distance_x.range( ) );

        while( !is_null( edge ) )
        {
            in_func( Point( x, edge->line( ).y( x ) ), edge );

            x += Random::rPlanc( in_distance_x );
            while( !is_null( edge ) && ( x > edge->vertex2( )->position( ).x( ) ) )
            {
                edge = edge->vertex2( )->edge2( );
            }
        }
    }
};

Terrain::Terrain( ptr<Room> in_room, vvarray<Point> cref in_vertices ) : m_room( in_room )
{
    bool first = true;
    for_each( vertices, in_vertices )
    {
        make( vertices, first ? DIRECTION_DOWN : NO_DIRECTION );
        first = false;
    }
}

void Terrain::typeset( ptr<const Terrain::Node> in_node, uint type ) { m_terrain_type_map[ in_node ] = type; }

void Terrain::make( varray<Point> cref in_positions, Direction in_bound_direction )
{
    return_if( in_positions.size( ) <= 1 );

    bool loop = ( in_bound_direction == NO_DIRECTION );

    varray<ptr<Terrain::Vertex>> ref new_vertices = m_vertices.insert_back( varray<ptr<Terrain::Vertex>>( ) );
    varray<ptr<Terrain::Edge>> ref new_edges = m_edges.insert_back( varray<ptr<Terrain::Edge>>( ) );

    ptr<Terrain::Vertex> first;
    ptr<Terrain::Vertex> last;
    ptr<Terrain::Vertex> previous;

    lambdaf( make_edge )( ptr<Terrain::Vertex> in_v1, ptr<Terrain::Vertex> in_v2 )
    {
        ptr<Terrain::Edge> edge = new Terrain::Edge( this, in_v1, in_v2 );

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
            // C! the room's bounds, stretched to reach any terrain point outside them, so the fill closes around all of the terrain
            FixedRectangle bounds = room( )->bounds( );
            for_each_const( pt, in_positions ) { bounds.union_with( FixedRectangle( pt ) ); }

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

void Terrain::fill( ) { for_each( poly, terrain( ) ) { draw( fill_color( ), poly ); } }
void Terrain::crop( ) { for_each( poly, terrain( ) ) { add_bounds( poly ); } }
