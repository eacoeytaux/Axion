#include "Object.hpp"

#include "World.hpp"
#include "Terrain.hpp"

AXN_NAMESPACES
using axn::reality::Object;

#ifdef AXN_DEBUG
uint Object::total_objects = 0;
#endif

Object::~Object( ) {
#ifdef AXN_DEBUG
    --total_objects;
#endif
}

Object::Object( World* world, const Coordinate & _position )
    : Matter( _position ), m_world( world ) {
#ifdef AXN_DEBUG
    ++total_objects;
#endif

    // defaults
    m_age = 1;
    m_terrarin_boundaries = true;
}

Object & Object::update( ) {
    move( );
    return *this;
}

Object & Object::pre_update( ) {
    m_last_position = position( );
    return *this;
}

Object & Object::post_update( ) {
    m_last_world_age = m_world->age( );
    ++m_age;
    return *this;
}

Object & Object::adjust_velocity( ) {
    Vector velocity = Matter::velocity( );

    if( m_ground && m_terrarin_boundaries ) {
        Angle ground_angle = m_ground->line( ).angle( );
        velocity.rotate( -ground_angle );
        velocity.dy( 0 );
        velocity.rotate( ground_angle );
    } else if( !m_ground && m_gravity_ratio ) {
        velocity += ( GRAVITY * m_gravity_ratio );
    }

    velocity *= 1 - friction_resistance( );

    Matter::velocity( velocity );
    return *this;
}

Object & Object::move( ) {
    float remaining_percentage = 1.f;
    while( remaining_percentage ) {
        adjust_velocity( );

        Coordinate center = position( );
        Vector velocity = Object::velocity( ) * remaining_percentage;
        Vector movement = Vector( center, center + velocity );
        float movement_percentage = 1.f;

        TerrainEdge* next_ground = m_ground;

        // if objects floats through walls don't bother
        if( m_terrarin_boundaries || !m_world->terrain( ) ) {
            TerrainEdge* ground_left = nullptr;
            TerrainEdge* ground_right = nullptr;

            if( m_ground ) {
                // check if currently sitting on vertex if so pick edge in
                // direction of movement for ground
                if( ( movement.dx( ) > 0.f ) &&
                    ( center.x( ) == m_ground->vertex2( )->position( ).x( ) ) ) {
                    m_ground = m_ground->vertex2( )->edge2( );
                } else if( ( movement.dx( ) > 0.f ) &&
                           ( center.x( ) == m_ground->vertex1( )->position( ).x( ) ) ) {
                    m_ground = m_ground->vertex1( )->edge1( );
                }

                if( m_ground ) {
                    ground_left =
                        ( m_ground->vertex1( ) ? m_ground->vertex1( )->edge1( ) : nullptr );
                    ground_right =
                        ( m_ground->vertex2( ) ? m_ground->vertex2( )->edge2( ) : nullptr );
                }
            }

            // check if object is colliding with any edges
            for_each( terrain_edge, m_world->terrain( )->edges( ) ) {
                // skip if terrain_edge is current ground or adjacent as these
                // are already dealt with
                if( ( terrain_edge == m_ground ) || ( terrain_edge == ground_left ) ||
                    ( terrain_edge == ground_right ) )
                    continue;

                Line movement_line = Line( center, center + velocity );
                if( movement_line.intersects(
                        terrain_edge->line( ) +
                        Vector( (planc)0.f, space( ).bound_height( ).half( ) ) ) ) {
                    next_ground = terrain_edge;
                    Coordinate intersection = movement_line.intersection(
                        terrain_edge->line( ) +
                        Vector( (planc)0.f, space( ).bound_height( ).half( ) ) );
                    movement = Vector( center, intersection );
                    movement_percentage = movement.magnitude( ) / velocity.magnitude( );
                }
            }
        }

        // check if object is moving to connecting edge
        if( m_ground && next_ground == m_ground ) {
            if( ( movement.dx( ) > 0.f ) &&
                ( center + movement ).x( ) > m_ground->line( ).right( ).x( ) ) {
                movement =
                    Vector( center, m_ground->vertex2( )->position( ) +
                                        Vector( (planc)0.f, space( ).bound_height( ).half( ) ) );
                movement_percentage = movement.magnitude( ) / velocity.magnitude( );
                if( m_ground->vertex2( ) ) {
                    next_ground = m_ground->vertex2( )->edge2( );
                } else {
                    next_ground = nullptr;
                }
            } else if( ( movement.dx( ) < 0.f ) &&
                       ( center + movement ).x( ) < m_ground->line( ).left( ).x( ) ) {
                movement =
                    Vector( center, m_ground->vertex1( )->position( ) +
                                        Vector( (planc)0.f, space( ).bound_height( ).half( ) ) );
                movement_percentage = movement.magnitude( ) / velocity.magnitude( );
                if( m_ground->vertex1( ) ) {
                    next_ground = m_ground->vertex1( )->edge1( );
                } else {
                    next_ground = nullptr;
                }
            }
        } else {
            int x = 666;
            x++;
        }

        m_ground = next_ground;
        position( position( ) + movement );
        remaining_percentage *= ( 1 - movement_percentage );
    }

    // if (float_gt(movement_percentage, 0)) {
    //     move(dt * movement_percentage);
    // }

    return *this;
}

ufloat Object::friction_resistance( ) const {
    if( m_terrarin_boundaries && m_ground )
        return m_ground->resistance( );
    return AIR_RESISTANCE;
}

planc Object::width( ) const { return space( ).bound_width( ); }

planc Object::height( ) const { return space( ).bound_height( ); }

planc Object::visible_width( ) const { return m_visible_width; }

Object & Object::visible_width( const planc & _visible_width ) {
    m_visible_width = _visible_width;
    return *this;
}

planc Object::visible_height( ) const { return m_visible_height; }

Object & Object::visible_height( const planc & _visible_height ) {
    m_visible_height = _visible_height;
    return *this;
}

bool Object::terrarin_boundaries( ) const { return m_terrarin_boundaries; }

Object & Object::terrarin_boundaries( const bool _terrarin_boundaries ) {
    m_terrarin_boundaries = _terrarin_boundaries;
    if( !m_terrarin_boundaries )
        m_ground = nullptr;
    return *this;
}

ufloat Object::gravity_ratio( ) const { return m_gravity_ratio; }

Object & Object::gravity_ratio( const ufloat _gravity_ratio ) {
    m_gravity_ratio = _gravity_ratio;
    return *this;
}

Shape Object::hit_box( ) const {
    Shape space = Matter::space( ) - position( );
    Shape hit_box = Shape::rectangle(
        space.upper_bound_x( ) - space.lower_bound_x( ),
        space.upper_bound_y( ) - space.lower_bound_y( ),
        position( ) );
    return hit_box;
}

Shape Object::visible_box( ) const {
    return Shape::rectangle( visible_width( ), visible_height( ), position( ) );
}

#ifdef AXN_DEBUG
Drawing Object::debug_overlay( ) const {
    const ufloat VELOCITY_SCALE = 3.f;
    const ufloat HIT_BOX_THICKNESS = 1.f;
    const Color HIT_BOX_COLOR = YELLOW;

    Drawing debug_overlay;

    // hit box
    debug_overlay.add( HIT_BOX_COLOR, hit_box( ), HIT_BOX_THICKNESS );

    // center / velocity
    debug_overlay.add( HIT_BOX_COLOR, Shape::circle( HIT_BOX_THICKNESS * 2, position( ) ), FILLED );
    Vector velocity_graphic = velocity( ).origin( position ( ) ) * VELOCITY_SCALE;
    if( velocity_graphic.magnitude( ) > 1.f )
        debug_overlay.add( HIT_BOX_COLOR, velocity_graphic, 10, HIT_BOX_THICKNESS );

    return debug_overlay;
}
#endif
