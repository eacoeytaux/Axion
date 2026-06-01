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

    Planc m_mass = 1.0;

    Coordinate m_position = ORIGIN;

    Vector m_velocity = V0;

    Polygon m_space = Polygon( );

    bool m_solid = false;

public:

    Matter( ) { }

    Matter( Coordinate cref position ) : m_position( position ) { }

    Planc mass( ) const { return ( m_mass ); }
    void mass( Planc cref mass ) { m_mass = mass; Assert( is_pos( mass ) ); }

    Coordinate position( ) const { return ( m_position ); }
    void position( Coordinate cref position ) { move( Vector( m_position, position ) ); }

    Polygon space( ) const { return ( m_space ); }
    void space( Polygon cref space ) { m_space = space; }

    bool moving( ) const { return ( velocity( ).has_magnitude( ) ); }

    void move( Vector cref distance ) { m_position += distance; }

    Vector velocity( ) const { return ( m_velocity ); }
    void velocity( Vector cref velocity ) { m_velocity = velocity; }

    void add_velocity( Vector cref velocity ) { m_velocity += velocity; }

    Vector force( ) const { return ( velocity( ) * mass( ) ); }

    void add_force( Vector cref force ) { add_velocity( force / mass( ) ); }

    bool solid( ) const { return ( m_solid ); }
    void solid( cbool solid ) { m_solid = solid; }

};

} // namespace physics
} // namespace axn

#endif /* Matter_hpp */
