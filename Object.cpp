#include "Object.hpp"

#include "World.hpp"
#include "Terrain.hpp"

#ifdef AXN_DEBUG
uint Object::total_objects = 0;
#endif

Object::~Object( )
{
#ifdef AXN_DEBUG
    --total_objects;
#endif
}

Object::Object( World * world, const Coordinate & _position ) : Matter( _position ), m_world( world )
{
#ifdef AXN_DEBUG
    ++total_objects;
#endif
    // defaults
    m_age = 1;
    m_terrain_boundaries = true;

    position_drawing( position( ) );
}

const Object & Object::render( ) const
{
    Visible::position_drawing( position( ) );
    Visible::render( );
    return *this;
}

const Object & Object::render_object( ) const
{
    render( );
    return *this;
}

Object & Object::update( )
{
    if( marked_to_delete( ) )
    {
        mark_deleted( );
    }

    update_movement( );

    return *this;
}

Object & Object::update_object( )
{
    if( m_last_world_age_update == world( )->age( ) )
        return *this;

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

    // -------- //
    this->update( );
    // -------- //

    m_last_world_age_update = m_world->age( );
    ++m_age;

    return *this;
}

Object & Object::update_velocity( )
{
    if( stationary( ) )
    {
        Matter::velocity( ZERO_VECTOR );
        return *this;
    }

    Vector velocity = Matter::velocity( );

    if( m_ground && terrain_boundaries( ) )
    {
        if( velocity.has_magnitude( ) )
        {
            velocity.flatten( m_ground->line( ).angle( ).flip( ) );
        }
    }
    else if( !m_ground && m_gravity_ratio )
    {
        velocity += ( GRAVITY * m_gravity_ratio );
    }

    velocity *= ( 1.0 - friction_resistance( ) );

    Matter::velocity( velocity );
    return *this;
}

Object & Object::update_movement( )
{
    if( stationary( ) )
        return *this;

    uset<TerrainEdge *> checked_edges;
    if( m_ground )
        checked_edges.insert( m_ground );

    dec remaining_percentage = 1.0;
    while( ( dec_ge( remaining_percentage, 0.0 ) && !isnan( remaining_percentage ) ) && !stationary( ) )
    {
        update_velocity( );
        Vector velocity = Object::velocity( ) * remaining_percentage;
        if( !velocity.has_magnitude( ) )
            break;

        Coordinate center = position( );

        Vector movement = Vector( center, center + velocity );

        dec movement_percentage = 1.0;

        TerrainEdge * next_ground = m_ground;

        // if objects decs through walls don't bother
        if( terrain_boundaries( ) || !m_world->terrain( ) )
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
            for_each( terrain_edge, m_world->terrain( )->edges( ) )
            {
                // skip if terrain_edge is current ground or adjacent as these are already dealt with
                if( checked_edges.contains( terrain_edge ) ||
                    ( terrain_edge == m_ground ) ||
                    ( terrain_edge == ground_left ) ||
                    ( terrain_edge == ground_right ) )
                    continue;

                checked_edges.insert( terrain_edge );
                Line movement_line = movement;
                if( movement_line.intersects( terrain_edge->line( ) + VectorY( space( ).bound_height( ).half( ) ) ) )
                {
                    next_ground = terrain_edge;
                    Coordinate intersection = movement_line.intersection( terrain_edge->line( ) + VectorY( space( ).bound_height( ).half( ) ) );
                    movement = Vector( center, intersection );
                    movement_percentage = movement.magnitude( ) / velocity.magnitude( );
                }
            }
        }

        // check if object is moving to connecting edge
        if( m_ground && next_ground == m_ground )
        {
            if( ( movement.dx( ) > 0.0 ) && ( center + movement ).x( ) > m_ground->line( ).right( ).x( ) )
            {
                movement = Vector( center, m_ground->vertex2( )->position( ) + VectorY( space( ).bound_height( ).half( ) ) );
                if( m_ground->vertex2( ) )
                {
                    next_ground = m_ground->vertex2( )->edge2( );
                }
                else
                {
                    next_ground = nullptr;
                }
            }
            else if( ( movement.dx( ) < 0.0 ) && ( center + movement ).x( ) < m_ground->line( ).left( ).x( ) )
            {
                movement = Vector( center, m_ground->vertex1( )->position( ) + VectorY( space( ).bound_height( ).half( ) ) );
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

        if( solid( ) )
        {
            struct ObjectCollision
            {
                Object * object;
                Line line;
            };

            list<ObjectCollision> collied_objects;

            Planc start_x = position( ).x( ) - width( ).half( );
            Planc end_x = position( ).x( ) + width( ).half( ) + movement.dx( );
            list<Object *> objects = world( )->solid_objects_in_range( start_x, end_x );

            Line movement_line( movement );
            for_each( object, objects )
            {
                if( this == object )
                {
                    continue;
                }

                FixedRectangle hit_box = object->hit_box( );
                hit_box.expand_width( width( ) );
                hit_box.expand_height( height( ) );

                varray<Line> intersections;
                if( hit_box.width( ) && hit_box.height( ) )
                {
                    intersections = hit_box.intersection( movement );
                }
                else
                {
                    if( hit_box.width( ) )
                    {
                        Line hit_box_line( hit_box.center( ).x( ) - hit_box.width( ).half( ), hit_box.center( ).x( ) + hit_box.width( ).half( ) );
                        if( movement_line.intersects( hit_box_line ) )
                        {
                            intersections = { Line( movement.origin( ), movement_line.intersection( hit_box_line ) ) };
                        }
                    }
                    else if( hit_box.height( ) )
                    {
                        Line hit_box_line( hit_box.center( ).y( ) - hit_box.height( ).half( ), hit_box.center( ).y( ) + hit_box.height( ).half( ) );
                        if( movement_line.intersects( hit_box_line ) )
                        {
                            intersections = { Line( movement.origin( ), movement_line.intersection( hit_box_line ) ) };
                        }
                    }
                    else
                    {
                        if( movement_line.on( hit_box.center( ) ) )
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
                    collied_objects.insert_back( collision );
                }
            }

            if( collied_objects.size( ) )
            {
                struct
                {
                    Coordinate origin;
                    bool operator( )( const ObjectCollision & c1, const ObjectCollision & c2 )
                    {
                        return ( c1.line.c1( ).distance( origin ) < c2.line.c1( ).distance( origin ) );
                    }
                } collision_sort;
                collision_sort.origin = position( );
                collied_objects.sort( collision_sort );

                ObjectCollision & collision = collied_objects.front( );

                collide( collision.object );

                // movement = Vector( position( ), collision.line.c1( ) );
            }
        }

        move( movement );
        ground( next_ground );

        remaining_percentage *= ( 1.0 - ( movement.magnitude( ) / velocity.magnitude( ) ) );
    }

    return *this;
}

Object & Object::move( const Vector & _movement )
{
    if( _movement.has_magnitude( ) )
    {
        position( position( ) + _movement );

        FixedRectangle world_bounds = world( )->bounds( );

        world_bounds.width( world_bounds.width( ) / ( z( ) * z( ) ) );
        world_bounds.height( world_bounds.height( ) / ( z( ) * z( ) ) );

        if( !world_bounds.contains( position( ) ) )
        {
            out_of_bounds( );
        }

        for_each( object, m_movement_subscribers )
        {
            object->react_to_movement( this, _movement );
        }
    }

    return *this;
}

Planc Object::width( ) const
{
    return space( ).bound_width( );
}

Planc Object::height( ) const
{
    return space( ).bound_height( );
}

Planc Object::visible_width( ) const
{
    return m_visible_width;
}

Object & Object::visible_width( const Planc & _visible_width )
{
    m_visible_width = _visible_width;
    return *this;
}

Planc Object::visible_height( ) const
{
    return m_visible_height;
}

Object & Object::visible_height( const Planc & _visible_height )
{
    m_visible_height = _visible_height;
    return *this;
}

bool Object::stationary( ) const
{
    return m_stationary;
}

Object & Object::stationary( const bool _stationary )
{
    m_stationary = _stationary;
    if( m_stationary )
    {
        velocity( ZERO_VECTOR );
    }

    return *this;
}

dec Object::friction_resistance( ) const
{
    if( m_ground )
    {
        return m_ground->resistance( );
    }

    return 0; // TODO AIR_RESISTANCE;
}

TerrainEdge * Object::ground( ) const
{
    return m_ground;
}

Object & Object::ground( TerrainEdge * ground )
{
    m_ground = ground;
    return *this;
}

Object & Object::out_of_bounds( )
{
    mark_to_delete( );
    return *this;
}

bool Object::collide( Object * object )
{
    return false;
}

Object & Object::react_to_movement( Object * object, const Vector & _v )
{
    Assert( m_movement_subscriptions.contains( object ), "not subscribed to object's movement" );
    return *this;
}

bool Object::terrain_boundaries( ) const { return m_terrain_boundaries; }

Object & Object::terrain_boundaries( const bool _terrain_boundaries )
{
    m_terrain_boundaries = _terrain_boundaries;
    if( !m_terrain_boundaries )
    {
        m_ground = nullptr;
    }

    return *this;
}

dec Object::gravity_ratio( ) const { return m_gravity_ratio; }

Object & Object::gravity_ratio( const dec _gravity_ratio )
{
    m_gravity_ratio = _gravity_ratio;
    return *this;
}

FixedRectangle Object::hit_box( ) const
{
    Polygon space = Matter::space( );
    return FixedRectangle( space.upper_bound_x( ) - space.lower_bound_x( ), space.upper_bound_y( ) - space.lower_bound_y( ), position( ) );
}

FixedRectangle Object::visible_box( ) const
{
    return FixedRectangle( visible_width( ), visible_height( ), position( ) );
}

Object & Object::track_position( uint count )
{
    m_last_position_count = count;
    m_last_positions.resize( count, position( ) );
    return *this;
}

Coordinate Object::last_position( uint past )
{
    return m_last_positions[ ( m_last_position_index + past ) % m_last_position_count ];
}

Object & Object::subscribe_to_movement( Object * object )
{
    m_movement_subscriptions.insert( object );
    object->add_movement_subscriber( this );
    return *this;
}

Object & Object::add_movement_subscriber( Object * object )
{
    m_movement_subscribers.insert( object );
    return *this;
}

#ifdef AXN_DEBUG
Drawing Object::debug_overlay( ) const
{
    const Planc HIT_BOX_THICKNESS = 2.0;
    const Planc DOT_RADIUS = HIT_BOX_THICKNESS;
    const Planc VELOCITY_THICKNESS = HIT_BOX_THICKNESS;
    const Planc VELOCITY_ARROW_LENGTH = 10.0;
    const Planc VELOCITY_MAGNITUDE_MINIMUM = 1.0;
    const Planc VELOCITY_SCALE = 3.0;
    const Color COLOR = YELLOW;

    Drawing debug_overlay;

    // hit box
    debug_overlay.draw( COLOR, hit_box( ) - position( ), HIT_BOX_THICKNESS, true );

    // center
    debug_overlay.draw( COLOR, Circle( DOT_RADIUS ), FILLED );

    // velocity
    Vector velocity_graphic = velocity( ) * VELOCITY_SCALE;
    if( velocity_graphic.magnitude( ) >= VELOCITY_MAGNITUDE_MINIMUM )
    {
        debug_overlay.draw( COLOR, velocity_graphic, VELOCITY_ARROW_LENGTH, VELOCITY_THICKNESS, true );
    }

    return debug_overlay;
}
#endif
