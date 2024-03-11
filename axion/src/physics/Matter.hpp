#ifndef Matter_hpp
#define Matter_hpp

#include "axn.hpp"

#include "Geometry.hpp"

namespace axn {
namespace physics {

class Matter {
public:
    virtual ~Matter( ) {}
    Matter( const Coordinate & position = ORIGIN, const Shape & space = Shape( ),
            ufloat mass = 0.f );
    Matter( const Vector & position_velocity, const Shape & space = Shape( ),
            ufloat mass = 0.f ); // origin of velocity is the position

    Coordinate position( ) const;
    Matter & position( const Coordinate & position );
    Shape space( ) const;
    Matter & space( const Shape & space );
    ufloat mass( ) const;
    Matter & mass( ufloat mass );
    Vector velocity( ) const;
    Matter & velocity( const Vector & velocity );
    Matter & add_velocity( const Vector & velocity );
    Matter & move( const Vector & );

private:
    Coordinate m_position;
    Shape m_space;
    ufloat m_mass;
    Vector m_velocity;
};

} // namespace physics
} // namespace axn

#endif /* Matter_hpp */
