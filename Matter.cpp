#include "Matter.hpp"

Matter::Matter( )
    : m_position( ORIGIN ),
    m_velocity( ZERO_VECTOR ),
    m_space( Polygon( ) ),
    m_mass( ZERO )
{
}

Matter::Matter( const Coordinate & _position, const Polygon & _space, const dec _mass )
    : m_position( _position ),
    m_velocity( ZERO_VECTOR ),
    m_space( _space ),
    m_mass( _mass )
{
}

Matter::Matter( const Coordinate & _position, const Vector & _velocity, const Polygon & _space, const dec _mass )
    : m_position( _position ),
    m_velocity( _velocity ),
    m_space( _space ),
    m_mass( _mass )
{
}

Matter::Matter( const Vector & _position_velocity, const Polygon & _space, const dec _mass )
    : m_position( _position_velocity.origin( ) ),
    m_velocity( _position_velocity ),
    m_space( _space ),
    m_mass( _mass )
{
}

dec Matter::mass( ) const
{
    return m_mass;
}

void Matter::mass( const dec _mass )
{
    m_mass = _mass;
}

Coordinate Matter::position( ) const
{
    return m_position;
}

void Matter::position( const Coordinate & _position )
{
    move( Vector( m_position, _position ) );
}

Polygon Matter::space( ) const
{
    return m_space + Vector( position( ) );
}

void Matter::space( const Polygon & _space )
{
    m_space = _space;
}

Vector Matter::velocity( ) const
{
    return m_velocity;
}

void Matter::velocity( const Vector & _velocity )
{
    m_velocity = _velocity;
}

void Matter::add_velocity( const Vector & _velocity )
{
    m_velocity += _velocity;
}

void Matter::move( const Vector & _distance )
{
    m_position += _distance;
}

bool Matter::solid( ) const
{
    return m_solid;
}

void Matter::solid( const bool _solid )
{
    m_solid = _solid;
}
