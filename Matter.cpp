#include "Matter.hpp"

Matter::Matter( const Coordinate & _position, const Polygon & _space, const dec _mass )
    : m_position( _position ), m_space( _space ), m_mass( _mass )
{
}

Matter::Matter( const Vector & _position_velocity, const Polygon & _space, const dec _mass )
    : m_position( _position_velocity.origin( ) ), m_velocity( _position_velocity ), m_space( _space ), m_mass( _mass )
{
}

dec Matter::mass( ) const
{
    return m_mass;
}

Matter & Matter::mass( const dec _mass )
{
    m_mass = _mass;
    return *this;
}

Coordinate Matter::position( ) const
{
    return m_position;
}

Matter & Matter::position( const Coordinate & _position )
{
    ;
    move( Vector( m_position, _position ) );
    return *this;
}

Polygon Matter::space( ) const
{
    return m_space + Vector( position( ) );
}

Matter & Matter::space( const Polygon & _space )
{
    m_space = _space;
    return *this;
}

Vector Matter::velocity( ) const
{
    return m_velocity;
}

Matter & Matter::velocity( const Vector & _velocity )
{
    m_velocity = _velocity;
    return *this;
}

Matter & Matter::add_velocity( const Vector & _velocity )
{
    m_velocity += _velocity;
    return *this;
}

Matter & Matter::move( const Vector & _distance )
{
    if( _distance.has_magnitude( ) )
    {
        m_position += _distance;
    }
    return *this;
}

bool Matter::solid( ) const { return m_solid; }
Matter & Matter::solid( const bool _solid )
{
    m_solid = _solid;
    return *this;
}
