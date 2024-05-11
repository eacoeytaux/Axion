#include "Matter.hpp"

using axn::physics::Matter;

Matter::Matter( const Coordinate & _position, const Shape & _space, const double _mass )
    : m_position( _position ), m_space( _space ), m_mass( _mass )
{
}

Matter::Matter( const Vector & _position_velocity, const Shape & _space, const double _mass )
    : m_position( _position_velocity.origin( ) ), m_velocity( _position_velocity ), m_space( _space ), m_mass( _mass )
{
}

double Matter::mass( ) const
{
    return m_mass;
}

Matter & Matter::mass( const double _mass )
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
    Vector d_position( m_position, _position );
    move( d_position );
    return *this;
}

Shape Matter::space( ) const
{
    return m_space + Vector( position( ) );
}

Matter & Matter::space( const Shape & _space )
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
    m_position += _distance;
    return *this;
}

bool Matter::solid( ) const { return m_solid; }
Matter & Matter::solid( const bool _solid )
{
    m_solid = _solid;
    return *this;
}
