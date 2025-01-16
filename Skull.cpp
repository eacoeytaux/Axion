#include "Skull.hpp"
#include "World.hpp"

using mtmercy::Skull;

namespace
{
cPlanc SKULL_RADIUS = METER / 3.0;
cPlanc SKULL_DIAMETER = SKULL_RADIUS * 2.0;
cPlanc SKULL_CIRCUMFERENCE = SKULL_RADIUS * SKULL_RADIUS * PI;

cPlanc JAW_TO_SKULL_WIDTH_RADIO = 0.64;
cPlanc JAW_TO_SKULL_HEIGHT_RADIO = 0.64;
cPlanc JAW_WIDTH = SKULL_DIAMETER * JAW_TO_SKULL_WIDTH_RADIO;
cPlanc JAW_HEIGHT = SKULL_DIAMETER * JAW_TO_SKULL_HEIGHT_RADIO;

cPlanc EYE_RADIUS = 2.0;
cdec EYE_OFFSET_X = 0.4;
cdec EYE_OFFSET_Y = 0.1;
cdec EYE_SPACING = 0.5;

const Color BONE_COLOR = WHITE;
const Color HOLE_COLOR = BLACK;
} // namespace

Skull::Skull( Room * room, Coordinate cref _position ) : Object( room, _position )
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
