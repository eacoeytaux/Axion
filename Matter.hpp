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

    Matter( Coordinate cref position ) : m_position( position ) { }

    dec mass( ) const { return m_mass; }
    void mass( cdec _mass ) { m_mass = _mass; }

    Coordinate position( ) const { return m_position; }
    void position( Coordinate cref _position ) { move( Vector( m_position, _position ) ); }
    
    Vector velocity( ) const { return m_velocity; }
    void velocity( Vector cref _velocity ) { m_velocity = _velocity; }

    Polygon space( ) const { return m_space + Vector( position( ) ); }
    void space( Polygon cref _space ) { m_space = _space; }

    void add_velocity( Vector cref _velocity ) { m_velocity += _velocity; }
    void add_force( Vector cref _force ) { m_velocity += ( _force * mass( ) ); }

    void move( Vector cref _distance ) { m_position += _distance; }

    bool solid( ) const { return m_solid; }
    void solid( cbool _solid ) { m_solid = _solid; }
    
};

} // namespace physics
} // namespace axn

#endif /* Matter_hpp */
