#ifndef Matter_hpp
#define Matter_hpp

#include "Geometry.hpp"

namespace axn
{
namespace physics
{

class Matter
{
public:
    virtual ~Matter( ) { }
    Matter( const Coordinate & position = ORIGIN, const Shape & space = Shape( ), double mass = 0.0 );
    Matter( const Vector & position_velocity, const Shape & space = Shape( ), double mass = 0.0 ); // origin of velocity is the position

    Coordinate position( ) const;
    Matter & position( const Coordinate & position );
    Shape space( ) const;
    Matter & space( const Shape & space );
    double mass( ) const;
    Matter & mass( double mass );
    Vector velocity( ) const;
    Matter & velocity( const Vector & velocity );
    Matter & add_velocity( const Vector & velocity );
    Matter & move( const Vector & );

    bool solid( ) const;
    Matter & solid( bool );

private:
    Coordinate m_position;
    Shape m_space;
    double m_mass;
    Vector m_velocity;

    bool m_solid = false;
};

} // namespace physics
} // namespace axn

#endif /* Matter_hpp */
