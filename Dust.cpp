#include "Dust.hpp"
#include "World.hpp"

using mtmercy::Dust;

namespace
{
const Span<Planc> RADIUS_START = { 0.5, 0.75 };
const Planc RADIUS_INCREASE_RATE = 0.2;
const Span<dec> ALPHA_START = { 0.75, 1.0 };
const dec ALPHA_DECREASE_RATE = 0.0525;
const Span<uint> SIDE_COUNT = { 6, 9 };
const Span<dec> ROTATION_SPEED = { TAU / 24.0, TAU / 4. };
} // namespace

Dust::Dust( World * world, const Coordinate & _position, const Vector & _velocity, const Color & _color ) : Object( world )
{
    needs_render_always( true );

    terrain_boundaries( false );
    no_gravity( );

    position( _position );
    velocity( _velocity );

    m_radius = Random::rPlanc( RADIUS_START );
    m_alpha = Random::rPlanc( ALPHA_START );
    m_sides = Random::rint( SIDE_COUNT );
    m_rotation = Random::rAngle( );
    m_rotation_speed = Random::negative( Random::rAngle( ROTATION_SPEED ) );
    m_color = _color;
}

void Dust::update( )
{
    m_radius += RADIUS_INCREASE_RATE;
    m_rotation += m_rotation_speed;
    m_alpha -= ALPHA_DECREASE_RATE;
    if( m_alpha <= 0.0 )
    {
        mark_deleted( );
    }
    
    Object::update( );
}

void Dust::render( )
{
    Object::render( );

    if( m_alpha >= 0.0 )
    {
        Polygon poly = Polygon::equilateral( m_sides, m_radius );
        poly.rotate( m_rotation );
        draw( m_color.a( m_alpha ), poly );
    }
}
