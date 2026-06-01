#include "axn/world/Object.hpp"

#include "axn/world/World.hpp"
#include "axn/world/Terrain.hpp"

#if defined( AXN_DEBUG )
uint Object::total_objects = 0;
#endif

#if defined( AXN_DEBUG )
section( DEBUG_OVERLAY )
{
    var_const( Planc HITBOX_THICKNESS = ( 1.5 ) );
    var_const( Planc DOT_RADIUS = ( HITBOX_THICKNESS ) );

    var_const( Planc TRACKING_THICKNESS = ( DOT_RADIUS ) );
    var_const( dec TRACKING_ALPHA_START = ( 0.75 ) );
    var_const( dec TRACKING_ALPHA_END = ( 0.0 ) );

    var_const( Planc VELOCITY_THICKNESS = ( HITBOX_THICKNESS ) );
    var_const( Planc VELOCITY_ARROW_LENGTH = ( 10.0 ) );
    var_const( Planc VELOCITY_MAGNITUDE_MINIMUM = ( 0.1 ) );
    var_const( Planc VELOCITY_SCALE = ( 3.0 ) );
}
#endif

Object::~Object( )
{
    Assert( m_initialized, "never initialized" );

    #if defined( AXN_DEBUG )
    total_objects--;
    #endif

    for_each( object, m_movement_subscribers )
    {
        object->unsubscribe_to_movement( this );
    } m_movement_subscribers.clear( );

    for_each( object, m_movement_subscriptions )
    {
        object->remove_movement_subscriber( this );
    } m_movement_subscriptions.clear( );
}

Object::Object( ptr<Room> in_room ) : Matter( ORIGIN ), m_room( in_room ) { init( ); }
Object::Object( ptr<Room> in_room, dec in_z ) : Matter( ORIGIN ), m_room( in_room ) { z( in_z ); init( ); }
Object::Object( ptr<Room> in_room, Point cref in_position ) : Matter( in_position ), m_room( in_room ) { init( ); }
Object::Object( ptr<Room> in_room, Point cref in_position, dec in_z ) : Matter( in_position ), m_room( in_room ) { z( in_z ); init( ); }
Object::Object( ptr<Room> in_room, Point cref in_position, Vector cref in_velocity ) : Matter( in_position ), m_room( in_room ) { velocity( in_velocity ); init( ); }
Object::Object( ptr<Room> in_room, Point cref in_position, Vector cref in_velocity, dec in_z ) : Matter( in_position ), m_room( in_room ) { z( in_z ); velocity( in_velocity ); init( ); }

void Object::init( )
{
    Assert( !m_initialized );
    m_initialized = true;

    #if defined( AXN_DEBUG )
    total_objects++;
    #endif

    m_age = 0;

    foreground( z( ) > 1.0 );
    background( z( ) < 1.0 );
}

Drawing Object::trajection_drawing( Planc cref in_distance, Color cref in_color, dec in_alpha_start, dec in_alpha_end ) const
{
    Drawing trajection;

    if( !grounded( ) )
    {
        Point pt;
        Vector v = velocity( );
        Planc d = in_distance;
        dec a = in_alpha_start;

        while( ( d > 0.0 ) && v.has_magnitude( ) )
        {
            v += gravity( );
            v *= one_minus( friction_resistance( ) );

            if( d > v.magnitude( ) )
            {
                d -= v.magnitude( );
            }
            else
            {
                v.magnitude( d );
                d = 0.0;
            }

            dec d_a = min<dec>( a, ( in_alpha_start - in_alpha_end ) * ( v.magnitude( ) / in_distance ) );

            trajection.draw( Color( in_color, a ), Color( in_color, a - d_a ), Line( pt, pt + v ) );
            a -= d_a;

            pt += v;
        }
    }

    return( trajection );
}

void Object::render( )
{
    Visible::center( position( ) );
    Visible::render( );
}

void Object::render_object( )
{
    if( needs_render( ) )
    {
        render( );
    }
}

void Object::update( )
{
    while( m_last_positions.size( ) && ( m_last_positions.size( ) >= m_last_position_count ) )
    {
        m_last_positions.pop( );
    }

    if( m_last_position_count )
    {
        m_last_positions.push( { position( ) } );
    }

    update_movement( );
}

void Object::update_object( )
{
    return_if( m_last_world_age_update == room( )->age( ) );

    m_last_world_age_update = room( )->age( );
    m_age++;

    if( marked_to_delete( ) && m_to_deletion_countdown.tick( ) && m_deletion_countdown.tick( ) )
    {
        mark_deleted( );
    }
    else
    {
        update( );
    }
}

void Object::trim_velocity( )
{
    if( stationary( ) )
    {
        Matter::velocity( V0 );
        return;
    }

    Vector velocity = Matter::velocity( );

    if( grounded( ) && terrain_bound( ) )
    {
        if( !passing_terrain( ) || !ground( )->passable( ) )
        {
            if( velocity.has_magnitude( ) )
            {
                // TODO use in_semi here and elsewhere
                if( abs( ground( )->normal( ).min_distance( velocity.angle( ) ) ) >= RIGHT )
                {
                    Angle a1, a2;
                    Angle va = velocity.angle( );

                    varray<Line> bumper_lines = ground( )->bumpers( terrain_hitbox( ) ).path( ).lines( );
                    for_each( bumper_line, bumper_lines )
                    {
                        if( bumper_line.on( position( ) ) )
                        {
                            a1 = bumper_line.angle( );
                            a2 = a1.flipped( );
                        }
                    }

                    Angle a = va.closest( { a1, a2 } );

                    velocity.flatten( a );
                }
                else
                {
                    clear_ground( );
                }
            }
        }
    }

    velocity *= one_minus( friction_resistance( ) );

    Matter::velocity( velocity );
}

void Object::update_velocity( )
{
    Vector velocity = Matter::velocity( );

    if( !grounded( ) || !terrain_bound( ) )
    {
        if( m_gravity_change.has_magnitude( ) )
        {
            velocity += gravity( );
        }
    }

    Matter::velocity( velocity );
}

void Object::update_movement( )
{
    // todo shouldn't this only keep track of previous one?
    // otherwise with a small enough loop you could fall
    // through terrain that you shouldn't be able to
    uset<ptr<Terrain::Node>> checked_edges;

    if( grounded( ) )
    {
        checked_edges.insert( ground( ) );

        // TODO if( m_passing_terrain && m_ground->passable( ) )
        {
            // ground = nullptr;
        }
    }

    update_velocity( );

    uset<ptr<Object>> all_collided_objects = { this };

    dec remaining_percentage = 1.0;

    do_while
    {
        trim_velocity( );

        Assert( is_num( remaining_percentage ) );

        Point center = position( );

        Vector adjusted_velocity = velocity( ) * remaining_percentage;

        Vector movement = Vector( center, center + adjusted_velocity );
        movement *= check_movement( movement );

        ptr<Terrain::Node> next_ground = ground( );
        Terrain::Bumper next_bumper = ground_bumper( );

        if( movement.has_magnitude( ) )
        {
            // todo move all this into check_movement

            // if objects goes through walls, don't bother
            if( terrain_bound( ) )
            {
                ptr<Terrain::Node> ground_left = nullptr;
                ptr<Terrain::Node> ground_right = nullptr;

                if( grounded( ) )
                {
                    Path bumper = ground( )->bumpers( terrain_hitbox( ) ).path( );
                    varray<Line> bumper_lines = bumper.lines( );

                    for_each( bumper_line, bumper_lines )
                    {
                        if( bumper_line.on( position( ) ) )
                        {
                            if( Line( movement ).in_bounds( bumper_line.pt1( ) ) )
                            {
                                if( center != bumper_line.pt1( ) )
                                {
                                    movement = Vector( center, bumper_line.pt1( ) );
                                }
                                else
                                {
                                    // TODO move to next bumper!
                                }
                            }
                            else if( Line( movement ).in_bounds( bumper_line.pt2( ) ) )
                            {
                                if( center != bumper_line.pt2( ) )
                                {
                                    movement = Vector( center, bumper_line.pt2( ) );
                                }
                                else
                                {
                                    // TODO move to next bumper!
                                }
                            }
                        }
                    }

                    Point bumper_start = bumper_lines.front( ).pt1( );
                    Point bumper_end = bumper_lines.back( ).pt2( );

                    // check if currently sitting on vertex if so pick edge in direction of movement for ground
                    //if( ( movement.dx( ) > 0.0 ) && ( center.x( ) == m_ground->vertex2( )->position( ).x( ) ) )
                    //{
                    //    ground = m_ground->vertex2( )->edge2( );
                    //}
                    //else if( ( movement.dx( ) > 0.0 ) && ( center.x( ) == m_ground->vertex1( )->position( ).x( ) ) )
                    //{
                    //    ground = m_ground->vertex1( )->edge1( );
                    //}

                    if( grounded( ) )
                    {
                        //ground_left = ( m_ground->vertex1( ) ? m_ground->vertex1( )->edge1( ) : nullptr );
                        //ground_right = ( m_ground->vertex2( ) ? m_ground->vertex2( )->edge2( ) : nullptr );
                    }
                }

                // check if object is colliding with any edges
                for_each( terrain_node, room( )->terrain_in_range( terrain_hitbox( ).polygon( ).bounds( ).union_with( ( terrain_hitbox( ).polygon( ) + movement ).bounds( ) ) ) )
                {
                    // skip if terrain_edge is current ground or adjacent as these are already dealt with
                    continue_if( ( checked_edges.contains( terrain_node ) ) ||
                                 ( terrain_node == ground( ) ) ||
                                 ( terrain_node == ground_left ) ||
                                 ( terrain_node == ground_right ) );

                    checked_edges.insert( terrain_node );

                    Terrain::Bumper bumper = terrain_node->bumpers( terrain_hitbox( ) );

                    for_each( bumper_line, bumper.path( ).lines( ) )
                    {
                        if( Line( movement ).intersects( bumper_line ) )
                        {
                            next_ground = terrain_node;
                            next_bumper = bumper;

                            movement = Vector( center, Line( movement ).intersection( bumper_line ) );
                        }
                    }
                }
            }
        }

        // check if object is moving to connecting edge
        if( grounded( ) && ( next_ground == ground( ) ) )
        {
            //if( ground_bumper( ) )

                //if( ( movement.dx( ) > 0.0 ) && ( center + movement ).x( ) > m_ground->line( ).upper_bound_x( ) )
            {
                //    movement = Vector( center, m_ground->vertex2( )->position( ) + VectorY( half( space( ).bound_height( ) ) ) );

                //    if( m_ground->vertex2( ) )
                //    {
                //        next_ground = m_ground->vertex2( )->edge2( );
                //    }
                //    else
                //    {
                //        next_ground = nullptr;
                //    }
                //}
                //else if( ( movement.dx( ) < 0.0 ) && ( center + movement ).x( ) < m_ground->line( ).lower_bound_x( ) )
                //{
                //    movement = Vector( center, m_ground->vertex1( )->position( ) + VectorY( half( space( ).bound_height( ) ) ) );

                //    if( m_ground->vertex1( ) )
                //    {
                //        next_ground = m_ground->vertex1( )->edge1( );
                //    }
                //    else
                //    {
                //        next_ground = nullptr;
                //    }
            }
        }

        if( interactive( ) )
        {
            struct ObjectCollision { ptr<Object> object; Line path; };
            list<ObjectCollision> collided_objects;

            varray<ptr<Object>> objects;

            for_each( hitbox, hitboxes( ) )
            {
                FixedRectangle hitbox_bounds = hitbox.polygon( ).bounds( );
                objects.insert_back( room( )->objects_in_range( hitbox_bounds.union_with( hitbox_bounds + movement ) ) );
            }

            for_each( object, objects )
            {
                continue_if( all_collided_objects.contains( object ) );

                for_each( object_hitbox, object->hitboxes( ) )
                {
                    Hitbox expanded_hitbox = object_hitbox.expanded( hitbox( ) );
                    Polygon expanded_hitbox_poly = expanded_hitbox.polygon( );

                    varray<Line> intersections = expanded_hitbox_poly.intersection( movement );

                    if( intersections.size( ) )
                    {
                        ObjectCollision collision;

                        collision.object = object;
                        collision.path = intersections.front( );

                        collided_objects.insert_back( collision );
                    }
                }
            }

            if( collided_objects.size( ) )
            {
                struct CollisionDistance { CollisionDistance( Point cref in_pt ) : origin( in_pt ) { } Point origin; bool operator( )( ObjectCollision cref in_pt1, ObjectCollision cref in_pt2 ) { return( origin.closer_than( in_pt1.path.pt1( ), in_pt2.path.pt1( ) ) ); } };

                collided_objects.sort( CollisionDistance( position( ) ) );

                for_each( collision, collided_objects )
                {
                    bool b1 = collide( collision.object );
                    bool b2 = collision.object->collide( this );

                    //if( b1 || b2 )
                    {
                        all_collided_objects.insert( collision.object );
                        movement = Vector( position( ), collision.path.pt1( ) );
                        break;
                    }
                }
            }
        }

        move( movement );

        ground( next_ground, next_bumper );

        if( adjusted_velocity.has_magnitude( ) )
        {
            remaining_percentage *= one_minus( movement.magnitude( ) / adjusted_velocity.magnitude( ) );
        }
        else
        {
            remaining_percentage = 0.0;
        }
    }
    while( is_pos( remaining_percentage ) );
}

dec Object::check_movement( Vector cref in_velocity )
{
    return( 1.0 ); // only changes when overidden
}

void Object::move( Vector cref in_movement )
{
    if( in_movement.has_magnitude( ) )
    {
        position( position( ) + in_movement );

        if( z( ) )
        {
            FixedRectangle world_bounds = room( )->bounds( z( ) );

            if( !world_bounds.contains( position( ) ) )
            {
                out_of_bounds( );
            }
        }

        for_each( object, m_movement_subscribers )
        {
            object->react_to_movement( this, in_movement );
        }
    }
}

Point Object::position( ) const { return( Matter::position( ) ); }
void Object::position( Point cref in_position )
{
    if( in_position != position( ) )
    {
        if( interactive( ) )
        {
            room( )->object_grid( ).remove( this );
        }

        if( m_last_positions.size( ) )
        {
            m_last_positions.back( ).insert_back( position( ) );
        }

        Visible::center( in_position );
        Matter::position( in_position );

        if( interactive( ) )
        {
            room( )->object_grid( ).insert( this );
        }
    }
}

Planc Object::width( ) const { return( space( ).bound_width( true ) ); }
Planc Object::height( ) const { return( space( ).bound_height( true ) ); }

void Object::rotate( Angle cref in_a, Point cref in_origin ) { m_rotation += in_a; move( position( ).rotate( in_a, in_origin ) - position( ) ); }

bool Object::background( ) const { return( m_background ); }
void Object::background( bool in_background ) { m_background = ( in_background || ( z( ) < 1.0 ) ); }

bool Object::foreground( ) const { return( m_foreground ); }
void Object::foreground( bool in_foreground ) { m_foreground = ( in_foreground || ( z( ) > 1.0 ) ); }

bool Object::interactive( ) const { return( m_interactive && !( background( ) || foreground( ) ) ); }
void Object::interactive( bool in_interactive )
{
    if( m_interactive != in_interactive )
    {
        m_interactive = in_interactive;

        if( !m_interactive )
        {
            room( )->object_grid( ).remove( this );
        }
    }
}

bool Object::stationary( ) const
{
    return( m_stationary );
}

void Object::stationary( bool in_stationary )
{
    m_stationary = in_stationary;

    if( m_stationary )
    {
        velocity( V0 );
    }
}

void Object::mark_deleted( ticks in_deletion_countdown )
{
    mark_to_delete( 0, in_deletion_countdown );

    if( !in_deletion_countdown )
    {
        m_deleted = true;
    }
}

void Object::mark_to_delete( ticks in_countdown, ticks in_deletion_countdown )
{
    if( m_marked_to_delete )
    {
        m_to_deletion_countdown.reset( min( in_countdown, m_deletion_countdown.remaining( ) ) );
        m_deletion_countdown.reset( min( in_deletion_countdown, m_deletion_countdown.remaining( ) ) );
    }
    else
    {
        m_marked_to_delete = true;
        m_to_deletion_countdown.reset( in_countdown );
        m_deletion_countdown.reset( in_deletion_countdown );
    }
}

dec Object::air_resistance( ) const { return( room( )->air_resistance( position( ) ) * m_air_resistance ); }
void Object::air_resistance( dec in_air_resistance_ratio ) { m_air_resistance = in_air_resistance_ratio; }

dec Object::friction_resistance( ) const
{
    if( grounded( ) )
    {
        return( 0.1 );
        // TODO return( m_ground->resistance( ) );
    }

    return( air_resistance( ) );
}

ptr<Terrain::Node> Object::ground( ) const { return( m_ground ); }
Terrain::Bumper Object::ground_bumper( ) const { return( m_ground_bumper ); }

void Object::ground( ptr<Terrain::Node> in_ground, Terrain::Bumper cref in_bumper )
{
    m_ground = in_ground;
    m_ground_bumper = in_bumper;
}

bool Object::sticks( ptr<Terrain::Node> in_node ) { return( false ); }

void Object::out_of_bounds( ) { mark_deleted( ); }

bool Object::collide( ptr<Object> in_object ) { return( false ); }

void Object::react_to_movement( ptr<Object> in_object, Vector cref in_movement ) { Assert( m_movement_subscriptions.contains( in_object ), "not subscribed to object's movement" ); }

bool Object::terrain_bound( ) const { return( m_terrain_bound ); }
void Object::terrain_bound( bool in_terrain_bound )
{
    m_terrain_bound = in_terrain_bound;
    if( !m_terrain_bound )
    {
        m_ground = nullptr;
    }
}

Vector Object::gravity( ) const
{
    return( room( )->gravity( position( ) ).rotate( m_gravity_change.angle( ) ) * m_gravity_change.magnitude( ) );
}

Hitbox Object::hitbox( ) const
{
    return( Hitbox( space( ), position( ) ) );
}

varray<Hitbox> Object::hitboxes( ) const
{
    return{ hitbox( ) };
}

FixedRectangle Object::hitboxes_bounds( bool in_include_terrain ) const
{
    FixedRectangle bounds = FixedRectangle( position( ) );

    for_each( hitbox, hitboxes( ) )
    {
        bounds.union_with( hitbox.polygon( ).bounds( ) );
    }

    return( bounds );
}

Hitbox Object::terrain_hitbox( ) const
{
    return( Hitbox( space( ), position( ) ) );
}

void Object::track_position( uint in_count )
{
    m_last_position_count = in_count;
}

Point Object::last_position( uint in_past ) const
{
    if( in_past ) // todo should 0 return current position?
    {
        uint position_size = m_last_positions.size( );

        if( in_past > position_size )
        {
            return( m_last_positions.front( )[ 0 ] );
        }
        else
        {
            queue<varray<Point>> last_positions = m_last_positions;

            for_range( i, in_past - 1 ) { last_positions.pop( ); }

            return( last_positions.front( )[ 0 ] );
        }
    }

    return( position( ) );
}

void Object::subscribe_to_movement( ptr<Object> in_object )
{
    m_movement_subscriptions.insert( in_object );
    in_object->add_movement_subscriber( this );
}

void Object::unsubscribe_to_movement( ptr<Object> in_object )
{
    m_movement_subscriptions.remove( in_object );
    in_object->remove_movement_subscriber( this );
}

uset<ptr<Object>> cref Object::movement_subscribers( ) const
{
    return( m_movement_subscribers );
}

uset<ptr<Object>> cref Object::movement_subscriptions( ) const
{
    return( m_movement_subscriptions );
}

void Object::add_movement_subscriber( ptr<Object> in_object )
{
    m_movement_subscribers.insert( in_object );
}

void Object::remove_movement_subscriber( ptr<Object> in_object )
{
    m_movement_subscribers.remove( in_object );
}

#if defined( AXN_DEBUG )
Drawing Object::debug_overlay( ) const
{
    static_once{ Assert( TRACKING_THICKNESS <= x2( DOT_RADIUS ) ); }

    Drawing debug_overlay;

    if( is_one( z( ) ) )
    {
        if( Settings::get( Settings::DEBUG_PHYSICS ) )
        {
            // position tracking
            uint position_count = m_last_positions.size( );

            if( position_count )
            {
                queue<varray<Point>> last_positions = m_last_positions;

                dec d_a = ( ( TRACKING_ALPHA_START - TRACKING_ALPHA_END ) / dec( position_count ) );
                dec a = ( TRACKING_ALPHA_END );

                Point pt_prev = last_positions.front( )[ 0 ];

                for_range( i, position_count )
                {
                    varray<Point> positions = last_positions.pop( );

                    for_each( pt, positions )
                    {
                        if( pt != pt_prev )
                        {
                            debug_overlay.draw( draw_debug_color.a( a ), draw_debug_color.a( a + d_a ), ( Line( pt_prev, pt ) - position( ) ), TRACKING_THICKNESS );
                        }

                        pt_prev = pt;
                    }

                    a += d_a;
                }

                debug_overlay.draw( draw_debug_color.a( a ), ( Line( pt_prev - position( ), ORIGIN ) ), TRACKING_THICKNESS );
            }

            // center
            {
                debug_overlay.draw( draw_debug_color, Polygon::circle( DOT_RADIUS ), FILLED );
            }

            // velocity
            if( velocity( ).magnitude( ) >= VELOCITY_MAGNITUDE_MINIMUM )
            {
                debug_overlay.draw( draw_debug_color, ( velocity( ) * VELOCITY_SCALE ).origin( ORIGIN ), min( VELOCITY_ARROW_LENGTH, ( velocity( ).magnitude( ) ) ), VELOCITY_THICKNESS, true );
            }

            // hitboxes
            {
                for_each( hitbox, hitboxes( ) )
                {
                    debug_overlay.draw( draw_debug_color, hitbox.polygon( false ), HITBOX_THICKNESS, true );
                }
            }
        }

        if( Settings::get( Settings::DEBUG_TERRAIN ) )
        {
            if( room( ) && room( )->terrain( ) )
            {
                debug_overlay.draw( room( )->terrain( )->draw_debug_color, terrain_hitbox( ).polygon( false ), HITBOX_THICKNESS, true );
            }
        }
    }

    return( debug_overlay );
}
#endif
