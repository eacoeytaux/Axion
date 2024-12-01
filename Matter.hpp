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
    Matter( );
    Matter( const Coordinate & position, const Polygon & space = Polygon( ), dec mass = ZERO );
    Matter( const Coordinate & position, const Vector & velocity, const Polygon & space = Polygon( ), dec mass = ZERO );
    Matter( const Vector & position_velocity, const Polygon & space = Polygon( ), dec mass = ZERO ); // origin of velocity is the position

    Coordinate position( ) const;
    void position( const Coordinate & position );

    Polygon space( ) const;
    void space( const Polygon & space );

    dec mass( ) const;
    void mass( dec mass );

    Vector velocity( ) const;
    void velocity( const Vector & velocity );
    void add_velocity( const Vector & velocity );

    void move( const Vector & );

    bool solid( ) const;
    void solid( bool );

private:
    Coordinate m_position;
    Polygon m_space;
    dec m_mass;
    Vector m_velocity;

    bool m_solid = false;
};

} // namespace physics
} // namespace axn

#endif /* Matter_hpp */
