#include "Lighting.hpp"

using axn::graphics::LightSource;

// LightSource::LightSource( const Coordinate & _position, const Planc & _radius, const Color & _tint, const double _flicker ) : m_position( _position ), m_radius( _radius ), m_flicker( _flicker ), m_tint( _tint ) { }

LightSource::LightSource( const Coordinate & _position, const Planc & _radius ) : m_position( _position ), m_radius( _radius ) { }

const Coordinate & LightSource::position( ) const { return m_position; }

LightSource & LightSource::position( const Coordinate & _position )
{
    m_position = _position;
    return *this;
}

Planc LightSource::radius( ) const { return m_radius; }

LightSource & LightSource::radius( const double _radius )
{
    m_radius = _radius;
    return *this;
}

// double LightSource::flicker( ) const { return m_flicker; }

// LightSource & LightSource::flicker( const double _flicker )
//{
//     m_flicker = _flicker;
//     return *this;
// }

// const Color & LightSource::tint( ) const { return m_tint; }

// LightSource & LightSource::tint( const Color & _tint )
//{
//     m_tint = _tint;
//     return *this;
// }

bool LightSource::operator==( const LightSource & _light_source ) const
{
    return ( ( m_position == _light_source.m_position ) &&
             ( m_radius == _light_source.m_radius ) );
    //             ( m_radius == _light_source.m_radius ) &&
    //             ( m_flicker == _light_source.m_flicker ) &&
    //             ( m_tint == _light_source.m_tint ) );
}

bool LightSource::operator!=( const LightSource & _light_source ) const { return !( *this == _light_source ); }
