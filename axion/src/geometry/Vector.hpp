#ifndef Vector_hpp
#define Vector_hpp

#include "axn.hpp"
#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"

namespace axn {
namespace geometry {

class Vector {
public:
    virtual ~Vector( ) {}
    Vector( const planc & dx, const planc & dy, const Coordinate & c = ORIGIN );
    Vector( const Coordinate & c = ORIGIN );
    Vector( const Coordinate & start, const Coordinate & end );

    planc dx( ) const;
    Vector & dx( const planc & dx );
    planc dy( ) const;
    Vector & dy( const planc & dy );
    Vector & dxdy( const planc & dx, const planc & dy );

    Coordinate origin( ) const;
    Vector & origin( const Coordinate & origin );
    Coordinate destination( ) const;
    Vector & destination( const Coordinate destination );

    planc magnitude( ) const;
    Vector & magnitude( const planc & magnitude );
    Vector & extend( const planc & length );
    Vector & normalize( );

    Vector half( ) const;
    Vector & halve( );

    Angle angle( ) const;
    Vector & rotate_to_angle( const Angle & angle );
    Vector & rotate( const Angle & rotation );

    Vector operator-( ) const;

    Vector operator+( const Vector & ) const;
    Vector operator-( const Vector & ) const;
    Vector & operator+=( const Vector & );
    Vector & operator-=( const Vector & );

    Vector operator*( float scale ) const;
    Vector operator/( float scale ) const;
    Vector & operator*=( float scale );
    Vector & operator/=( float scale );

    bool operator==( const Vector & ) const;
    bool operator!=( const Vector & ) const;

private:
    Coordinate m_origin = ORIGIN;
    planc m_dx = 0.f, m_dy = 0.f;
};

class VectorA : public Vector {
public:
    VectorA( const Angle & angle, const planc & magnitude = 1.f,
             const Coordinate & origin = ORIGIN );
};

const Vector X_HAT( (planc)1.f, (planc)0.f );
const Vector Y_HAT( (planc)0.f, (planc)1.f );

} // namespace geometry
} // namespace axn

#endif /* Vector_hpp */
