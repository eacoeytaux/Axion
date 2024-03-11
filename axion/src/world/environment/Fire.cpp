#include "Fire.hpp"

#include "World.hpp"

AXN_NAMESPACES
using axn::reality::Fire;

const int FLAME_WAIT = 4;
const Angle FLAME_DEVIATION = Angle( PI_1D4 );
const planc FLAME_ORANGE_SPEED = 2.5f;
const planc FLAME_ORANGE_RADIUS = METER * 1.8f;
const planc FLAME_ORANGE_SHRINK_RATE = 1.5f;
const planc FLAME_YELLOW_SPEED = 2.f;
const planc FLAME_YELLOW_RADIUS = FLAME_ORANGE_RADIUS / 3.33f * 2.f;
const planc FLAME_YELLOW_SHRINK_RATE = FLAME_ORANGE_SHRINK_RATE;
const planc FLAME_WIND_RESISTANCE = 2.f;
const Angle SMOKE_DEVIATION = Angle( PI_1D4 );
// const int SMOKE_SPEED = 4;
// const ufloat SMOKE_GROW_RATE = 1;
// const ufloat SMOKE_WIND_RESISTANCE = 1;
// const int SMOKE_RADIUS_START = 8;
// const int SMOKE_ALPHA_MAX = 80;
// const int SMOKE_ALPHA_GROW_RATE = 5;
// const int SMOKE_ALPHA_SHRINK_RATE = 2;
const planc LIGHT_DISTANCE = FLAME_ORANGE_RADIUS * 3.f;
const Color FLAME_COLOR_ORANGE = Color::rgb( 0xFF8000 );
const Color FLAME_COLOR_YELLOW = Color::rgb( 0xFFC000 );
const planc TINT_DISTANCE = LIGHT_DISTANCE * 0.8f;
const float TINT_INTENSITY = 0.125f;

Fire::Flame::Flame( const planc & _radius, const Coordinate & _offset ) {
    offset = _offset;
    radius = _radius;
}

Fire::Fire( World* world, const Coordinate & _position ) : Object( world, _position ) {
    drawing_always_dirty( true );
    gravity_ratio( 0.f );
    space( Shape::rectangle( FLAME_ORANGE_RADIUS.half( ), FLAME_ORANGE_RADIUS.half( ) ) );

    flame_timer.reset( FLAME_WAIT );
}

const Fire & Fire::render( ) const {
    Object::render( );

    for_each( flame, m_flames_orange ) {
        draw( FLAME_COLOR_ORANGE, Shape::equilateral( 4, flame.radius.half( ), flame.offset ) );
    }

    for_each( flame, m_flames_yellow ) {
        draw( FLAME_COLOR_ORANGE, Shape::equilateral( 4, flame.radius.half( ) * ( 4.f / 3.f ), flame.offset ) );
    }

    for_each( flame, m_flames_yellow ) {
        draw( FLAME_COLOR_YELLOW, Shape::equilateral( 4, flame.radius.half( ), flame.offset ) );
    }

    return *this;
}

Fire & Fire::update( ) {
    Object::update( );

    clear_light_sources( );
    add_light_source( position( ), LIGHT_DISTANCE );
    add_light_source( position( ), TINT_DISTANCE,
                      Color( FLAME_COLOR_ORANGE ).a( TINT_INTENSITY ), 2 );
    add_light_source( position( ), TINT_DISTANCE * 0.8f,
                      Color( FLAME_COLOR_ORANGE ).a( TINT_INTENSITY ), 2 );

    if( flame_timer.tick( ) ) {
        flame_timer.reset( FLAME_WAIT );
        m_flames_orange.push_back( Flame( FLAME_ORANGE_RADIUS ) );
        m_flames_yellow.push_back( Flame( FLAME_YELLOW_RADIUS ) );
    }

    for_each( flame, m_flames_orange ) {
        flame.radius -= min<float>( FLAME_ORANGE_SHRINK_RATE, flame.radius );
        flame.offset +=
            VectorA( Angle( PI_1D2 + Random::rng_float( -FLAME_DEVIATION.radians( ),
                                                        FLAME_DEVIATION.radians( ) ) ),
                     FLAME_YELLOW_SPEED ) +
            ( world( )->wind( ) / FLAME_WIND_RESISTANCE );
    }
    m_flames_orange.remove_if( []( Flame flame ) { return !flame.radius; } );

    for_each( flame, m_flames_yellow ) {
        flame.radius -= min<float>( FLAME_YELLOW_SHRINK_RATE, flame.radius );
        flame.offset +=
            VectorA( Angle( PI_1D2 + Random::rng_float( -FLAME_DEVIATION.radians( ),
                                                        FLAME_DEVIATION.radians( ) ) ),
                     FLAME_ORANGE_SPEED ) +
            ( world( )->wind( ) / FLAME_WIND_RESISTANCE );
    }
    m_flames_yellow.remove_if( []( Flame flame ) { return !flame.radius; } );

    return *this;
}
