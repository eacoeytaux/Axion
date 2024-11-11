#include "Lighting.hpp"

// LightSource::LightSource( const Coordinate & _position, const Planc & _radius, const Color & _tint, const dec _flicker ) : m_position( _position ), m_radius( _radius ), m_flicker( _flicker ), m_tint( _tint ) { }

LightSource::LightSource( const Coordinate & _position, const Planc & _radius ) : m_position( _position ), m_radius( _radius ) { }

const Coordinate & LightSource::position( ) const { return m_position; }

void LightSource::position( const Coordinate & _position )
{
    m_position = _position;
}

Planc LightSource::radius( ) const { return m_radius; }

void LightSource::radius( const dec _radius )
{
    m_radius = _radius;
}

// dec LightSource::flicker( ) const { return m_flicker; }

// void LightSource::flicker( const dec _flicker )
// {
//     m_flicker = _flicker
// }

// const Color & LightSource::tint( ) const { return m_tint; }

// void LightSource::tint( const Color & _tint )
// {
//     m_tint = _tint;
// }
