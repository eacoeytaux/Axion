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

section( MOVEMENT )
{
    // TODO shouldn't this be part of the standard point equality check?
    var_const( Planc BUMPER_EPSILON = ( 0.01 ) ); // how close the center has to be to count as touching a bumper

    // velocity flattened onto a surface is at a right angle to its normal, rounding puts it a hair either side
    // so only count as moving away from a surface when clearly moving away
    var_const( Angle LEAVE_SURFACE_EPSILON = ( Angle( 0.001 ) ) );
    var_const( uint MAX_MOVEMENT_STEPS = ( 100 ) ); // safety net so update_movement can never loop forever

    // floating point drift means the center is rarely exactly on a bumper line
    bool on_bumper( Line cref in_line, Point cref in_pt ) { return( in_line.closest( in_pt ).distance_to( in_pt ) < BUMPER_EPSILON ); }
    bool on_bumper( Path cref in_path, Point cref in_pt ) { for_each( line, in_path.lines( ) ) { return_true_if( on_bumper( line, in_pt ) ); } return( false ); }

    // the stretch of bumper the center can slide along from in_pt in the direction of in_movement
    // returns an empty line if the bumper ends in that direction
    Line bumper_direction( Path cref in_path, Point cref in_pt, Vector cref in_movement )
    {
        Line along;
        Angle along_distance = RIGHT; // anything at or past a right angle would be moving backwards

        for_each( line, in_path.lines( ) )
        {
            continue_if( !on_bumper( line, in_pt ) );

            varray<Point> ends = { line.pt1( ), line.pt2( ) };

            for_each( end, ends )
            {
                continue_if( end.distance_to( in_pt ) < BUMPER_EPSILON );

                Line direction( in_pt, end );
                Angle distance = in_movement.angle( ).min_distance( direction.angle( ) ).abs( );

                if( distance < along_distance )
                {
                    along = direction;
                    along_distance = distance;
                }
            }
        }

        return( along );
    }

    // the bumper line under in_pt that lines up best with in_velocity (either way along it), empty line if not on the bumper
    Line surface_line( Path cref in_path, Point cref in_pt, Vector cref in_velocity )
    {
        Line surface;
        Angle surface_distance;
        bool found = false;

        for_each( line, in_path.lines( ) )
        {
            continue_if( is_zero( line.length( ) ) || !on_bumper( line, in_pt ) );

            Angle distance = min( in_velocity.angle( ).min_distance( line.angle( ) ).abs( ), in_velocity.angle( ).min_distance( line.angle( ).flipped( ) ).abs( ) );

            if( !found || ( distance < surface_distance ) )
            {
                surface = line;
                surface_distance = distance;
                found = true;
            }
        }

        return( surface );
    }

    // moving away from a surface with in_normal, as opposed to into it or along it
    bool moving_away( Angle cref in_normal, Vector cref in_velocity ) { return( abs( in_normal.min_distance( in_velocity.angle( ) ) ) < ( RIGHT - LEAVE_SURFACE_EPSILON ) ); }

    // moving into a surface with in_normal, as opposed to along it or away from it
    bool moving_into( Angle cref in_normal, Vector cref in_velocity ) { return( abs( in_normal.min_distance( in_velocity.angle( ) ) ) > ( RIGHT + LEAVE_SURFACE_EPSILON ) ); }

    // where in_path first touches in_bumper_line, Line::intersection returns the path's start for any overlapping parallel lines
    Point first_contact( Line cref in_path, Line cref in_bumper_line )
    {
        return_if( !in_path.parallel( in_bumper_line ), in_path.intersection( in_bumper_line ) );
        return_if( on_bumper( in_bumper_line, in_path.pt1( ) ), in_path.pt1( ) );

        Point pt1 = in_bumper_line.pt1( );
        Point pt2 = in_bumper_line.pt2( );

        return_if( !in_path.on( pt1 ), pt2 );
        return_if( !in_path.on( pt2 ), pt1 );

        return( in_path.pt1( ).closer_than( pt1, pt2 ) ? pt1 : pt2 );
    }
}

Object::~Object( )
{
    Assert( m_initialized, "never initialized" );

    #if defined( AXN_DEBUG )
    total_objects--;
    #endif

    // loop over copies, unsubscribing removes entries from these sets and would break the loop
    uset<ptr<Object>> subscribers = m_movement_subscribers;
    for_each( object, subscribers )
    {
        object->unsubscribe_to_movement( this );
    } m_movement_subscribers.clear( );

    uset<ptr<Object>> subscriptions = m_movement_subscriptions;
    for_each( object, subscriptions )
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
        // start this frame's positions where the object is now, position( ) adds the rest as it moves
        m_last_positions.push( ).push( position( ) );
    }

    update_movement( );
}

void Object::update_object( )
{
    return_if( m_last_world_age_update == room( )->age( ) );

    if( marked_to_delete( ) && m_to_deletion_countdown.tick( ) && m_deletion_countdown.tick( ) )
    {
        mark_deleted( );
    }
    else
    {
        update( );
    }

    m_last_world_age_update = room( )->age( );
    m_age++;
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
                // judge leaving against the bumper line actually under the object, a vertex's own normal
                // is only an average of its edges and is wrong for most of its corner
                Line surface = surface_line( ground( )->bumpers( terrain_hitbox( ) ).path( ), position( ), velocity );
                Angle surface_normal = ( !is_zero( surface.length( ) ) ? ground( )->bumper_normal( surface ) : ground( )->normal( ) );

                if( !moving_away( surface_normal, velocity ) )
                {
                    // keep only the part of the velocity along the surface, anything pushing into it is stopped
                    // (landing straight down onto flat ground leaves no velocity at all)
                    if( !is_zero( surface.length( ) ) )
                    {
                        velocity.flatten( surface.angle( ) );
                    }
                    else
                    {
                        // drifted off the bumper or it's a single point, slide along the surface
                        velocity.flatten( ground( )->normal( ) + RIGHT );
                    }
                }
                else
                {
                    clear_ground( );
                }
            }
        }
    }

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

    velocity *= one_minus( friction_resistance( ) );

    Matter::velocity( velocity );
}

void Object::update_movement( )
{
    // TODO if( m_passing_terrain && m_ground->passable( ) ) ground = nullptr;

    update_velocity( );

    dec remaining_percentage = 1.0;
    uint steps = 0;

    uset<ptr<Object>> all_collided_objects = { this };

    // only the ground most recently left is skipped
    ptr<Terrain::Node> previous_ground;

    do_while
    {
        ptr<Terrain::Node> ground_before_trim = ground( );

        trim_velocity( );

        if( ground( ) != ground_before_trim )
        {
            previous_ground = ground_before_trim;
        }

        Assert( is_num( remaining_percentage ) );

        Point center = position( );

        Vector adjusted_velocity = velocity( ) * remaining_percentage;

        Vector movement = Vector( center, center + adjusted_velocity );
        movement *= check_movement( movement );

        ptr<Terrain::Node> next_ground = ground( );
        Terrain::Bumper next_bumper = ground_bumper( );

        bool collided = false; // touched a new object this step

        // if objects goes through walls, don't bother
        if( movement.has_magnitude( ) && terrain_bound( ) )
        {
            if( grounded( ) )
            {
                Path bumper = ground( )->bumpers( terrain_hitbox( ) ).path( );

                if( on_bumper( bumper, center ) )
                {
                    Line along = bumper_direction( bumper, center, movement );

                    if( !is_zero( along.length( ) ) )
                    {
                        // stop at the end of this bumper line, the next step follows the next one
                        if( movement.magnitude( ) > along.length( ) )
                        {
                            movement = Vector( center, along.pt2( ) );
                        }
                    }
                    else
                    {
                        // reached the end of the ground's bumper, step onto the neighbor that continues it
                        // if none do the object leaves the ground (e.g. walking off a ledge)
                        next_ground = nullptr;
                        next_bumper = Terrain::Bumper( );

                        // a vertex between two edges at the same angle has no bumper to slide along, so also look one node further
                        varray<ptr<Terrain::Node>> neighbors;

                        for_each( neighbor, varray<ptr<Terrain::Node>>( { ground( )->front( ), ground( )->back( ) } ) )
                        {
                            continue_if( is_null( neighbor ) );

                            neighbors.insert_back( neighbor );
                            neighbors.insert_back( neighbor->front( ) );
                            neighbors.insert_back( neighbor->back( ) );
                        }

                        for_each( neighbor, neighbors )
                        {
                            continue_if( is_null( neighbor ) || ( neighbor == ground( ) ) );

                            Terrain::Bumper neighbor_bumper = neighbor->bumpers( terrain_hitbox( ) );

                            if( !is_zero( bumper_direction( neighbor_bumper.path( ), center, movement ).length( ) ) )
                            {
                                next_ground = neighbor;
                                next_bumper = neighbor_bumper;

                                // the next step slides along the new ground
                                movement = Vector( center, center );
                                break;
                            }
                        }
                    }
                }
            }

            // check if object is colliding with any terrain, the closest hit wins
            if( movement.has_magnitude( ) )
            {
                Line path = Line( movement );

                bool hit = false;
                Planc hit_distance = P0;
                Point hit_point;

                for_each( terrain_node, room( )->terrain_in_range( terrain_hitbox( ).polygon( ).bounds( ).union_with( ( terrain_hitbox( ).polygon( ) + movement ).bounds( ) ) ) )
                {
                    // current ground is dealt with above and the previous ground was just left
                    continue_if( ( terrain_node == ground( ) ) || ( terrain_node == previous_ground ) );

                    Terrain::Bumper bumper = terrain_node->bumpers( terrain_hitbox( ) );

                    for_each( bumper_line, bumper.path( ).lines( ) )
                    {
                        // a single point bumper (a vertex between edges at the same angle), the edges on either side cover it
                        continue_if( is_zero( bumper_line.length( ) ) );

                        continue_if( !path.intersects( bumper_line ) );

                        Point contact = first_contact( path, bumper_line );
                        Planc distance = center.distance_to( contact );

                        // already touching it: only a hit when moving into it, sliding along a neighbor or leaving it isn't
                        // (otherwise the neighbors at a vertex keep taking turns being the ground and the object never moves)
                        continue_if( ( distance < BUMPER_EPSILON ) && !moving_into( terrain_node->bumper_normal( bumper_line ), movement ) );

                        if( !hit || ( distance < hit_distance ) )
                        {
                            hit = true;
                            hit_distance = distance;
                            hit_point = contact;

                            next_ground = terrain_node;
                            next_bumper = bumper;
                        }
                    }
                }

                if( hit )
                {
                    movement = Vector( center, hit_point );
                }
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

                    if( !is_empty( intersections ) )
                    {
                        ObjectCollision collision;

                        collision.object = object;
                        collision.path = intersections.front( );

                        collided_objects.insert_back( collision );
                    }
                }
            }

            if( !is_empty( collided_objects ) )
            {
                struct CollisionDistance { CollisionDistance( Point cref in_pt ) : origin( in_pt ) { } Point origin; bool operator( )( ObjectCollision cref in_pt1, ObjectCollision cref in_pt2 ) { return( origin.closer_than( in_pt1.path.pt1( ), in_pt2.path.pt1( ) ) ); } };

                collided_objects.sort( CollisionDistance( position( ) ) );

                for_each( collision, collided_objects )
                {
                    bool b1 = collide( collision.object );
                    bool b2 = collision.object->collide( this );

                    //if( b1 || b2 )
                    {
                        Planc terrain_movement = movement.magnitude( );

                        all_collided_objects.insert( collision.object );
                        movement = Vector( position( ), collision.path.pt1( ) );
                        collided = true;

                        // stopped by the object before reaching the terrain it was going to land on
                        if( movement.magnitude( ) < ( terrain_movement - BUMPER_EPSILON ) )
                        {
                            next_ground = ground( );
                            next_bumper = ground_bumper( );
                        }

                        break;
                    }
                }
            }
        }

        move( movement );

        bool ground_changed = ( next_ground != ground( ) );

        if( ground_changed && grounded( ) )
        {
            previous_ground = ground( );
        }

        ground( next_ground, next_bumper );

        if( adjusted_velocity.has_magnitude( ) )
        {
            remaining_percentage *= one_minus( movement.magnitude( ) / adjusted_velocity.magnitude( ) );
        }
        else
        {
            remaining_percentage = 0.0;
        }

        // nothing moved and nothing changed, another step would do exactly the same thing
        if( !movement.has_magnitude( ) && !ground_changed && !collided )
        {
            remaining_percentage = 0.0;
        }

        steps++;

        if( steps > MAX_MOVEMENT_STEPS )
        {
            #if defined( AXN_DEBUG )
            Assert( "too many movement steps!" );
            #endif
            break;
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
            if( !room_bounds_z( ).contains( position( ) ) )
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

        if( !is_empty( m_last_positions ) )
        {
            m_last_positions.back( ).push( position( ) );
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

Line Object::ground_bumper_line( ) const
{
    Assert( grounded( ), "not grounded" );

    for_each( line, ground( )->bumpers( terrain_hitbox( ) ).path( ).lines( ) )
    {
        return_if( on_bumper( line, position( ) ), line );
    }

    // drifted off the bumper or the bumper is a single point, use the surface direction instead
    // oriented the same way as an edge's bumper line
    return( Line( position( ), position( ) + VectorA( ground( )->normal( ) + RIGHT, 1.0 ) ) );
}

void Object::ground( ptr<Terrain::Node> in_ground, Terrain::Bumper cref in_bumper )
{
    // TODO no need for two objects passed when bumper points to ground node
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
    return( last_positions( in_past ).front( ) );
}

// m_last_positions holds one queue per frame, oldest at the front and the current frame at the back
// in_past = 1 is the current frame (starting where the object was 1 update ago), 2 the frame before, ...
// asking further back than is tracked gives the oldest frame
queue<Point> Object::last_positions( uint in_past ) const
{
    if( in_past && !is_empty( m_last_positions ) )
    {
        uint frame_count = m_last_positions.size( );

        if( in_past >= frame_count )
        {
            return( m_last_positions.front( ) );
        }
        else
        {
            queue<queue<Point>> last_positions = m_last_positions;

            for_range( i, frame_count - in_past ) { last_positions.pop( ); }

            return( last_positions.front( ) );
        }
    }

    // todo allow queue to be initialized by vector
    queue<Point> ret;
    ret.push( position( ) );

    return( ret );
}

queue<queue<Point>> cref Object::last_positions( ) const
{
    return( m_last_positions );
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
                queue<queue<Point>> last_positions = m_last_positions;

                dec d_a = ( ( TRACKING_ALPHA_START - TRACKING_ALPHA_END ) / dec( position_count ) );
                dec a = ( TRACKING_ALPHA_END );

                Point pt_prev = last_positions.front( ).front( );

                for_range( i, position_count )
                {
                    queue<Point> positions = last_positions.pop( );

                    while( positions.size( ) )
                    {
                        Point pt = positions.pop( );

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
