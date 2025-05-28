#ifndef Vector_hpp
#define Vector_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"

namespace axn
{
namespace geometry
{

axnclass( Vector )
{

private:

    Planc m_dx = P0;
    Planc m_dy = P0;

    Coordinate m_origin = ORIGIN;

public:

    Vector( ) { }

    Vector( Planc cref dx, Planc cref dy ) : m_dx( dx ), m_dy( dy ) { }
    Vector( Planc cref dx, Planc cref dy, Coordinate cref origin ) : m_dx( dx ), m_dy( dy ), m_origin( origin ) { }

    Vector( Coordinate cref c ) : m_dx( c.x( ) ), m_dy( c.y( ) ), m_origin( ORIGIN ) { }

    Vector( Coordinate cref start, Coordinate cref end ) : m_dx( end.x( ) - start.x( ) ), m_dy( end.y( ) - start.y( ) ), m_origin( start ) { }

    Planc dx( ) const { return m_dx; }
    Vector & dx( Planc cref dx ) { m_dx = dx; rethis; }

    Planc dy( ) const { return m_dy; }
    Vector & dy( Planc cref dy ) { m_dy = dy; rethis; }

    Coordinate origin( ) const { return m_origin; }
    Vector & origin( Coordinate cref origin ) { m_origin = origin; rethis; }

    Coordinate destination( ) const { return Coordinate( origin( ).x( ) + dx( ), origin( ).y( ) + dy( ) ); }
    Vector & destination( Coordinate c ) { dx( c.x( ) - origin( ).x( ) ); dy( c.y( ) - origin( ).y( ) ); rethis; }

    bool has_magnitude( ) const { return ( dx( ) || dy( ) ); }
    Planc magnitude( ) const { return origin( ).distance_to( destination( ) ); }

    Vector & magnitude( Planc cref m ) { Angle a = angle( ); dx( a.cos( ) * m ); dy( a.sin( ) * m ); rethis; }

    Vector & extend( Planc cref p ) { return magnitude( magnitude( ) + p ); }

    Vector & normalize( ) { return magnitude( 1.0 ); }

    Angle angle( ) const { return Angle( m_dx, m_dy ); }
    Vector & rotate( Angle cref a ) { return rotate_to_angle( angle( ) + a ); }
    Vector & rotate_to_angle( Angle cref a ) { if( angle( ) != a ) { Planc _magnitude = magnitude( ); dx( _magnitude * a.cos( ) ); dy( _magnitude * a.sin( ) ); } rethis; }

    Vector & flatten( Angle cref a ) { rotate( -a ); dy( 0.0 ); rotate( a ); rethis; }

    Vector & operator=( Coordinate cref c ) { origin( ORIGIN ); dx( c.x( ) ); dy( c.y( ) ); rethis; }

    Vector operator-( ) const { return Vector( -dx( ), -dy( ), origin( ) ); }

    Vector operator+( Vector cref v ) const { return Vector( dx( ) + v.dx( ), dy( ) + v.dy( ), origin( ) ); }
    Vector operator-( Vector cref v ) const { return Vector( dx( ) - v.dx( ), dy( ) - v.dy( ), origin( ) ); }

    Vector operator*( cdec scale ) const { return Vector( origin( ), Coordinate( origin( ).x( ) + ( dx( ) * scale ), origin( ).y( ) + ( dy( ) * scale ) ) ); }
    Vector operator/( cdec scale ) const { return Vector( origin( ), Coordinate( origin( ).x( ) + ( dx( ) / scale ), origin( ).y( ) + ( dy( ) / scale ) ) ); }

    Vector & operator+=( Vector cref v ) { dx( dx( ) + v.dx( ) ); dy( dy( ) + v.dy( ) ); rethis; }
    Vector & operator-=( Vector cref v ) { dx( dx( ) - v.dx( ) ); dy( dy( ) - v.dy( ) ); rethis; }

    Vector & operator*=( cdec scale ) { dx( dx( ) * scale ); dy( dy( ) * scale ); rethis; }
    Vector & operator/=( cdec scale ) { dx( dx( ) / scale ); dy( dy( ) / scale ); rethis; }

    default_equal( Vector );
    
};

cVector V0( 0.0, 0.0 );
cVector XHAT( 1.0, 0.0 );
cVector YHAT( 0.0, 1.0 );

inline Vector VectorX( Planc cref dx ) { return Vector( dx, P0 ); }
inline Vector VectorX( Planc cref dx, Coordinate cref origin ) { return Vector( dx, P0, origin ); }

inline Vector VectorY( Planc cref dy ) { return Vector( P0, dy ); }
inline Vector VectorY( Planc cref dy, Coordinate cref origin ) { return Vector( P0, dy, origin ); }

inline Vector VectorA( Angle cref a ) { return Vector( a.cos( ), a.sin( ) ); }
inline Vector VectorA( Angle cref a, Coordinate cref origin ) { return Vector( a.cos( ), a.sin( ), origin ); }

inline Vector VectorA( Angle cref a, Planc cref magnitude ) { return Vector( a.cos( ) * magnitude, a.sin( ) * magnitude ); }
inline Vector VectorA( Angle cref a, Planc cref magnitude, Coordinate cref origin ) { return Vector( a.cos( ) * magnitude, a.sin( ) * magnitude, origin ); }

inline Coordinate::Coordinate( Vector cref v ) { *this = v.destination( ); }

inline Coordinate & Coordinate::move( Planc cref x, Planc cref y ) { Coordinate::x( Coordinate::x( ) + x ); Coordinate::y( Coordinate::y( ) + y ); rethis; }
inline Coordinate & Coordinate::rotate( Angle cref a ) { rethis = Vector( *this ).rotate( a ).destination( ); }
inline Coordinate & Coordinate::rotate( Angle cref a, Coordinate cref c ) { rethis = Vector( c, *this ).rotate( a ).destination( ); }
inline Coordinate & Coordinate::mirror_x( ) { return mirror( XHAT ); }
inline Coordinate & Coordinate::mirror_y( ) { return mirror( YHAT ); }
inline Coordinate & Coordinate::mirror( Vector cref v )
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

inline Coordinate Coordinate::operator+( Vector cref v ) const { return Coordinate( x( ) + v.dx( ), y( ) + v.dy( ) ); }
inline Coordinate Coordinate::operator-( Vector cref v ) const { return Coordinate( x( ) - v.dx( ), y( ) - v.dy( ) ); }

inline Coordinate & Coordinate::operator+=( Vector cref v ) { x( x( ) + v.dx( ) ); y( y( ) + v.dy( ) ); rethis; }
inline Coordinate & Coordinate::operator-=( Vector cref v ) { x( x( ) - v.dx( ) ); y( y( ) - v.dy( ) ); rethis; }

} // namespace geometry
} // namespace axn

#endif /* Vector_hpp */
