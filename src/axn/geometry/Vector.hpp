#ifndef Vector_hpp
#define Vector_hpp

#include "axn/geometry/Planc.hpp"
#include "axn/geometry/Point.hpp"
#include "axn/geometry/Angle.hpp"

namespace axn
{
namespace geometry
{

class Vector
{

private:

    Planc m_dx = P0;
    Planc m_dy = P0;

    Point m_origin = ORIGIN;

public:

    Vector( ) { }

    Vector( Planc cref in_dx, Planc cref in_dy ) : m_dx( in_dx ), m_dy( in_dy ) { }
    Vector( Planc cref in_dx, Planc cref in_dy, Point cref in_origin ) : m_dx( in_dx ), m_dy( in_dy ), m_origin( in_origin ) { }

    Vector( Point cref in_pt ) : m_dx( in_pt.x( ) ), m_dy( in_pt.y( ) ) { }

    Vector( Point cref in_start, Point cref in_end ) : m_dx( in_end.x( ) - in_start.x( ) ), m_dy( in_end.y( ) - in_start.y( ) ), m_origin( in_start ) { }

    Planc dx( ) const { return( m_dx ); }
    Vector ref dx( Planc cref in_dx ) { m_dx = in_dx; rethis; }

    Planc dy( ) const { return( m_dy ); }
    Vector ref dy( Planc cref in_dy ) { m_dy = in_dy; rethis; }

    Point origin( ) const { return( m_origin ); }
    Vector ref origin( Point cref in_origin ) { m_origin = in_origin; rethis; }

    Point destination( ) const { return( Point( origin( ).x( ) + dx( ), origin( ).y( ) + dy( ) ) ); }
    Vector ref destination( Point in_pt ) { dx( in_pt.x( ) - origin( ).x( ) ); dy( in_pt.y( ) - origin( ).y( ) ); rethis; }

    Planc magnitude( ) const { return( origin( ).distance_to( destination( ) ) ); }
    Vector ref magnitude( Planc cref in_m ) { Planc p = ( in_m / magnitude( ) ); dx( p * dx( ) ), dy( p * dy( ) );  rethis; }

    bool has_magnitude( ) const { return( dx( ) || dy( ) ); }

    Angle angle( ) const { return( Angle( m_dx, m_dy ) ); }

    Vector ref normalize( ) { return( magnitude( 1.0 ) ); }
    Vector normalized( ) const { return( Vector( dx( ) / magnitude( ), dy( ) / magnitude( ) ) ); }

    Vector ref rotate( Angle cref in_a ) { return( rotate_to( angle( ) + in_a ) ); }
    Vector ref rotate_right( bool in_ccw ) { return( rotate_to( angle( ) + ( in_ccw ? RIGHT : -RIGHT ) ) ); }
    Vector ref rotate_to( Angle cref in_a ) { if( angle( ) != in_a ) { Planc m = magnitude( ); dx( m * in_a.cos( ) ); dy( m * in_a.sin( ) ); } rethis; }

    Vector rotated( Angle cref in_a ) const { return( Vector( *this ).rotate( in_a ) ); }
    Vector rotated_right( bool in_ccw ) const { return( Vector( *this ).rotate( in_ccw ? RIGHT : -RIGHT ) ); }
    Vector rotated_to( Angle cref in_a ) const { return( Vector( *this ).rotate_to( in_a ) ); }

    Vector ref flip( ) { return( rotate( PI ) ); }
    Vector flipped( ) const { return( Vector( *this ).flip( ) ); }

    Vector ref extend( Planc cref in_p ) { return( magnitude( magnitude( ) + in_p ) ); }
    Vector extended( Planc cref in_p ) const { return( Vector( *this ).extend( in_p ) ); }

    Vector ref flatten( Angle cref in_a ) { rotate( -in_a ); dy( 0.0 ); rotate( in_a ); rethis; }
    Vector flattened( Angle cref in_a ) const { return( Vector( *this ).flatten( in_a ) ); }

    Vector ref operator=( Point cref in_pt ) { origin( ORIGIN ); dx( in_pt.x( ) ); dy( in_pt.y( ) ); rethis; }

    Vector ref operator+=( Vector cref in_vec ) { dx( dx( ) + in_vec.dx( ) ); dy( dy( ) + in_vec.dy( ) ); rethis; }
    Vector ref operator-=( Vector cref in_vec ) { dx( dx( ) - in_vec.dx( ) ); dy( dy( ) - in_vec.dy( ) ); rethis; }

    Vector ref operator*=( dec in_scale ) { dx( dx( ) * in_scale ); dy( dy( ) * in_scale ); rethis; }
    Vector ref operator/=( dec in_scale ) { dx( dx( ) / in_scale ); dy( dy( ) / in_scale ); rethis; }

    Vector operator+( Vector cref in_vec ) const { return( Vector( dx( ) + in_vec.dx( ), dy( ) + in_vec.dy( ), origin( ) ) ); }
    Vector operator-( Vector cref in_vec ) const { return( Vector( dx( ) - in_vec.dx( ), dy( ) - in_vec.dy( ), origin( ) ) ); }

    Vector operator*( dec in_scale ) const { return( Vector( origin( ), Point( origin( ).x( ) + ( dx( ) * in_scale ), origin( ).y( ) + ( dy( ) * in_scale ) ) ) ); }
    Vector operator/( dec in_scale ) const { return( Vector( origin( ), Point( origin( ).x( ) + ( dx( ) / in_scale ), origin( ).y( ) + ( dy( ) / in_scale ) ) ) ); }

    Vector operator+( ) const { return( Vector( +dx( ), +dy( ), origin( ) ) ); }
    Vector operator-( ) const { return( Vector( -dx( ), -dy( ), origin( ) ) ); }

    default_equal( Vector );

};

var_const( Vector V0( 0.0, 0.0 ) );
var_const( Vector XHAT( 1.0, 0.0 ) );
var_const( Vector YHAT( 0.0, 1.0 ) );

inline Vector VectorX( Planc cref in_dx ) { return( Vector( in_dx, P0 ) ); }
inline Vector VectorX( Planc cref in_dx, Point cref in_origin ) { return( Vector( in_dx, P0, in_origin ) ); }

inline Vector VectorY( Planc cref in_dy ) { return( Vector( P0, in_dy ) ); }
inline Vector VectorY( Planc cref in_dy, Point cref in_origin ) { return( Vector( P0, in_dy, in_origin ) ); }

inline Vector VectorA( Angle cref in_a ) { return( Vector( in_a.cos( ), in_a.sin( ) ) ); }
inline Vector VectorA( Angle cref in_a, Point cref in_origin ) { return( Vector( in_a.cos( ), in_a.sin( ), in_origin ) ); }

inline Vector VectorA( Angle cref in_a, Planc cref in_magnitude ) { return( Vector( in_a.cos( ) * in_magnitude, in_a.sin( ) * in_magnitude ) ); }
inline Vector VectorA( Angle cref in_a, Planc cref in_magnitude, Point cref in_origin ) { return( Vector( in_a.cos( ) * in_magnitude, in_a.sin( ) * in_magnitude, in_origin ) ); }

inline Point::Point( Vector cref in_vec ) { *this = in_vec.destination( ); }

inline Point ref Point::move( Planc cref in_x, Planc cref in_y ) { Point::x( Point::x( ) + in_x ); Point::y( Point::y( ) + in_y ); rethis; }
inline Point ref Point::rotate( Angle cref in_a, Point cref in_pt ) { rethis = Vector( in_pt, *this ).rotate( in_a ).destination( ); }
inline Point ref Point::mirror( Vector cref in_vec ) { x( x( ) - in_vec.origin( ).x( ) ); y( y( ) - in_vec.origin( ).y( ) ); rotate( -in_vec.angle( ) ); y( -y( ) ); rotate( in_vec.angle( ) ); x( x( ) + in_vec.origin( ).x( ) ); y( y( ) + in_vec.origin( ).y( ) ); rethis; }
inline Point ref Point::mirror_x( ) { return( mirror( XHAT ) ); }
inline Point ref Point::mirror_y( ) { return( mirror( YHAT ) ); }

inline Point ref Point::operator+=( Vector cref in_vec ) { x( x( ) + in_vec.dx( ) ); y( y( ) + in_vec.dy( ) ); rethis; }
inline Point ref Point::operator-=( Vector cref in_vec ) { x( x( ) - in_vec.dx( ) ); y( y( ) - in_vec.dy( ) ); rethis; }

inline Point Point::operator+( Vector cref in_vec ) const { return( Point( x( ) + in_vec.dx( ), y( ) + in_vec.dy( ) ) ); }
inline Point Point::operator-( Vector cref in_vec ) const { return( Point( x( ) - in_vec.dx( ), y( ) - in_vec.dy( ) ) ); }

} // namespace geometry
} // namespace axn

#endif /* Vector_hpp */
