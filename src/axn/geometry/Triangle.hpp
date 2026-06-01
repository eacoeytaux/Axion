#ifndef Triangle_hpp
#define Triangle_hpp

#include "axn/geometry/Planc.hpp"
#include "axn/geometry/Point.hpp"
#include "axn/geometry/Angle.hpp"
#include "axn/geometry/Vector.hpp"
#include "axn/geometry/Line.hpp"
#include "axn/geometry/Path.hpp"
#include "axn/geometry/Transform.hpp"

namespace axn
{
namespace geometry
{

class Triangle : public Transformable
{

private:

    Point m_pt1 = ORIGIN;
    Point m_pt2 = ORIGIN;
    Point m_pt3 = ORIGIN;

public:

    Triangle( ) { }

    Triangle( Point cref in_pt1, Point cref in_pt2, Point cref in_pt3 ) : m_pt1( in_pt1 ), m_pt2( in_pt2 ), m_pt3( in_pt3 ) { }

    Point cref pt1( ) const { return( m_pt1 ); }
    Point cref pt2( ) const { return( m_pt2 ); }
    Point cref pt3( ) const { return( m_pt3 ); }

    Planc cref lower_bound_x( ) const { return( min( min( pt1( ).x( ), pt2( ).x( ) ), pt3( ).x( ) ) ); }
    Planc cref lower_bound_y( ) const { return( min( min( pt1( ).y( ), pt2( ).y( ) ), pt3( ).y( ) ) ); }
    Planc cref upper_bound_x( ) const { return( max( max( pt1( ).x( ), pt2( ).x( ) ), pt3( ).x( ) ) ); }
    Planc cref upper_bound_y( ) const { return( max( max( pt1( ).y( ), pt2( ).y( ) ), pt3( ).y( ) ) ); }

    Planc bound_width( ) const { return( upper_bound_x( ) - lower_bound_x( ) ); }
    Planc bound_height( ) const { return( upper_bound_y( ) - lower_bound_y( ) ); }

    Planc area( ) const { return( abs( half( ( pt1( ).x( ) * ( pt2( ).y( ) - pt3( ).y( ) ) ) + ( pt2( ).x( ) * ( pt3( ).y( ) - pt1( ).y( ) ) ) + ( pt3( ).x( ) * ( pt1( ).y( ) - pt2( ).y( ) ) ) ) ) ); }

    Point centroid( ) const { return( Point( ( ( pt1( ).x( ) + pt2( ).x( ) + pt3( ).x( ) ) / 3.0 ), ( ( pt1( ).y( ) + pt2( ).y( ) + pt3( ).y( ) ) / 3.0 ) ) ); }

    Point circumcenter( ) const
    {
        dec d12 = distance( pt1( ), pt2( ) );
        dec d13 = distance( pt1( ), pt3( ) );
        dec d23 = distance( pt2( ), pt3( ) );

        dec s1 = sin( x2( acos( ( square( d12 ) + square( d13 ) - square( d23 ) ) / x2( d12 * d13 ) ) ) );
        dec s2 = sin( x2( acos( ( square( d12 ) + square( d23 ) - square( d13 ) ) / x2( d12 * d23 ) ) ) );
        dec s3 = sin( x2( acos( ( square( d13 ) + square( d23 ) - square( d12 ) ) / x2( d13 * d23 ) ) ) );

        return( Point( ( ( s1 * pt1( ).x( ) ) + ( s2 * pt2( ).x( ) ) + ( s3 * pt3( ).x( ) ) ),
                       ( ( s1 * pt1( ).y( ) ) + ( s2 * pt2( ).y( ) ) + ( s3 * pt3( ).y( ) ) ) ) / ( s1 + s2 + s3 ) );
    }

    bool contains( Point cref in_pt, bool in_inclusive = true ) const
    {
        dec a = 0.5 / half( ( pt1( ).x( ) * ( pt2( ).y( ) - pt3( ).y( ) ) ) + ( pt1( ).y( ) * ( pt3( ).x( ) - pt2( ).x( ) ) ) + ( pt2( ).x( ) * pt3( ).y( ) ) + ( pt3( ).x( ) * -pt2( ).y( ) ) );

        dec s = a * ( ( pt1( ).y( ) * pt3( ).x( ) ) - ( pt1( ).x( ) * pt3( ).y( ) ) + ( ( pt3( ).y( ) - pt1( ).y( ) ) * in_pt.x( ) ) + ( ( pt1( ).x( ) - pt3( ).x( ) ) * in_pt.y( ) ) );
        return_false_if( less( !in_inclusive, s, 0.0 ) );

        dec t = a * ( ( pt1( ).x( ) * pt2( ).y( ) ) - ( pt1( ).y( ) * pt2( ).x( ) ) + ( ( pt1( ).y( ) - pt2( ).y( ) ) * in_pt.x( ) ) + ( ( pt2( ).x( ) - pt1( ).x( ) ) * in_pt.y( ) ) );
        return_false_if( less( !in_inclusive, t, 0.0 ) );

        return( !less( !in_inclusive, one_minus( s + t ), 0.0 ) );
    }

    bool intersects( Line cref in_line ) const { return( ( contains( in_line.pt1( ) ) || contains( in_line.pt2( ) ) ) || ( in_line.intersects( Line( pt1( ), pt2( ) ) ) || in_line.intersects( Line( pt2( ), pt3( ) ) ) || in_line.intersects( Line( pt3( ), pt1( ) ) ) ) ); }
    Line intersection( Line cref in_line ) const
    {
        bool pt1c = contains( in_line.pt1( ) );
        bool pt2c = contains( in_line.pt2( ) );

        return_if( ( pt1c && pt2c ), in_line );

        Line l1( pt1( ), pt2( ) );
        Line l2( pt2( ), pt3( ) );
        Line l3( pt3( ), pt1( ) );

        bool l1i = l1.intersects( in_line );
        bool l2i = l2.intersects( in_line );

        return_if( pt1c, Line( in_line.pt1( ), l1i ? l1.intersection( in_line ) : ( l2i ? l2.intersection( in_line ) : l3.intersection( in_line ) ) ) );
        return_if( pt2c, Line( l1i ? l1.intersection( in_line ) : ( l2i ? l2.intersection( in_line ) : l3.intersection( in_line ) ), in_line.pt2( ) ) );

        Point pt1 = l1i ? l1.intersection( in_line ) : l2.intersection( in_line );
        Point pt2 = ( l1i && l2i ) ? l2.intersection( in_line ) : l3.intersection( in_line );

        return( ( in_line.pt1( ).closer_than( pt1, pt2 ) ) ? Line( pt1, pt2 ) : Line( pt2, pt1 ) );
    }

    virtual Triangle ref transform( Transform cref in_trans ) override { m_pt1 = in_trans.apply( m_pt1 ); m_pt2 = in_trans.apply( m_pt2 ); m_pt3 = in_trans.apply( m_pt3 ); rethis; }

    Triangle ref operator+=( Vector cref in_vec ) { rethis = ( *this + in_vec ); }
    Triangle ref operator-=( Vector cref in_vec ) { rethis = ( *this - in_vec ); }

    Triangle ref operator*=( Planc cref in_p ) { rethis = ( Triangle( pt1( ) * in_p, pt2( ) * in_p, pt3( ) * in_p ) ); }
    Triangle ref operator/=( Planc cref in_p ) { rethis = ( Triangle( pt1( ) / in_p, pt2( ) / in_p, pt3( ) / in_p ) ); }

    Triangle operator+( Vector cref in_vec ) const { return( Triangle( pt1( ) + in_vec, pt2( ) + in_vec, pt3( ) + in_vec ) ); }
    Triangle operator-( Vector cref in_vec ) const { return( Triangle( pt1( ) - in_vec, pt2( ) - in_vec, pt3( ) - in_vec ) ); }

    Triangle operator*( Planc cref in_p ) const { return( Triangle( pt1( ) * in_p, pt2( ) * in_p, pt3( ) * in_p ) ); }
    Triangle operator/( Planc cref in_p ) const { return( Triangle( pt1( ) / in_p, pt2( ) / in_p, pt3( ) / in_p ) ); }

    default_equal( Triangle );

};

} // namespace geometry
} // namespace axn

#endif /* Triangle_hpp */
