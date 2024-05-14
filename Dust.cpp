#include "Dust.hpp"

#include "World.hpp"

using mtmercy::Dust;

namespace
{
const Span<Planc> RADIUS_START = { 0.5, 0.75 };
const Planc RADIUS_INCREASE_RATE = 0.3;
const Span<double> ALPHA_START = { 0.75, 1.0 };
const double ALPHA_DECREASE_RATE = 0.05;
const Span<uint> SIDE_COUNT = { 6, 9 };
const Span<double> ROTATION_SPEED = { TAU / 24.0, TAU / 4. };
} // namespace

Dust::Dust( World * world, const Coordinate & _position, const Vector & _velocity, const Color & _color ) : Object( world )
{
    drawing_always_dirty( true );

    terrain_boundaries( false );
    gravity_ratio( 0.0 );

    position( _position );
    velocity( _velocity );

    m_radius = Random::rPlanc( RADIUS_START );
    m_alpha = Random::rPlanc( ALPHA_START );
    m_sides = Random::rint( SIDE_COUNT );
    m_rotation = Random::rAngle( );
    m_rotation_speed = Random::flipped( Random::rAngle( ROTATION_SPEED ) );
    m_color = _color;
}

Dust & Dust::update( )
{
    Object::update( );

    m_radius += RADIUS_INCREASE_RATE;
    m_rotation += m_rotation_speed;
    m_alpha -= ALPHA_DECREASE_RATE;
    if( m_alpha <= 0.0 )
    {
        mark_deleted( );
    }

    return *this;
}

const Dust & Dust::render( ) const
{
    Object::render( );

    draw( m_color.a( m_alpha ), Polygon::equilateral( m_sides, m_radius ).rotate( m_rotation ) );

    return *this;
}
