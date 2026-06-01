#include "mt/environment/misc/Dust.hpp"

#include "axn/world/World.hpp"

var_const( Span<Planc> RADIUS_START = { 0.5, 0.75 } );
var_const( Planc RADIUS_INCREASE_RATE = ( 0.1 ) );
var_const( Span<dec> ALPHA_START = { 0.25, 0.75 } );
var_const( dec ALPHA_DECREASE_RATE = ( 0.02625 ) );
var_const( Span<uint> SIDE_COUNT = { 6, 9 } );
var_const( Span<Angle> ROTATION_SPEED = { Angle::fraction( 28.0 ), Angle::fraction( 8.0 ) } );

Dust::Dust( ptr<Room> in_room, Point cref in_position, Vector cref in_velocity, Color cref in_color ) : Object( in_room )
{
    needs_render_always( true );

    terrain_bound( false );
    no_gravity( );

    position( in_position );
    velocity( in_velocity );

    m_radius = Random::rPlanc( RADIUS_START );
    m_alpha = Random::rPlanc( ALPHA_START );
    m_sides = Random::rint( SIDE_COUNT );
    m_rotation = Random::rAngle( );
    m_rotation_speed = Random::negated( Random::rAngle( ROTATION_SPEED ) );
    m_color = in_color;
}

void Dust::update( )
{
    m_radius += RADIUS_INCREASE_RATE;
    m_rotation += m_rotation_speed;
    m_alpha -= ALPHA_DECREASE_RATE;
    if( !is_pos( m_alpha ) )
    {
        mark_to_delete( );
    }

    Object::update( );
}

void Dust::render( )
{
    Object::render( );

    if( is_pos( m_alpha ) )
    {
        Polygon poly = Polygon::equilateral( m_sides, m_radius );
        poly.rotate( m_rotation );
        draw( m_color.a( m_alpha ), poly );
    }
}
