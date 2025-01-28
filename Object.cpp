#include "Object.hpp"
#include "World.hpp"
#include "Terrain.hpp"

#if defined ( AXN_DEBUG )
uint Object::total_objects = 0;
#endif

Object::~Object( )
{
    #if defined ( AXN_DEBUG )
    --total_objects;
    #endif

    for_each( object, m_movement_subscribers )
    {
        object->unsubscribe_to_movement( this );
    }
    m_movement_subscribers.clear( );

    for_each( object, m_movement_subscriptions )
    {
        object->remove_movement_subscriber( this );
    }
    m_movement_subscriptions.clear( );
}

Object::Object( Room * room ) : Matter( ORIGIN ), m_room( room ) { init( ); }

Object::Object( Room * room, Coordinate cref _position, Vector cref _velocity ) : Matter( Vector( _velocity ).origin( _position ) ), m_room( room ) { init( ); }

Object::Object( Room * room, Vector cref _position_velocity ) : Matter( _position_velocity ), m_room( room ) { init( ); }

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
            v += ( GRAVITY * gravity_ratio( ) );
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
    if( m_last_position_count )
    {
        uint last_positions_size = m_last_positions.size( );
        if( last_positions_size < m_last_position_count )
        {
            m_last_positions.insert_back( position( ) );
        }
        else
        {
            m_last_positions[ m_last_position_index++ % last_positions_size ] = position( );
        }
    }

    update_movement( );
}

void Object::update_object( )
{
    if( m_last_world_age_update == room( )->age( ) )
    {
        return;
    }

    update( );

    if( marked_to_delete( ) )
    {
        mark_deleted( );
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

    if( terrain_boundaries( ) && m_ground )
    {
        if( !m_passing_terrain || !m_ground->passable( ) )
        {
            if( velocity.has_magnitude( ) )
            {
                velocity.flatten( m_ground->line( ).angle( ).flip( ) );
            }
        }
    }
    else if( !m_ground && m_gravity_ratio )
    {
        velocity += ( GRAVITY * m_gravity_ratio );
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

    uset<TerrainEdge *> checked_edges;
    if( m_ground )
    {
        checked_edges.insert( m_ground );

        if( m_passing_terrain && m_ground->passable( ) )
        {
            m_ground = nullptr;
        }
    }

    uset<Object *> all_collided_objects;

    dec remaining_percentage = 1.0;
    while( ( greater( remaining_percentage, 0.0 ) && !isnan( remaining_percentage ) ) )
    {
        update_velocity( );
        Vector velocity = Object::velocity( ) * remaining_percentage;
        if( !velocity.has_magnitude( ) )
        {
            // break;
        }

        Coordinate center = position( );

        Vector movement = Vector( center, center + velocity );

        dec movement_percentage = 1.0;

        TerrainEdge * next_ground = m_ground;

        // if objects decs through walls don't bother
        if( terrain_boundaries( ) || !room( )->terrain( ) )
        {
            TerrainEdge * ground_left = nullptr;
            TerrainEdge * ground_right = nullptr;

            if( m_ground )
            {
                // check if currently sitting on vertex if so pick edge in direction of movement for ground
                if( ( movement.dx( ) > 0.0 ) && ( center.x( ) == m_ground->vertex2( )->position( ).x( ) ) )
                {
                    m_ground = m_ground->vertex2( )->edge2( );
                }
                else if( ( movement.dx( ) > 0.0 ) && ( center.x( ) == m_ground->vertex1( )->position( ).x( ) ) )
                {
                    m_ground = m_ground->vertex1( )->edge1( );
                }

                if( m_ground )
                {
                    ground_left = ( m_ground->vertex1( ) ? m_ground->vertex1( )->edge1( ) : nullptr );
                    ground_right = ( m_ground->vertex2( ) ? m_ground->vertex2( )->edge2( ) : nullptr );
                }
            }

            // check if object is colliding with any edges
            for_each( terrain_node, room( )->terrain_in_range( hit_box( ).union_with( hit_box( ) + movement ) ) )
            {
                if( TerrainEdge * terrain_edge = dynamic_cast<TerrainEdge *>( terrain_node ) )
                {
                    // skip if terrain_edge is current ground or adjacent as these are already dealt with
                    if( checked_edges.contains( terrain_edge ) ||
                        ( terrain_edge == m_ground ) ||
                        ( terrain_edge == ground_left ) ||
                        ( terrain_edge == ground_right ) )
                    {
                        continue;
                    }

                    checked_edges.insert( terrain_edge );

                    Line movement_line = movement;
                    if( movement_line.intersects( terrain_edge->line( ) + VectorY( half( space( ).bound_height( ) ) ) ) )
                    {
                        next_ground = terrain_edge;

                        Coordinate intersection = movement_line.intersection( terrain_edge->line( ) + VectorY( half( space( ).bound_height( ) ) ) );

                        movement = Vector( center, intersection );

                        if( velocity.magnitude( ) )
                        {
                            movement_percentage = movement.magnitude( ) / velocity.magnitude( );
                        }
                        else
                        {
                            movement_percentage = 1.0;
                        }
                    }
                }
            }
        }

        // check if object is moving to connecting edge
        if( m_ground && next_ground == m_ground )
        {
            if( ( movement.dx( ) > 0.0 ) && ( center + movement ).x( ) > m_ground->line( ).upper_bound_x( ) )
            {
                movement = Vector( center, m_ground->vertex2( )->position( ) + VectorY( half( space( ).bound_height( ) ) ) );

                if( m_ground->vertex2( ) )
                {
                    next_ground = m_ground->vertex2( )->edge2( );
                }
                else
                {
                    next_ground = nullptr;
                }
            }
            else if( ( movement.dx( ) < 0.0 ) && ( center + movement ).x( ) < m_ground->line( ).lower_bound_x( ) )
            {
                movement = Vector( center, m_ground->vertex1( )->position( ) + VectorY( half( space( ).bound_height( ) ) ) );

                if( m_ground->vertex1( ) )
                {
                    next_ground = m_ground->vertex1( )->edge1( );
                }
                else
                {
                    next_ground = nullptr;
                }
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

            varray<Object *> objects = room( )->objects_in_range( hit_box( ).union_with( hit_box( ) + movement ) );

            Line movement_line( movement );
            for_each( object, objects )
            {
                if( ( this == object ) || all_collided_objects.contains( object ) )
                {
                    continue;
                }

                FixedRectangle hit_box = object->hit_box( );
                hit_box.expand_width( width( ) );
                hit_box.expand_height( height( ) );

                varray<Line> intersections;
                if( hit_box.area( ) )
                {
                    intersections = hit_box.intersection( movement );
                }
                else
                {
                    if( hit_box.width( ) )
                    {
                        Line hit_box_line( hit_box.center( ) - VectorX( half( hit_box.width( ) ) ),
                                           hit_box.center( ) + VectorX( half( hit_box.width( ) ) ) );

                        if( movement_line.intersects( hit_box_line ) )
                        {
                            intersections = { Line( movement.origin( ), movement_line.intersection( hit_box_line ) ) };
                        }
                    }
                    else if( hit_box.height( ) )
                    {
                        Line hit_box_line( hit_box.center( ) - VectorY( half( hit_box.height( ) ) ),
                                           hit_box.center( ) + VectorY( half( hit_box.height( ) ) ) );

                        if( movement_line.intersects( hit_box_line ) )
                        {
                            intersections = { Line( movement.origin( ), movement_line.intersection( hit_box_line ) ) };
                        }
                    }
                    else
                    {
                        if( movement_line.on( hit_box.center( ) ) && movement_line.in_bounds( hit_box.center( ) ) )
                        {
                            intersections = { Line( movement.origin( ), hit_box.center( ) ) };
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
        ground( next_ground );

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
    if( interactive( ) )
    {
        room( )->object_grid( ).remove( this );
    }

    Visible::center( _position );
    Matter::position( _position );

    if( interactive( ) )
    {
        room( )->object_grid( ).add( this );
    }
}

Planc Object::width( ) const { return space( ).bound_width( ); }

Planc Object::height( ) const { return space( ).bound_height( ); }

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

dec Object::friction_resistance( ) const
{
    if( m_ground )
    {
        return m_ground->resistance( );
    }

    return m_air_resistance_ratio;
}

TerrainEdge * Object::ground( ) const { return m_ground; }
void Object::ground( TerrainEdge * ground ) { m_ground = ground; }

void Object::out_of_bounds( ) { mark_to_delete( ); }

bool Object::collide( Object * object ) { return false; }

void Object::react_to_movement( Object * object, Vector cref _v ) { Assert( m_movement_subscriptions.contains( object ), "not subscribed to object's movement" ); }

bool Object::terrain_boundaries( ) const { return m_terrain_boundaries; }
void Object::terrain_boundaries( cbool _terrain_boundaries )
{
    m_terrain_boundaries = _terrain_boundaries;
    if( !m_terrain_boundaries )
    {
        m_ground = nullptr;
    }
}

dec Object::gravity_ratio( ) const { return m_gravity_ratio; }
void Object::gravity_ratio( cdec _gravity_ratio ) { m_gravity_ratio = _gravity_ratio; }


dec Object::air_resistance_ratio( ) const { return m_air_resistance_ratio; }
void Object::air_resistance_ratio( cdec _air_resistance_ratio ) { m_air_resistance_ratio = _air_resistance_ratio; }

FixedRectangle Object::hit_box( ) const
{
    Polygon space = Matter::space( );
    return FixedRectangle( space.upper_bound_x( ) - space.lower_bound_x( ), space.upper_bound_y( ) - space.lower_bound_y( ), position( ) );
}

void Object::track_position( uint count )
{
    m_last_position_count = count;
    m_last_positions.resize( count, position( ) );
}

Coordinate Object::last_position( uint past )
{
    return m_last_positions[ ( m_last_position_index + past ) % m_last_position_count ];
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
    cPlanc HIT_BOX_THICKNESS = 1.5;
    cPlanc DOT_RADIUS = HIT_BOX_THICKNESS;
    cPlanc VELOCITY_THICKNESS = HIT_BOX_THICKNESS;
    cPlanc VELOCITY_ARROW_LENGTH = 10.0;
    cPlanc VELOCITY_MAGNITUDE_MINIMUM = 1.0;
    cPlanc VELOCITY_SCALE = 3.0;
    const Color PHYSICS_COLOR = YELLOW;

    Drawing debug_overlay;

    // physics
    // (only for objects on plane)
    if( Settings::get( Settings::DEBUG_PHYSICS ) && ( z( ) == 1.0 ) )
    {
        // hit box
        debug_overlay.draw( PHYSICS_COLOR, hit_box( ) - position( ), HIT_BOX_THICKNESS, true );

        // center
        debug_overlay.draw( PHYSICS_COLOR, Polygon::circle( DOT_RADIUS ), FILLED );

        // velocity
        Vector velocity_graphic = velocity( ) * VELOCITY_SCALE;
        if( velocity_graphic.magnitude( ) >= VELOCITY_MAGNITUDE_MINIMUM )
        {
            debug_overlay.draw( PHYSICS_COLOR, velocity_graphic.origin( ORIGIN ), VELOCITY_ARROW_LENGTH, VELOCITY_THICKNESS, true );
        }
    }

    return debug_overlay;
}
#endif
