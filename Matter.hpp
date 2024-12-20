#ifndef Matter_hpp
#define Matter_hpp

#include "Geometry.hpp"

namespace axn
{
namespace physics
{

class Matter
{
private:
    dec m_mass = ZERO;

    Coordinate m_position = ORIGIN;

    Vector m_velocity = V0;

    Polygon m_space = Polygon( );

    bool m_solid = false;

public:
    Matter( ) { }

    Matter( const Coordinate & position, const Polygon & space, const dec mass = ZERO ) : m_position( position ), m_space( space ), m_mass( mass ) { }
    Matter( const Coordinate & position, const Vector & velocity, const Polygon & space = Polygon( ), const dec mass = ZERO ) : m_position( position ), m_velocity( velocity ), m_space( space ), m_mass( mass ) { }

    // origin of velocity is the position
    Matter( const Vector & position_velocity, const Polygon & space = Polygon( ), const dec mass = ZERO ) : m_position( position_velocity.origin( ) ), m_velocity( Vector( position_velocity.dx( ), position_velocity.dy( ) ) ), m_space( space ), m_mass( mass ) { }

    dec mass( ) const { return m_mass; }
    void mass( const dec _mass ) { m_mass = _mass; }

    Coordinate position( ) const { return m_position; }
    void position( const Coordinate & _position ) { move( Vector( m_position, _position ) ); }

    Polygon space( ) const { return m_space + Vector( position( ) ); }
    void space( const Polygon & _space ) { m_space = _space; }

    Vector velocity( ) const { return m_velocity; }
    void velocity( const Vector & _velocity ) { m_velocity = _velocity; }

    void add_velocity( const Vector & _velocity ) { m_velocity += _velocity; }

    void move( const Vector & _distance ) { m_position += _distance; }

    bool solid( ) const { return m_solid; }
    void solid( const bool _solid ) { m_solid = _solid; }
};

} // namespace physics
} // namespace axn

#endif /* Matter_hpp */
