#ifndef Vector_hpp
#define Vector_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"
#include "Line.hpp"

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
    Vector & halve( );

    Angle angle( ) const;
    Vector & rotate_to_angle( const Angle & angle );
    Vector & rotate( const Angle & rotation );

    Vector & flatten( const Angle & );

    Vector operator-( ) const;

    Vector operator+( const Vector & ) const;
    Vector operator-( const Vector & ) const;
    Vector & operator+=( const Vector & );
    Vector & operator-=( const Vector & );

    Vector operator*( double scale ) const;
    Vector operator/( double scale ) const;
    Vector & operator*=( double scale );
    Vector & operator/=( double scale );

    bool operator==( const Vector & ) const;
    bool operator!=( const Vector & ) const;

    operator Line( ) const;

private:
    Coordinate m_origin = ORIGIN;
    Planc m_dx = 0.0, m_dy = 0.0;
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

const Vector ZERO_VECTOR( 0.0, 0.0 );
const Vector X_HAT( 1.0, 0.0 );
const Vector Y_HAT( 0.0, 1.0 );

} // namespace geometry
} // namespace axn

#endif /* Vector_hpp */
