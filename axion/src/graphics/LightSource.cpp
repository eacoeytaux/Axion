#include "Lighting.hpp"

AXN_NAMESPACES
using axn::graphics::LightSource;

LightSource::LightSource( const Coordinate & _position, const planc & _distance,
                          const Color & _tint, const ufloat _flicker )
    : m_position( _position ), m_distance( _distance ), m_flicker( _flicker ), m_tint( _tint ) {
}

const Coordinate & LightSource::position( ) const { return m_position; }

LightSource & LightSource::position( const Coordinate & _position ) {
    m_position = _position;
    return *this;
}

planc LightSource::distance( ) const { return m_distance; }

LightSource & LightSource::distance( const ufloat _distance ) {
    m_distance = _distance;
    return *this;
}

ufloat LightSource::flicker( ) const { return m_flicker; }

LightSource & LightSource::flicker( const ufloat _flicker ) {
    m_flicker = _flicker;
    return *this;
}

const Color & LightSource::tint( ) const { return m_tint; }

LightSource & LightSource::tint( const Color & _tint ) {
    m_tint = _tint;
    return *this;
}

bool LightSource::operator==( const LightSource & _light_source ) const {
    return ( ( m_position == _light_source.m_position ) &&
             ( m_distance == _light_source.m_distance ) &&
             ( m_flicker == _light_source.m_flicker ) && ( m_tint == _light_source.m_tint ) );
}

bool LightSource::operator!=( const LightSource & _light_source ) const {
    return !( *this == _light_source );
}
