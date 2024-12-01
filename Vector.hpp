#ifndef Vector_hpp
#define Vector_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"

namespace axn
{
namespace geometry
{

class Vector
{
public:
    virtual ~Vector( ) { }
    Vector( const Planc & dx, const Planc & dy, const Coordinate & c = ORIGIN );
    Vector( const Coordinate & c = ORIGIN );
    Vector( const Coordinate & start, const Coordinate & end );

    Planc dx( ) const;
    Vector & dx( const Planc & dx );
    Planc dy( ) const;
    Vector & dy( const Planc & dy );
    Vector & dxdy( const Planc & dx, const Planc & dy );

    Coordinate origin( ) const;
    Vector & origin( const Coordinate & origin );
    Coordinate destination( ) const;
    Vector & destination( const Coordinate destination );

    bool has_magnitude( ) const;
    Planc magnitude( ) const;
    Vector & magnitude( const Planc & magnitude );
    Vector & extend( const Planc & length );
    Vector & normalize( );

    Vector half( ) const;

    Angle angle( ) const;
    Vector & rotate_to_angle( const Angle & angle );
    Vector & rotate( const Angle & rotation );

    Vector & flatten( const Angle & );

    Vector operator-( ) const;

    Vector operator+( const Vector & ) const;
    Vector operator-( const Vector & ) const;
    Vector & operator+=( const Vector & );
    Vector & operator-=( const Vector & );

    Vector operator*( dec scale ) const;
    Vector operator/( dec scale ) const;
    Vector & operator*=( dec scale );
    Vector & operator/=( dec scale );

    Vector & operator=( const Coordinate & );

    default_equal( Vector );

private:
    Coordinate m_origin = ORIGIN;
    Planc m_dx = ZERO, m_dy = ZERO;
};

class VectorX : public Vector
{
public:
    virtual ~VectorX( ) { }
    VectorX( const Planc & dx, const Coordinate & origin = ORIGIN );
};

class VectorY : public Vector
{
public:
    virtual ~VectorY( ) { }
    VectorY( const Planc & dy, const Coordinate & origin = ORIGIN );
};

class VectorA : public Vector
{
public:
    virtual ~VectorA( ) { }
    VectorA( const Angle & angle, const Planc & magnitude = 1.0, const Coordinate & origin = ORIGIN );
    VectorA( const Angle & angle, const Coordinate & origin );
};

const Vector ZERO_VECTOR( ZERO, ZERO );
const Vector X_HAT( ONE, ZERO );
const Vector Y_HAT( ZERO, ONE );

} // namespace geometry
} // namespace axn

#endif /* Vector_hpp */
