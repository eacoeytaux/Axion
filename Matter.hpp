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

    dec m_mass = 0.0;

    Coordinate m_position = ORIGIN;

    Vector m_velocity = V0;

    Polygon m_space = Polygon( );

    bool m_solid = false;

public:

    Matter( ) { }

    Matter( Coordinate cref position, Polygon cref space, cdec mass = 0.0 ) : m_position( position ), m_space( space ), m_mass( mass ) { }
    Matter( Coordinate cref position, Vector cref velocity, Polygon cref space = Polygon( ), cdec mass = 0.0 ) : m_position( position ), m_velocity( velocity ), m_space( space ), m_mass( mass ) { }

    // origin of velocity is the position
    Matter( Vector cref position_velocity, Polygon cref space = Polygon( ), cdec mass = 0.0 ) : m_position( position_velocity.origin( ) ), m_velocity( Vector( position_velocity.dx( ), position_velocity.dy( ) ) ), m_space( space ), m_mass( mass ) { }

    dec mass( ) const { return m_mass; }
    void mass( cdec _mass ) { m_mass = _mass; }

    Coordinate position( ) const { return m_position; }
    void position( Coordinate cref _position ) { move( Vector( m_position, _position ) ); }

    Polygon space( ) const { return m_space + Vector( position( ) ); }
    void space( Polygon cref _space ) { m_space = _space; }

    Vector velocity( ) const { return m_velocity; }
    void velocity( Vector cref _velocity ) { m_velocity = _velocity; }

    void add_velocity( Vector cref _velocity ) { m_velocity += _velocity; }

    void move( Vector cref _distance ) { m_position += _distance; }

    bool solid( ) const { return m_solid; }
    void solid( cbool _solid ) { m_solid = _solid; }
};

} // namespace physics
} // namespace axn

#endif /* Matter_hpp */
