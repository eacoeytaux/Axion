#include "Object.hpp"
#include "World.hpp"
#include "Terrain.hpp"

#if defined ( AXN_DEBUG )
uint Object::total_objects = 0;
#endif

Object::~Object( )
{
    Assert( m_initialized, "never initialized!" );

    #if defined ( AXN_DEBUG )
    --total_objects;
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

Object::Object( Room * room ) : Matter( ORIGIN ), m_room( room ) { init( ); }
Object::Object( Room * room, const dec _z ) : Matter( ORIGIN ), m_room( room ) { z( _z ); init( ); }
Object::Object( Room * room, Coordinate cref _position ) : Matter( _position ), m_room( room ) { init( ); }
Object::Object( Room * room, Coordinate cref _position, const dec _z ) : Matter( _position ), m_room( room ) { z( _z ); init( ); }
Object::Object( Room * room, Coordinate cref _position, Vector cref _velocity ) : Matter( _position ), m_room( room ) { velocity( _velocity ); init( ); }
Object::Object( Room * room, Coordinate cref _position, Vector cref _velocity, const dec _z ) : Matter( _position ), m_room( room ) { z( _z ); velocity( _velocity ); init( ); }

void Object::init( )
{
    Assert( !m_initialized );
    m_initialized = true;

    #if defined ( AXN_DEBUG )
    ++total_objects;
    #endif

    m_age = 0;

    foreground( z( ) > 1.0 );
    background( z( ) < 1.0 );
}

Drawing Object::trajection_drawing( Planc cref _distance, Color cref _color, cdec _alpha_start, cdec _alpha_end ) const
{
    Drawing trajection;

    if( !Object::ground( ) )
    {
        Coordinate c;
        Vector v = velocity( );
        Planc d = _distance;
        dec a = _alpha_start;

        while( ( d > 0.0 ) && v.has_magnitude( ) )
        {
            v += ( gravity( ) );
            v *= ( 1.0 - friction_resistance( ) );

            if( d > v.magnitude( ) )
            {
                d -= v.magnitude( );
            }
            else
            {
                v.magnitude( d );
                d = 0.0;
            }

            dec d_a = min<dec>( a, ( _alpha_start - _alpha_end ) * ( v.magnitude( ) / _distance ) );

            trajection.draw( Color( _color, a ), Color( _color, a - d_a ), Line( c, c + v ) );
            a -= d_a;

            c += v;
        }
    }

    return trajection;
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
    if( m_last_world_age_update == room( )->age( ) )
    {
        return;
    }

    if( marked_to_delete( ) )
    {
        mark_deleted( );
    }
    else
    {
        update( );
    }

    m_last_world_age_update = room( )->age( );
    ++m_age;
}

void Object::update_velocity( )
{
    if( stationary( ) )
    {
        Matter::velocity( V0 );
        return;
    }

    Vector velocity = Matter::velocity( );

    if( ground( ) && terrain_bound( ) )
    {
        if( !m_passing_terrain || !ground( )->passable( ) )
        {
            if( velocity.has_magnitude( ) )
            {
                Angle va = velocity.angle( );

                Angle a1, a2;

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

                velocity.rotate_to( a );
            }
        }
    }
    else if( !ground( ) && m_gravity_change.has_magnitude( ) )
    {
        velocity += gravity( );
    }

    velocity *= ( 1.0 - friction_resistance( ) );

    Matter::velocity( velocity );
}

void Object::update_movement( )
{
    if( stationary( ) )
    {
        return;
    }

    Terrain::Node * ground = Object::ground( );

    // todo shouldn't this only keep track of previous one?
    // otherwise with a small enough loop you could fall
    // through terrain that you shouldn't be able to
    uset<Terrain::Node *> checked_edges;

    if( ground )
    {
        checked_edges.insert( ground );

        // TODO if( m_passing_terrain && m_ground->passable( ) )
        {
            // ground = nullptr;
        }
    }

    uset<Object *> all_collided_objects;

    dec remaining_percentage = 1.0;
    while( ( greater( remaining_percentage, 0.0 ) && !isnan( remaining_percentage ) ) )
    {
        update_velocity( );
        Vector velocity = Object::velocity( ) * remaining_percentage;

        Coordinate center = position( );

        Vector movement = Vector( center, center + velocity );
        movement *= check_movement( movement );

        if( !movement.has_magnitude( ) )
        {
            // TODO still need to check collisions if still no?
            break;
        }

        Terrain::Node * next_ground = ground;
        Terrain::Bumper next_bumper = ground_bumper( );

        // todo move all this into check_movement

        // if objects decs through walls don't bother
        if( terrain_bound( ) )
        {
            Terrain::Node * ground_left = nullptr;
            Terrain::Node * ground_right = nullptr;

            if( ground )
            {
                Path bumper = ground->bumpers( terrain_hitbox( ) ).path( );
                varray<Line> bumper_lines = bumper.lines( );

                for_each( bumper_line, bumper_lines )
                {
                    if( bumper_line.on( position( ) ) && bumper_line.in_bounds( position( ) ) )
                    {
                        if( Line( movement ).in_bounds( bumper_line.c1( ) ) )
                        {

                        }
                        else if( Line( movement ).in_bounds( bumper_line.c2( ) ) )
                        {

                        }
                    }
                }

                Coordinate bumper_start = bumper_lines.front( ).c1( );
                Coordinate bumper_end = bumper_lines.back( ).c2( );

                // check if currently sitting on vertex if so pick edge in direction of movement for ground
                //if( ( movement.dx( ) > 0.0 ) && ( center.x( ) == m_ground->vertex2( )->position( ).x( ) ) )
                //{
                //    ground = m_ground->vertex2( )->edge2( );
                //}
                //else if( ( movement.dx( ) > 0.0 ) && ( center.x( ) == m_ground->vertex1( )->position( ).x( ) ) )
                //{
                //    ground = m_ground->vertex1( )->edge1( );
                //}

                if( ground )
                {
                    //ground_left = ( m_ground->vertex1( ) ? m_ground->vertex1( )->edge1( ) : nullptr );
                    //ground_right = ( m_ground->vertex2( ) ? m_ground->vertex2( )->edge2( ) : nullptr );
                }
            }

            // check if object is colliding with any edges
            for_each( terrain_node, room( )->terrain_in_range( terrain_hitbox( ).bounds( ).union_with( ( terrain_hitbox( ) + movement ).bounds( ) ) ) )
            {
                // skip if terrain_edge is current ground or adjacent as these are already dealt with
                if( checked_edges.contains( terrain_node ) ||
                    ( terrain_node == ground ) ||
                    ( terrain_node == ground_left ) ||
                    ( terrain_node == ground_right ) )
                {
                    continue;
                }

                checked_edges.insert( terrain_node );

                Line movement_line = movement;
                Terrain::Bumper bumper = terrain_node->bumpers( terrain_hitbox( ) + center );

                for_each( bumper_line, bumper.path( ).lines( ) )
                {
                    if( movement_line.intersects( bumper_line + center ) )
                    {
                        next_ground = terrain_node;

                        movement = Vector( center, movement_line.intersection( bumper_line + center ) );
                        next_bumper = bumper;
                    }
                }
            }
        }

        // check if object is moving to connecting edge
        if( ground && ( next_ground == ground ) )
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
            struct ObjectCollision
            {
                Object * object;
                Line line;
            };

            list<ObjectCollision> collided_objects;

            varray<Object *> objects = room( )->objects_in_range( hitbox( ).bounds( ).union_with( ( hitbox( ) + movement ).bounds( ) ) );

            Line movement_line( movement );
            for_each( object, objects )
            {
                if( ( this == object ) || all_collided_objects.contains( object ) )
                {
                    continue;
                }

                FixedRectangle hitbox = object->hitbox( ).bounds( );
                hitbox.expand_width( width( ) );
                hitbox.expand_height( height( ) );

                varray<Line> intersections;
                if( hitbox.area( ) )
                {
                    intersections = hitbox.intersection( movement );
                }
                else
                {
                    if( hitbox.width( ) )
                    {
                        Line hitbox_line( hitbox.center( ) - VectorX( half( hitbox.width( ) ) ),
                                          hitbox.center( ) + VectorX( half( hitbox.width( ) ) ) );

                        if( movement_line.intersects( hitbox_line ) )
                        {
                            intersections = { Line( movement.origin( ), movement_line.intersection( hitbox_line ) ) };
                        }
                    }
                    else if( hitbox.height( ) )
                    {
                        Line hitbox_line( hitbox.center( ) - VectorY( half( hitbox.height( ) ) ),
                                          hitbox.center( ) + VectorY( half( hitbox.height( ) ) ) );

                        if( movement_line.intersects( hitbox_line ) )
                        {
                            intersections = { Line( movement.origin( ), movement_line.intersection( hitbox_line ) ) };
                        }
                    }
                    else
                    {
                        if( movement_line.on( hitbox.center( ) ) && movement_line.in_bounds( hitbox.center( ) ) )
                        {
                            intersections = { Line( movement.origin( ), hitbox.center( ) ) };
                        }
                    }
                }

                if( intersections.size( ) )
                {
                    ObjectCollision collision;
                    collision.object = object;
                    collision.line = intersections.front( );
                    collided_objects.insert_back( collision );
                }
            }

            if( collided_objects.size( ) )
            {
                struct CollisionDistance { CollisionDistance( Coordinate cref c ) : origin( c ) { } Coordinate origin; bool operator( )( ObjectCollision cref c1, ObjectCollision cref c2 ) { return origin.closer_than( c1.line.c1( ), c2.line.c1( ) ); } };

                collided_objects.sort( CollisionDistance( position( ) ) );

                for_each( collision, collided_objects )
                {
                    if( collide( collision.object ) || collision.object->collide( this ) )
                    {
                        all_collided_objects.insert( collision.object );
                        movement = Vector( position( ), collision.line.c1( ) );
                        break;
                    }
                }
            }
        }

        move( movement );

        if( ground != next_ground )
        {
            Object::ground( next_ground, next_bumper );
        }

        if( velocity.has_magnitude( ) )
        {
            remaining_percentage *= ( 1.0 - ( movement.magnitude( ) / velocity.magnitude( ) ) );
        }
        else
        {
            remaining_percentage = 0.0;
        }
    }
}

dec Object::check_movement( Vector cref _velocity )
{
    return 1.0;
}

void Object::move( Vector cref _movement )
{
    if( _movement.has_magnitude( ) )
    {
        position( position( ) + _movement );

        if( z( ) )
        {
            FixedRectangle world_bounds = room( )->bounds( );

            // todo overshoots
            world_bounds.width( world_bounds.width( ) / ( z( ) * z( ) ) );
            world_bounds.height( world_bounds.height( ) / ( z( ) * z( ) ) );

            if( !world_bounds.contains( position( ) ) )
            {
                out_of_bounds( );
            }
        }

        for_each( object, m_movement_subscribers )
        {
            object->react_to_movement( this, _movement );
        }
    }
}

Coordinate Object::position( ) const { return Matter::position( ); }
void Object::position( Coordinate cref _position )
{
    if( _position != position( ) )
    {
        if( interactive( ) )
        {
            room( )->object_grid( ).remove( this );
        }

        if( m_last_positions.size( ) )
        {
            m_last_positions.back( ).insert_back( position( ) );
        }

        Visible::center( _position );
        Matter::position( _position );

        if( interactive( ) )
        {
            room( )->object_grid( ).insert( this );
        }
    }
}

Planc Object::width( ) const { return space( ).bound_width( true ); }
Planc Object::height( ) const { return space( ).bound_height( true ); }

bool Object::foreground( ) const { return m_foreground; }
void Object::foreground( cbool _foreground ) { m_foreground = _foreground || ( z( ) > 1.0 ); }

bool Object::background( ) const { return m_background; }
void Object::background( cbool _background ) { m_background = _background || ( z( ) < 1.0 ); }

bool Object::interactive( ) const { return m_interactive; }
void Object::interactive( cbool _interactive )
{
    m_interactive = _interactive;

    if( !m_interactive )
    {
        room( )->object_grid( ).remove( this );
    }
}

bool Object::stationary( ) const
{
    return m_stationary;
}

void Object::stationary( cbool _stationary )
{
    m_stationary = _stationary;

    if( m_stationary )
    {
        velocity( V0 );
    }
}

dec Object::air_resistance( ) const { return ( room( )->air_resistance( position( ) ) * m_air_resistance ); }
void Object::air_resistance( cdec _air_resistance_ratio ) { m_air_resistance = _air_resistance_ratio; }

dec Object::friction_resistance( ) const
{
    if( m_ground )
    {
        return 0.1;
        // TODO return m_ground->resistance( );
    }

    return air_resistance( );
}

Terrain::Node * Object::ground( ) const { return m_ground; }
Terrain::Bumper Object::ground_bumper( ) const { return m_ground_bumper; }

void Object::ground( Terrain::Node * ground, Terrain::Bumper cref _bumper ) { m_ground = ground; m_ground_bumper = _bumper; }

bool Object::sticks( Terrain::Node * node ) { return false; }

void Object::out_of_bounds( ) { mark_deleted( ); }

bool Object::collide( Object * object ) { return false; }

void Object::react_to_movement( Object * object, Vector cref _v ) { Assert( m_movement_subscriptions.contains( object ), "not subscribed to object's movement" ); }

bool Object::terrain_bound( ) const { return m_terrain_bound; }
void Object::terrain_bound( cbool _terrain_bound )
{
    m_terrain_bound = _terrain_bound;
    if( !m_terrain_bound )
    {
        m_ground = nullptr;
    }
}

Vector Object::gravity( ) const
{
    return ( room( )->gravity( position( ) ).rotate( m_gravity_change.angle( ) ) * m_gravity_change.magnitude( ) );
}

Hitbox Object::hitbox( ) const
{
    return Hitbox( space( ) );
}

varray<Hitbox> Object::hitboxes( ) const
{
    return { hitbox( ) };
}

FixedRectangle Object::hitboxes_bounds( cbool _include_terrain ) const
{
    FixedRectangle bounds = FixedRectangle( position( ) );

    for_each( hitbox, hitboxes( ) )
    {
        bounds.union_with( hitbox.bounds( ) );
    }

    return bounds;
}

Hitbox Object::terrain_hitbox( ) const
{
    return Hitbox( space( ) );
}

void Object::track_position( uint count )
{
    m_last_position_count = count;
}

Coordinate Object::last_position( uint past ) const
{
    if( past ) // todo should 0 return current position?
    {
        uint position_size = m_last_positions.size( );

        if( past > position_size )
        {
            return ( m_last_positions.front( )[ 0 ] );
        }
        else
        {
            queue<varray<Coordinate>> last_positions = m_last_positions;

            for_range( i, ( past - 1 ) ) { last_positions.pop( ); }

            return ( last_positions.front( )[ 0 ] );
        }
    }

    return position( );
}

void Object::subscribe_to_movement( Object * object )
{
    m_movement_subscriptions.insert( object );
    object->add_movement_subscriber( this );
}

void Object::unsubscribe_to_movement( Object * object )
{
    m_movement_subscriptions.remove( object );
    object->remove_movement_subscriber( this );
}

void Object::add_movement_subscriber( Object * object )
{
    m_movement_subscribers.insert( object );
}

void Object::remove_movement_subscriber( Object * object )
{
    m_movement_subscribers.remove( object );
}

#if defined ( AXN_DEBUG )
Drawing Object::debug_overlay( ) const
{
    cPlanc HITBOX_THICKNESS = ( 1.5 );
    cPlanc DOT_RADIUS = ( HITBOX_THICKNESS );
    cPlanc TRACKING_THICKNESS = ( DOT_RADIUS ); Assert( TRACKING_THICKNESS <= ( DOT_RADIUS * 2.0 ) );
    cdec TRACKING_ALPHA_START = ( 0.75 );
    cdec TRACKING_ALPHA_END = ( 0.25 );
    cPlanc VELOCITY_THICKNESS = ( HITBOX_THICKNESS );
    cPlanc VELOCITY_ARROW_LENGTH = ( 10.0 );
    cPlanc VELOCITY_MAGNITUDE_MINIMUM = ( 1.0 );
    cPlanc VELOCITY_SCALE = ( 3.0 );

    Drawing debug_overlay;

    if( z( ) == 1.0 )
    {
        if( Settings::get( Settings::DEBUG_PHYSICS ) )
        {
            // position tracking
            uint position_count = m_last_positions.size( );

            if( position_count )
            {
                queue<varray<Coordinate>> last_positions = m_last_positions;

                dec d_a = ( ( TRACKING_ALPHA_START - TRACKING_ALPHA_END ) / (dec)position_count );
                dec a = ( TRACKING_ALPHA_END );

                Coordinate c_prev = last_positions.front( )[ 0 ];

                for_range( i, position_count )
                {
                    varray<Coordinate> positions = last_positions.pop( );

                    for_each( c, positions )
                    {
                        if( c != c_prev )
                        {
                            debug_overlay.draw( draw_debug_color.a( a ), ( Line( c_prev, c ) - position( ) ), TRACKING_THICKNESS );
                        }

                        c_prev = c;
                    }

                    a += d_a;
                }

                debug_overlay.draw( draw_debug_color.a( a ), ( Line( c_prev - position( ), ORIGIN ) ), TRACKING_THICKNESS );
            }

            // center
            {
                debug_overlay.draw( draw_debug_color, Polygon::circle( DOT_RADIUS ), FILLED );
            }

            // velocity
            if( velocity( ).magnitude( ) >= VELOCITY_MAGNITUDE_MINIMUM )
            {
                debug_overlay.draw( draw_debug_color, ( velocity( ) * VELOCITY_SCALE ).origin( ORIGIN ), VELOCITY_ARROW_LENGTH, VELOCITY_THICKNESS, true );
            }

            // hitboxes
            {
                for_each( hitbox, hitboxes( ) )
                {
                    debug_overlay.draw( draw_debug_color, hitbox, HITBOX_THICKNESS, true );
                }
            }
        }

        if( Settings::get( Settings::DEBUG_PHYSICS_TERRAIN ) )
        {
            if( room( ) && room( )->terrain( ) )
            {
                debug_overlay.draw( room( )->terrain( )->draw_debug_color, terrain_hitbox( ), HITBOX_THICKNESS, true );
            }
        }
    }

    return debug_overlay;
}
#endif
