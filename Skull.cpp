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
const dec EYE_OFFSET_X = 0.4;
const dec EYE_OFFSET_Y = 0.1;
const dec EYE_SPACING = 0.5;

const Color BONE_COLOR = WHITE;
const Color HOLE_COLOR = BLACK;
} // namespace

Skull::Skull( World * world, const Coordinate & _position ) : Object( world, _position )
{
    needs_render_always( true );

    m_skull_drawing.draw( BONE_COLOR, Polygon::rectangle( JAW_WIDTH, JAW_HEIGHT, Coordinate( SKULL_RADIUS - half( JAW_WIDTH ), -half( JAW_HEIGHT ) ) ) );
    m_skull_drawing.draw( BONE_COLOR, Polygon::circle( SKULL_RADIUS ) );

    m_skull_drawing.draw( HOLE_COLOR, Polygon::circle( EYE_RADIUS, Coordinate( ( SKULL_RADIUS * EYE_OFFSET_X ) + ( SKULL_RADIUS * half( EYE_SPACING ) ), -( SKULL_RADIUS * EYE_OFFSET_Y ) ) ) );
    m_skull_drawing.draw( HOLE_COLOR, Polygon::circle( EYE_RADIUS, Coordinate( ( SKULL_RADIUS * EYE_OFFSET_X ) - ( SKULL_RADIUS * half( EYE_SPACING ) ), -( SKULL_RADIUS * EYE_OFFSET_Y ) ) ) );

    m_skull_drawing.rotate( Random::rAngle( ) );
}

void Skull::render( )
{
    Object::render( );

    draw( m_skull_drawing );
}

void Skull::update( )
{
    Object::update( );

    if( ground( ) )
    {
        Planc distance = velocity( ).magnitude( );
        dec ratio = Random::negate( distance / SKULL_CIRCUMFERENCE, is_neg( velocity( ).dx( ) ) );
        m_skull_drawing.rotate( Angle( TAU * ratio ) );
    }
}
