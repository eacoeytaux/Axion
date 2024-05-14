#include "Skull.hpp"

#include "World.hpp"

using mtmercy::Skull;

namespace
{
const Planc SKULL_RADIUS = METER / 3.0;
const Planc SKULL_DIAMETER = SKULL_RADIUS * 2.0;
const Planc SKULL_CIRCUMFERENCE = SKULL_RADIUS * SKULL_RADIUS * PI;

const Planc JAW_TO_SKULL_WIDTH_RADIO = 0.64;
const Planc JAW_TO_SKULL_HEIGHT_RADIO = 0.64;
const Planc JAW_WIDTH = SKULL_DIAMETER * JAW_TO_SKULL_WIDTH_RADIO;
const Planc JAW_HEIGHT = SKULL_DIAMETER * JAW_TO_SKULL_HEIGHT_RADIO;

const Planc EYE_RADIUS = 2.0;
const double EYE_OFFSET_X = 0.4;
const double EYE_OFFSET_Y = 0.1;
const double EYE_SPACING = 0.5;

const Color BONE_COLOR = WHITE;
const Color HOLE_COLOR = BLACK;
} // namespace

Skull::Skull( World * world, const Coordinate & _position ) : Object( world, _position )
{
    drawing_always_dirty( true );

    gravity_ratio( 1.0 );

    m_skull_drawing.draw( BONE_COLOR, Polygon::rectangle( JAW_WIDTH, JAW_HEIGHT, Coordinate( SKULL_RADIUS - half( JAW_WIDTH ), -half( JAW_HEIGHT ) ) ) );
    m_skull_drawing.draw( BONE_COLOR, Polygon::circle( SKULL_RADIUS ) );

    m_skull_drawing.draw( HOLE_COLOR, Polygon::circle( EYE_RADIUS, Coordinate( ( SKULL_RADIUS * EYE_OFFSET_X ) + ( SKULL_RADIUS * half( EYE_SPACING ) ), -( SKULL_RADIUS * EYE_OFFSET_Y ) ) ) );
    m_skull_drawing.draw( HOLE_COLOR, Polygon::circle( EYE_RADIUS, Coordinate( ( SKULL_RADIUS * EYE_OFFSET_X ) - ( SKULL_RADIUS * half( EYE_SPACING ) ), -( SKULL_RADIUS * EYE_OFFSET_Y ) ) ) );

    m_skull_drawing.rotate( Random::rAngle( ) );
}

const Skull & Skull::render( ) const
{
    Object::render( );

    draw( m_skull_drawing );

    return *this;
}

Skull & Skull::update( )
{
    Object::update( );

    if( ground( ) )
    {
        Planc distance = velocity( ).magnitude( );
        double ratio = Random::flipped( distance / SKULL_CIRCUMFERENCE, ( velocity( ).dx( ) < 0.0 ) );
        m_skull_drawing.rotate( Angle( TAU * ratio ) );
        m_skull_drawing.rotate( Angle( TAU * ratio ) );
    }

    return *this;
}
