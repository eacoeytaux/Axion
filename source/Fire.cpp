#include "Fire.hpp"

#include "World.hpp"

using mtmercy::Fire;

namespace
{
const Span<uint> FLAME_WAIT = { 4, 4 };
const Angle FLAME_DEVIATION = RIGHT_ANGLE / 2.0;
const Planc FLAME_ORANGE_SPEED = 2.5;
const Planc FLAME_ORANGE_RADIUS = METER * 1.8;
const Planc FLAME_ORANGE_SHRINK_RATE = 1.5;
const Planc FLAME_YELLOW_SPEED = 2.0;
const Planc FLAME_YELLOW_RADIUS = FLAME_ORANGE_RADIUS / 3.33 * 2.0;
const Planc FLAME_YELLOW_SHRINK_RATE = FLAME_ORANGE_SHRINK_RATE;
const Planc FLAME_YELLOW_OUTLINE_RATIO = 1.5;
const Planc FLAME_WIND_RESISTANCE = 2.0;
const Planc FLAME_RADIUS_MIN = 0.1;

const Planc LIGHT_DISTANCE = FLAME_ORANGE_RADIUS * 2.0;
const Color FLAME_COLOR_ORANGE = Color::rgb( 0xFF8000 );
const Color FLAME_COLOR_YELLOW = Color::rgb( 0xFFC000 );
const Planc TINT_DISTANCE = LIGHT_DISTANCE * 0.8;
const double TINT_FLICKER = 2.0;
const double TINT_INTENSITY = 0.125;
} // namespace

Fire::Fire( World * world, const Coordinate & _position ) : Object( world, _position )
{
    drawing_always_dirty( true );

    gravity_ratio( 0.0 );
    space( Shape::rectangle( FLAME_ORANGE_RADIUS.half( ), FLAME_ORANGE_RADIUS.half( ) ) );

    flame_timer.reset( FLAME_WAIT.min( ) );
}

const Fire & Fire::render( ) const
{
    Object::render( );

    for_each( flame, m_flames_orange )
        draw( FLAME_COLOR_ORANGE, Shape::equilateral( 4, flame.radius.half( ), flame.offset ) );

    for_each( flame, m_flames_yellow )
        draw( FLAME_COLOR_ORANGE, Shape::equilateral( 4, flame.radius.half( ) * FLAME_YELLOW_OUTLINE_RATIO, flame.offset ) );

    for_each( flame, m_flames_yellow )
        draw( FLAME_COLOR_YELLOW, Shape::equilateral( 4, flame.radius.half( ), flame.offset ) );

    return *this;
}

Fire & Fire::update( )
{
    Object::update( );

    clear_light_sources( );
    add_light_source( position( ), LIGHT_DISTANCE );
    add_light_source( position( ), TINT_DISTANCE, Color( FLAME_COLOR_ORANGE, TINT_INTENSITY ), TINT_FLICKER );
    add_light_source( position( ), TINT_DISTANCE * 0.8, Color( FLAME_COLOR_ORANGE, TINT_INTENSITY ), TINT_FLICKER );

    if( flame_timer.tick( ) )
    {
        flame_timer.reset( Random::rint( FLAME_WAIT ) );
        m_flames_orange.insert_back( ).radius = FLAME_ORANGE_RADIUS;
        m_flames_yellow.insert_back( ).radius = FLAME_YELLOW_RADIUS;
    }

    for_each( flame, m_flames_orange )
    {
        flame.radius -= min( FLAME_ORANGE_SHRINK_RATE, flame.radius );
        flame.offset += VectorA( Angle( RIGHT_ANGLE_1 + Random::rAngle( -FLAME_DEVIATION, FLAME_DEVIATION ) ), FLAME_YELLOW_SPEED ) + ( world( )->wind( ) / FLAME_WIND_RESISTANCE );
    }
    m_flames_orange.remove_if( []( Flame flame )
                               { return flame.radius < FLAME_RADIUS_MIN; } );

    for_each( flame, m_flames_yellow )
    {
        flame.radius -= min( FLAME_YELLOW_SHRINK_RATE, flame.radius );
        flame.offset += VectorA( Angle( RIGHT_ANGLE_1 + Random::rAngle( -FLAME_DEVIATION, FLAME_DEVIATION ) ), FLAME_ORANGE_SPEED ) + ( world( )->wind( ) / FLAME_WIND_RESISTANCE );
    }
    m_flames_yellow.remove_if( []( Flame flame )
                               { return flame.radius < FLAME_RADIUS_MIN; } );

    return *this;
}
