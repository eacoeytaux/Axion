#include "axn/graphics/Lighting.hpp"

LightSource::LightSource( Point cref in_position, Planc cref in_radius ) : m_position( in_position ), m_radius( in_radius ) { }

Point cref LightSource::position( ) const { return( m_position ); }

void LightSource::position( Point cref in_position )
{
    m_position = in_position;
}

Planc LightSource::radius( ) const { return( m_radius ); }

void LightSource::radius( Planc cref in_radius )
{
    m_radius = in_radius;
}

// dec LightSource::flicker( ) const { return( m_flicker ); }

// void LightSource::flicker( dec in_flicker )
// {
//     m_flicker = in_flicker
// }

// Color cref LightSource::tint( ) const { return( m_tint ); }

// void LightSource::tint( Color cref in_tint )
// {
//     m_tint = in_tint;
// }
