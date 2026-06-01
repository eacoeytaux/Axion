#ifndef Matter_hpp
#define Matter_hpp

#include "axn/geometry/Geometry.hpp"

namespace axn
{
namespace physics
{

class Matter
{

private:

    Planc m_mass = 1.0;

    Point m_position = ORIGIN;

    Vector m_velocity = V0;

    Polygon m_space = Polygon( );

    bool m_solid = false;

public:

    Matter( ) { }

    Matter( Point cref in_position ) : m_position( in_position ) { }

    Planc mass( ) const { return( m_mass ); }
    virtual void mass( Planc cref in_mass ) { m_mass = in_mass; Assert( is_pos( in_mass ) ); }

    Point position( ) const { return( m_position ); }
    virtual void position( Point cref in_position ) { m_position = in_position; }

    Polygon space( ) const { return( m_space ); }
    virtual void space( Polygon cref in_space ) { m_space = in_space; }

    bool moving( ) const { return( velocity( ).has_magnitude( ) ); }

    virtual void move( Vector cref in_distance ) { m_position += in_distance; }

    Vector velocity( ) const { return( m_velocity ); }
    virtual void velocity( Vector cref in_velocity ) { m_velocity = in_velocity; }

    virtual void add_velocity( Vector cref in_velocity ) { m_velocity += in_velocity; }

    Vector force( ) const { return( velocity( ) * mass( ) ); }

    virtual void add_force( Vector cref in_force ) { add_velocity( in_force / mass( ) ); }

    bool solid( ) const { return( m_solid ); }
    virtual void solid( bool in_solid ) { m_solid = in_solid; }

};

} // namespace physics
} // namespace axn

#endif /* Matter_hpp */
