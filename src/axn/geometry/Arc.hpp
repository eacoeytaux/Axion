#ifndef Arc_hpp
#define Arc_hpp

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

#if defined( AXN_DEBUG )
var_const( uint CURVE_MAX_COUNT = ( 120 ) );
var_const( Planc CURVE_RATIO = ( 0.5 ) );
#else
var_const( uint CURVE_MAX_COUNT = ( 120 ) );
var_const( Planc CURVE_RATIO = ( 1.0 ) );
#endif

inline uint curve_point_count( Planc cref in_radius, uint in_min_count = 0 ) { return( min<uint>( max<uint>( max<uint>( 8, in_min_count ), uint( in_radius * numbers::TAU * CURVE_RATIO ) + 1 ), CURVE_MAX_COUNT ) ); }

class Arc
{

private:

    Planc m_radius = P0;

    Point m_center = ORIGIN;

    Angle m_start = A0;
    Angle m_end = A0;

    bool m_ccw = true;

    Arc( Planc cref in_radius, Point cref in_center, Angle cref in_start, Angle cref in_end, bool in_ccw ) : m_radius( in_radius ), m_center( in_center ), m_start( in_start ), m_end( in_end ), m_ccw( in_ccw ) { }

public:

    static Arc ccw( Planc cref in_radius, Point cref in_center, Angle cref in_start, Angle cref in_end ) { return( Arc( in_radius, in_center, in_start, in_end, true ) ); }
    static Arc cw( Planc cref in_radius, Point cref in_center, Angle cref in_start, Angle cref in_end ) { return( Arc( in_radius, in_center, in_end, in_start, false ) ); }

    static Arc ccw( Planc cref in_radius, Angle cref in_start, Angle cref in_end ) { return( Arc( in_radius, ORIGIN, in_start, in_end, true ) ); }
    static Arc cw( Planc cref in_radius, Angle cref in_start, Angle cref in_end ) { return( Arc( in_radius, ORIGIN, in_end, in_start, false ) ); }

    static Arc semi_ccw( Planc cref in_radius, Point cref in_center, Angle cref in_start = A0 ) { return( Arc( in_radius, in_center, in_start, in_start + PI, true ) ); }
    static Arc semi_cw( Planc cref in_radius, Point cref in_center, Angle cref in_start = A0 ) { return( Arc( in_radius, in_center, in_start - PI, in_start, false ) ); }

    static Arc semi_ccw( Planc cref in_radius, Angle cref in_start = A0 ) { return( Arc( in_radius, ORIGIN, in_start, in_start + PI, true ) ); }
    static Arc semi_cw( Planc cref in_radius, Angle cref in_start = A0 ) { return( Arc( in_radius, ORIGIN, in_start - PI, in_start, false ) ); }

    Planc radius( ) const { return( m_radius ); }

    Point center( ) const { return( m_center ); }

    Angle start( ) const { return( m_start ); }
    Angle end( ) const { return( m_end ); }

    bool ccw( ) const { return( m_ccw ); }

    dec percentage( ) const
    {
        Angle start_angle = start( ).truncated( );
        Angle end_angle = end( ).truncated( );

        return_if( ( start_angle == end_angle ), 0.0 );

        Angle d_angle;
        if( start_angle > end_angle )
        {
            d_angle = end_angle + ( TAU - start_angle );
        }
        else
        {
            d_angle = end_angle - start_angle;
        }

        return( d_angle.radians( ) / TAU );
    }

    Path path( ) { return( path( curve_point_count( radius( ) ) * percentage( ) ) ); }
    Path path( uint in_line_count )
    {
        Angle start_angle = start( ).truncated( );
        Angle end_angle = end( ).truncated( );

        return_if( ( start_angle == end_angle ), Path( ) );

        Angle d_angle;
        if( start_angle > end_angle )
        {
            d_angle = end_angle + ( TAU - start_angle );
        }
        else
        {
            d_angle = end_angle - start_angle;
        }

        Angle dd_angle = d_angle / dec( in_line_count );
        Point start_pt = center( ) + VectorA( start_angle, radius( ) );

        varray<Line> lines;

        for_range( i, in_line_count )
        {
            Point pt = m_center + VectorA( start_angle + ( dd_angle * dec( i ) ), radius( ) );
            lines.insert_back( Line( start_pt, pt ) );
            start_pt = pt;
        }

        Point end_pt = center( ) + VectorA( end_angle, radius( ) );
        lines.insert_back( Line( start_pt, end_pt ) );

        if( ccw( ) )
        {
            return( lines );
        }
        else
        {
            for_each( line, lines )
            {
                line = line.flipped( );
            }

            return( lines.reversed( ) );
        }
    }

    varray<Point> points( ) { return( path( ).points( ) ); }
    varray<Point> points( uint in_line_count ) { return( path( in_line_count ).points( ) ); }

};

} // namespace geometry
} // namespace axn

#endif /* Arc_hpp */
