#include "Waterfall.hpp"
#include "World.hpp"

AXN_NAMESPACES
using axn::reality::Waterfall;

const planc FOAM_X_SPEED = 5.f;
const planc FOAM_Y_SPEED = 10.f;
const planc FOAM_RADIUS_START_MIN = 5.f;
const planc FOAM_RADIUS_START_MAX = 20.f;
const planc FOAM_RADIUS_MIN = 2.f;
const planc FOAM_RADIUS_SHRINK = 0.5f;
const float FOAM_GRAVITY_RATIO = 0.5f;
const uint FOAM_EDGE_COUNT = 16;
const uint FOAM_NEW_COUNT_MIN = 0;
const uint FOAM_NEW_COUNT_MAX = 4;
// const float DIVIDER_DELTA = 0.01f;
// const float MIN_DIVIDER_PERCENTAGE = 0.1f;

Waterfall::Waterfall( World* _world, const Coordinate & _bottom, const planc _width,
                      const planc _height, const ufloat _z )
    : StationaryObject( _world, _bottom ) {
    drawing_always_dirty( true );

    z( _z );

    space( Shape::rectangle( _width, _height ) );
}

const Waterfall & Waterfall::render( ) const {
    const Color foam_color = WHITE;

    const Color bottom_color = Color( foam_color ).a( 0.75f );
    const Color top_color = Color( bottom_color ).a( 0.f );

    Object::render( );

    draw( CYAN, Shape::rectangle( width( ), height( ), Coordinate( 0, height( ).half( ) ) ) );

    draw( { top_color, top_color, bottom_color, bottom_color },
         Shape::rectangle( width( ), height( ), Coordinate( 0, height( ).half( ) ) ) );

    for_each( foam, m_foam ) {
        draw( WHITE, Shape::equilateral( FOAM_EDGE_COUNT, foam.radius, foam.position ) );
    }

    return *this;
}

Waterfall & Waterfall::update( ) {
    uint new_foam_count = Random::rng_int( FOAM_NEW_COUNT_MIN, FOAM_NEW_COUNT_MAX );
    while( new_foam_count-- ) {
        Foam foam;
        foam.position = ORIGIN + Vector( Random::rng_float( width( ).half( ).half( ) ) *
                                             ( Random::rng_bool( ) ? 1.f : -1.f ),
                                         0.f );
        foam.radius = Random::rng_float( FOAM_RADIUS_START_MIN, FOAM_RADIUS_START_MAX );
        foam.movement =
            Vector( Random::rng_float( FOAM_X_SPEED, -FOAM_X_SPEED ), FOAM_Y_SPEED );
        m_foam.push_back( foam );
    }

    for_each( foam, m_foam ) {
        foam.movement += GRAVITY * FOAM_GRAVITY_RATIO;
        foam.position += foam.movement;
        foam.radius -= FOAM_RADIUS_SHRINK;
    }
    m_foam.remove_if( []( Foam foam ) {
        return ( ( foam.radius <= FOAM_RADIUS_MIN ) ||
                 ( foam.position.y( ) < -foam.movement.dy( ) ) );
    } );

    return *this;
}
