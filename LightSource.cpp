#include "Lighting.hpp"

// LightSource::LightSource( Coordinate cref _position, Planc cref _radius, Color cref _tint, cdec _flicker ) : m_position( _position ), m_radius( _radius ), m_flicker( _flicker ), m_tint( _tint ) { }

LightSource::LightSource( Coordinate cref _position, Planc cref _radius ) : m_position( _position ), m_radius( _radius ) { }

Coordinate cref LightSource::position( ) const { return m_position; }

void LightSource::position( Coordinate cref _position )
{
    m_position = _position;
}

Planc LightSource::radius( ) const { return m_radius; }

void LightSource::radius( cdec _radius )
{
    m_radius = _radius;
}

// dec LightSource::flicker( ) const { return m_flicker; }

// void LightSource::flicker( cdec _flicker )
// {
//     m_flicker = _flicker
// }

// Color cref LightSource::tint( ) const { return m_tint; }

// void LightSource::tint( Color cref _tint )
// {
//     m_tint = _tint;
// }
