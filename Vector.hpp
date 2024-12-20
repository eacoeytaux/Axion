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
private:
    Planc m_dx = P0;
    Planc m_dy = P0;

    Coordinate m_origin = ORIGIN;

public:
    Vector( ) { }

    Vector( const Planc & dx, const Planc & dy ) : m_dx( dx ), m_dy( dy ) { }
    Vector( const Planc & dx, const Planc & dy, const Coordinate & origin ) : m_dx( dx ), m_dy( dy ), m_origin( origin ) { }

    Vector( const Coordinate & c ) : m_dx( c.x( ) ), m_dy( c.y( ) ), m_origin( ORIGIN ) { }

    Vector( const Coordinate & start, const Coordinate & end ) : m_dx( end.x( ) - start.x( ) ), m_dy( end.y( ) - start.y( ) ), m_origin( start ) { }

    static Vector X( const Planc & dx ) { return Vector( dx, P0 ); }
    static Vector X( const Planc & dx, const Coordinate & origin ) { return Vector( dx, P0, origin ); }

    static Vector Y( const Planc & dy ) { return Vector( P0, dy ); }
    static Vector Y( const Planc & dy, const Coordinate & origin ) { return Vector( P0, dy, origin ); }

    static Vector A( const Angle & a ) { return Vector( a.cos( ), a.sin( ) ); }
    static Vector A( const Angle & a, const Coordinate & origin ) { return Vector( a.cos( ), a.sin( ), origin ); }

    static Vector A( const Angle & a, const Planc & magnitude ) { return Vector( a.cos( ) * magnitude, a.sin( ) * magnitude ); }
    static Vector A( const Angle & a, const Planc & magnitude, const Coordinate & origin ) { return Vector( a.cos( ) * magnitude, a.sin( ) * magnitude, origin ); }

    Planc dx( ) const { return m_dx; }
    Vector & dx( const Planc & dx ) { m_dx = dx; rethis; }

    Planc dy( ) const { return m_dy; }
    Vector & dy( const Planc & dy ) { m_dy = dy; rethis; }

    Coordinate origin( ) const { return m_origin; }
    Vector & origin( const Coordinate & origin ) { m_origin = origin; rethis; }

    Coordinate destination( ) const { return Coordinate( origin( ).x( ) + dx( ), origin( ).y( ) + dy( ) ); }
    Vector & destination( Coordinate c ) { dx( c.x( ) - origin( ).x( ) ); dy( c.y( ) - origin( ).y( ) ); rethis; }

    bool has_magnitude( ) const { return ( dx( ) || dy( ) ); }
    Planc magnitude( ) const { return origin( ).distance_to( destination( ) ); }

    Vector & magnitude( const Planc & m ) { Angle a = angle( ); dx( a.cos( ) * m ); dy( a.sin( ) * m ); rethis; }

    Vector & extend( const Planc & p ) { return magnitude( magnitude( ) + p ); }

    Vector & normalize( ) { return magnitude( ONE ); }

    Angle angle( ) const { return Angle( m_dx, m_dy ); }
    Vector & rotate( const Angle & a ) { return rotate_to_angle( angle( ) + a ); }
    Vector & rotate_to_angle( const Angle & a ) { if( angle( ) != a ) { Planc _magnitude = magnitude( ); dx( _magnitude * a.cos( ) ); dy( _magnitude * a.sin( ) ); } rethis; }

    Vector & flatten( const Angle & a ) { rotate( -a ); dy( ZERO ); rotate( a ); rethis; }

    Vector & operator=( const Coordinate & c ) { origin( ORIGIN ); dx( c.x( ) ); dy( c.y( ) ); rethis; }

    Vector operator-( ) const { return Vector( -dx( ), -dy( ), origin( ) ); }

    Vector operator+( const Vector & v ) const { return Vector( dx( ) + v.dx( ), dy( ) + v.dy( ), origin( ) ); }
    Vector operator-( const Vector & v ) const { return Vector( dx( ) - v.dx( ), dy( ) - v.dy( ), origin( ) ); }

    Vector operator*( const dec scale ) const { return Vector( origin( ), Coordinate( origin( ).x( ) + ( dx( ) * scale ), origin( ).y( ) + ( dy( ) * scale ) ) ); }
    Vector operator/( const dec scale ) const { return Vector( origin( ), Coordinate( origin( ).x( ) + ( dx( ) / scale ), origin( ).y( ) + ( dy( ) / scale ) ) ); }

    Vector & operator+=( const Vector & v ) { dx( dx( ) + v.dx( ) ); dy( dy( ) + v.dy( ) ); rethis; }
    Vector & operator-=( const Vector & v ) { dx( dx( ) - v.dx( ) ); dy( dy( ) - v.dy( ) ); rethis; }

    Vector & operator*=( const dec scale ) { dx( dx( ) * scale ); dy( dy( ) * scale ); rethis; }
    Vector & operator/=( const dec scale ) { dx( dx( ) / scale ); dy( dy( ) / scale ); rethis; }

    default_equal( Vector );
};

const Vector V0( ZERO, ZERO );
const Vector X_HAT( ONE, ZERO );
const Vector Y_HAT( ZERO, ONE );

inline Coordinate::Coordinate( const Vector & v ) { *this = v.destination( ); }

inline Coordinate & Coordinate::move( const Planc & x, const Planc & y ) { Coordinate::x( Coordinate::x( ) + x ); Coordinate::y( Coordinate::y( ) + y ); rethis; }
inline Coordinate & Coordinate::rotate( const Angle & a ) { rethis = Vector( *this ).rotate( a ).destination( ); }
inline Coordinate & Coordinate::rotate( const Angle & a, const Coordinate & c ) { rethis = Vector( c, *this ).rotate( a ).destination( ); }
inline Coordinate & Coordinate::mirror_x( ) { return mirror( X_HAT ); }
inline Coordinate & Coordinate::mirror_y( ) { return mirror( Y_HAT ); }
inline Coordinate & Coordinate::mirror( const Vector & v )
{
    x( x( ) - v.origin( ).x( ) );
    y( y( ) - v.origin( ).y( ) );

    rotate( -v.angle( ) );

    y( -y( ) );

    rotate( v.angle( ) );

    x( x( ) + v.origin( ).x( ) );
    y( y( ) + v.origin( ).y( ) );

    rethis;
}

inline Coordinate Coordinate::operator+( const Vector & v ) const { return Coordinate( x( ) + v.dx( ), y( ) + v.dy( ) ); }
inline Coordinate Coordinate::operator-( const Vector & v ) const { return Coordinate( x( ) - v.dx( ), y( ) - v.dy( ) ); }

inline Coordinate & Coordinate::operator+=( const Vector & v ) { x( x( ) + v.dx( ) ); y( y( ) + v.dy( ) ); rethis; }
inline Coordinate & Coordinate::operator-=( const Vector & v ) { x( x( ) - v.dx( ) ); y( y( ) - v.dy( ) ); rethis; }

} // namespace geometry
} // namespace axn

#endif /* Vector_hpp */
