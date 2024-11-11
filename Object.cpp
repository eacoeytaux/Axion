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

    for_each( object, m_movement_subscribers )
    {
        object->unsubscribe_to_movement( this );
    }

    for_each( object, m_movement_subscriptions )
    {
        object->remove_movement_subscriber( this );
    }
}

Object::Object( World * world ) : Matter( ORIGIN ), m_world( world )
{
    init( );
}

Object::Object( World * world, const Coordinate & _position, const Vector & _velocity ) : Matter( Vector( _velocity ).origin( _position ) ), m_world( world )
{
    init( );
}

Object::Object( World * world, const Vector & _position_velocity ) : Matter( _position_velocity ), m_world( world )
{
    init( );
}

void Object::init( )
{
    assert( !m_initialized );
    m_initialized = true;
    
#ifdef AXN_DEBUG
    ++total_objects;
#endif

    m_age = 0;
    
    foreground( z( ) > ONE );
    background( z( ) < ONE );
}

Drawing Object::path( const Planc & _distance, const Color & _color, const dec _alpha_start, const dec _alpha_end ) const
{
    Drawing path;
    
    if ( !Object::ground( ) )
    {
        Coordinate c;
        Vector v = velocity( );
        Planc d = _distance;
        dec a = _alpha_start;
        
        while( d > ZERO && v.has_magnitude( ) )
        {
            v += GRAVITY * gravity_ratio( );
            v *= ONE - friction_resistance( );
            
            if( d > v.magnitude( ) )
            {
                d -= v.magnitude( );
            }
            else
            {
                v.magnitude( d );
                d = ZERO;
            }
            
            dec d_a = min<dec>( a, ( _alpha_start - _alpha_end ) * ( v.magnitude( ) / _distance ) );
            
            path.draw( Color( _color, a ), Color( _color, a - d_a ), Line( c, c + v ) );
            a -= d_a;
            
            c += v;
        }
    }
    
    return path;
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
    update_movement( );
}

void Object::update_object( )
{
    if( m_last_world_age_update == world( )->age( ) )
    {
        return;
    }

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

    update( );

    if( marked_to_delete( ) )
    {
        mark_deleted( );
    }

    m_last_world_age_update = world( )->age( );
    ++m_age;
}

void Object::update_velocity( )
{
    if( stationary( ) )
    {
        Matter::velocity( ZERO_VECTOR );
        return;
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

    velocity *= ( ONE - friction_resistance( ) );

    Matter::velocity( velocity );
}

void Object::update_movement( )
{
    // if( stationary( ) )
    // {
    //    return;
    // }

    uset<TerrainEdge *> checked_edges;
    if( m_ground )
    {
        checked_edges.insert( m_ground );
    }

    dec remaining_percentage = ONE;
    while( ( dec_gt( remaining_percentage, ZERO ) && !isnan( remaining_percentage ) ) )
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
            for_each( terrain_edge, m_world->terrain( )->edges( hit_box( ) ) )
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
                if( movement_line.intersects( terrain_edge->line( ) + VectorY( space( ).bound_height( ).half( ) ) ) )
                {
                    next_ground = terrain_edge;

                    Coordinate intersection = movement_line.intersection( terrain_edge->line( ) + VectorY( space( ).bound_height( ).half( ) ) );

                    movement = Vector( center, intersection );
                    
                    if( velocity.magnitude( ) )
                    {
                        movement_percentage = movement.magnitude( ) / velocity.magnitude( );
                    }
                    else
                    {
                        movement_percentage = ONE;
                    }
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

        if( interactive( ) )
        {
            struct ObjectCollision
            {
                Object * object;
                Line line;
            };

            list<ObjectCollision> collied_objects;

            // todo union with moved hit box
            // list<Object *> objects = world( )->objects_in_range( hit_box( ) + movement );
            varray<Object *> objects = world( )->objects_in_range( hit_box( ) );

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
                if( hit_box.area( ) )
                {
                    intersections = hit_box.intersection( movement );
                }
                else
                {
                    if( hit_box.width( ) )
                    {
                        Line hit_box_line( hit_box.center( ) - VectorX( hit_box.width( ).half( ) ),
                                           hit_box.center( ) + VectorX( hit_box.width( ).half( ) ) );

                        if( movement_line.intersects( hit_box_line ) )
                        {
                            intersections = { Line( movement.origin( ), movement_line.intersection( hit_box_line ) ) };
                        }
                    }
                    else if( hit_box.height( ) )
                    {
                        Line hit_box_line( hit_box.center( ) - VectorY( hit_box.height( ).half( ) ),
                                           hit_box.center( ) + VectorY( hit_box.height( ).half( ) ) );

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
                        return ( c1.line.c1( ).distance_to( origin ) < c2.line.c1( ).distance_to( origin ) );
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

        if( velocity.magnitude( ) )
        {
            remaining_percentage *= ( ONE - ( movement.magnitude( ) / velocity.magnitude( ) ) );
        }
        else
        {
            remaining_percentage = ZERO;
        }
    }
}

void Object::move( const Vector & _movement )
{
    if( _movement.has_magnitude( ) )
    {
        position( position( ) + _movement );
        
        if( z( ) )
        {
            FixedRectangle world_bounds = world( )->bounds( );
            
            // todo seems messy?
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

Coordinate Object::position( ) const
{
    return Matter::position( );
}

void Object::position( const Coordinate & _position )
{
    if( interactive( ) )
    {
        world( )->object_grid( ).erase( this );
    }
    
    Visible::center( _position );
    Matter::position( _position );
    
    if( interactive( ) )
    {
        world( )->object_grid( ).add( this );
    }
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

void Object::visible_width( const Planc & _visible_width )
{
    m_visible_width = _visible_width;
}

Planc Object::visible_height( ) const
{
    return m_visible_height;
}

void Object::visible_height( const Planc & _visible_height )
{
    m_visible_height = _visible_height;
}

bool Object::foreground( ) const
{
    return m_foreground;
}

void Object::foreground( const bool _foreground )
{
    m_foreground = _foreground || ( z( ) > ONE );
}

bool Object::background( ) const
{
    return m_background;
}

void Object::background( const bool _background )
{
    m_background = _background || ( z( ) < ONE );
}

bool Object::interactive( ) const
{
    return m_interactive;
}

void Object::interactive( const bool _interactive )
{
    m_interactive = _interactive;
    
    if( !m_interactive )
    {
        world( )->object_grid( ).erase( this );
    }
}

bool Object::stationary( ) const
{
    return m_stationary;
}

void Object::stationary( const bool _stationary )
{
    m_stationary = _stationary;
    
    if( m_stationary )
    {
        velocity( ZERO_VECTOR );
    }
}

dec Object::friction_resistance( ) const
{
    if( m_ground )
    {
        return m_ground->resistance( );
    }

    return ZERO; // TODO AIR_RESISTANCE;
}

TerrainEdge * Object::ground( ) const
{
    return m_ground;
}

void Object::ground( TerrainEdge * ground )
{
    m_ground = ground;
}

void Object::out_of_bounds( )
{
    mark_to_delete( );
}

bool Object::collide( Object * object )
{
    return false;
}

void Object::react_to_movement( Object * object, const Vector & _v )
{
    Assert( m_movement_subscriptions.contains( object ), "not subscribed to object's movement" );
}

bool Object::terrain_boundaries( ) const { return m_terrain_boundaries; }

void Object::terrain_boundaries( const bool _terrain_boundaries )
{
    m_terrain_boundaries = _terrain_boundaries;
    if( !m_terrain_boundaries )
    {
        m_ground = nullptr;
    }
}

dec Object::gravity_ratio( ) const { return m_gravity_ratio; }

void Object::gravity_ratio( const dec _gravity_ratio )
{
    m_gravity_ratio = _gravity_ratio;
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
    m_movement_subscriptions.erase( object );
    object->remove_movement_subscriber( this );
}

void Object::add_movement_subscriber( Object * object )
{
    m_movement_subscribers.insert( object );
}

void Object::remove_movement_subscriber( Object * object )
{
    m_movement_subscribers.erase( object );
}

#ifdef AXN_DEBUG
Drawing Object::debug_overlay( ) const
{
    const Planc HIT_BOX_THICKNESS = 1.5;
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
        debug_overlay.draw( COLOR, velocity_graphic.origin( ORIGIN ), VELOCITY_ARROW_LENGTH, VELOCITY_THICKNESS, true );
    }

    return debug_overlay;
}
#endif
