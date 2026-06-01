#ifndef Line_hpp
#define Line_hpp

#include "axn/geometry/Planc.hpp"
#include "axn/geometry/Point.hpp"
#include "axn/geometry/Angle.hpp"
#include "axn/geometry/Vector.hpp"
#include "axn/geometry/Transform.hpp"

namespace axn
{
namespace geometry
{

class Line : public Transformable
{

private:

    Point m_pt1 = ORIGIN;
    Point m_pt2 = ORIGIN;

    Planc m_m = INFINITY;
    Planc m_b = P0;

    Line ref solve_mb( ) { if( pt1( ).x( ) == pt2( ).x( ) ) { m_m = ( ( pt1( ).y( ) <= pt2( ).y( ) ) ? INFINITY : -INFINITY ); m_b = pt1( ).x( ); } else { m_m = ( ( pt2( ).y( ) - pt1( ).y( ) ) / ( pt2( ).x( ) - pt1( ).x( ) ) ); m_b = ( pt1( ).y( ) - ( m_m * pt1( ).x( ) ) ); } rethis; }

public:

    Line( ) { }

    Line( Point cref in_pt1, Point cref in_pt2 ) : m_pt1( in_pt1 ), m_pt2( in_pt2 ) { solve_mb( ); }

    Line( Vector cref in_vec ) : m_pt1( in_vec.origin( ) ), m_pt2( in_vec.destination( ) ) { solve_mb( ); }

    Point cref pt1( ) const { return( m_pt1 ); }
    Point cref pt2( ) const { return( m_pt2 ); }

    Line flipped( ) const { return( Line( pt2( ), pt1( ) ) ); }

    Angle angle( ) const { return( Angle( pt1( ), pt2( ) ) ); }
    Vector vector( ) const { return( Vector( pt1( ), pt2( ) ) ); }

    bool vertical( ) const { return( is_inf( m( ) ) ); }
    bool horizontal( ) const { return( is_zero( m( ) ) ); }

    Planc cref m( ) const { return( m_m ); }
    Planc cref b( ) const { return( m_b ); }

    Planc length( ) const { return( pt1( ).distance_to( pt2( ) ) ); }

    Planc cref lower_bound_x( ) const { return( min( pt1( ).x( ), pt2( ).x( ) ) ); }
    Planc cref lower_bound_y( ) const { return( min( pt1( ).y( ), pt2( ).y( ) ) ); }
    Planc cref upper_bound_x( ) const { return( max( pt1( ).x( ), pt2( ).x( ) ) ); }
    Planc cref upper_bound_y( ) const { return( max( pt1( ).y( ), pt2( ).y( ) ) ); }

    Planc x( Planc cref in_y ) const { return( ( vertical( ) || horizontal( ) ) ? pt1( ).x( ) : ( ( in_y - b( ) ) / m( ) ) ); }
    Planc y( Planc cref in_x ) const { return( ( vertical( ) || horizontal( ) ) ? pt1( ).y( ) : ( ( m( ) * in_x ) + b( ) ) ); }

    Planc cross( Point cref in_pt ) const { return( ( ( in_pt.y( ) - pt1( ).y( ) ) * ( pt2( ).x( ) - pt1( ).x( ) ) ) - ( ( in_pt.x( ) - pt1( ).x( ) ) * ( pt2( ).y( ) - pt1( ).y( ) ) ) ); }

    bool on( Point cref in_pt, bool in_bounded = true ) const { return( equal( cross( in_pt ), 0.0 ) && ( ( pt1( ) != pt2( ) ) || ( in_pt == pt1( ) ) ) && ( !in_bounded || in_bounds( in_pt ) ) ); }
    bool right( Point cref in_pt, bool in_inclusive = false ) const { return( less( in_inclusive, cross( in_pt ), 0.0 ) && ( pt1( ) != pt2( ) ) ); }
    bool left( Point cref in_pt, bool in_inclusive = false ) const { return( greater( in_inclusive, cross( in_pt ), 0.0 ) && ( pt1( ) != pt2( ) ) ); }

    bool in_bounds( Point cref in_pt, bool in_inclusive = true ) const { return( in_range( in_pt.x( ), lower_bound_x( ), upper_bound_x( ), in_inclusive ) && in_range( in_pt.y( ), lower_bound_y( ), upper_bound_y( ), in_inclusive ) ); }

    bool parallel( Line cref in_line ) const { return( ( ( m( ) == in_line.m( ) ) || ( is_inf( m( ) ) && is_inf( in_line.m( ) ) ) ) && ( ( pt1( ) != pt2( ) ) && ( in_line.pt1( ) != in_line.pt2( ) ) ) ); }
    bool colinear( Line cref in_line ) const { return( ( *this == in_line ) || ( parallel( in_line ) && ( b( ) == in_line.b( ) ) ) ); }

    bool intersects( Line cref in_line, bool inclusive = true ) const
    {
        if( parallel( in_line ) )
        {
            if( b( ) != in_line.b( ) )
            {
                return( false );
            }
            else
            {
                if( vertical( ) )
                {
                    return( in_range( in_line.pt1( ).y( ), lower_bound_y( ), upper_bound_y( ), inclusive ) ||
                            in_range( in_line.pt2( ).y( ), lower_bound_y( ), upper_bound_y( ), inclusive ) ||
                            in_range( pt1( ).y( ), in_line.lower_bound_y( ), in_line.upper_bound_y( ), inclusive ) ||
                            in_range( pt2( ).y( ), in_line.lower_bound_y( ), in_line.upper_bound_y( ), inclusive ) );
                }
                else
                {
                    return( in_range( in_line.pt1( ).x( ), lower_bound_x( ), upper_bound_x( ), inclusive ) ||
                            in_range( in_line.pt2( ).x( ), lower_bound_x( ), upper_bound_x( ), in_line.pt2( ).x( ), inclusive ) ||
                            in_range( pt1( ).x( ), in_line.lower_bound_x( ), in_line.upper_bound_x( ), inclusive ) ||
                            in_range( pt2( ).x( ), in_line.lower_bound_x( ), in_line.upper_bound_x( ), inclusive ) );
                }
            }
        }
        else
        {
            Point intersect;

            if( vertical( ) )
            {
                intersect = Point( lower_bound_x( ), ( in_line.horizontal( ) ? in_line.lower_bound_y( ) : ( lower_bound_x( ) * in_line.m( ) ) + in_line.b( ) ) );
            }
            else if( in_line.vertical( ) )
            {
                intersect = Point( in_line.lower_bound_x( ), ( horizontal( ) ? lower_bound_y( ) : ( in_line.lower_bound_x( ) * m( ) ) + b( ) ) );
            }
            else if( horizontal( ) )
            {
                intersect = Point( ( ( lower_bound_y( ) - in_line.b( ) ) / in_line.m( ) ), lower_bound_y( ) );
            }
            else if( in_line.horizontal( ) )
            {
                intersect = Point( ( ( in_line.lower_bound_y( ) - b( ) ) / m( ) ), in_line.lower_bound_y( ) );
            }
            else
            {
                Planc x = ( in_line.b( ) - b( ) ) / ( m( ) - in_line.m( ) );
                intersect = Point( x, ( x * m( ) ) + b( ) );
            }

            return( in_bounds( intersect, inclusive ) && in_line.in_bounds( intersect, inclusive ) );
        }
    }

    Point intersection( Line cref in_line ) const
    {
        if( parallel( in_line ) )
        {
            if( b( ) != in_line.b( ) )
            {
                return( INVALID_POINT );
            }
            else
            {
                return( pt1( ) );
            }
        }
        else
        {
            if( vertical( ) )
            {
                if( in_line.horizontal( ) )
                {
                    return( Point( pt1( ).x( ), in_line.pt1( ).y( ) ) );
                }
                else
                {
                    return( Point( pt1( ).x( ), in_line.y( pt1( ).x( ) ) ) );
                }
            }
            else if( horizontal( ) )
            {
                if( in_line.vertical( ) )
                {
                    return( Point( in_line.pt1( ).x( ), pt1( ).y( ) ) );
                }
                else
                {
                    return( Point( in_line.x( pt1( ).y( ) ), pt1( ).y( ) ) );
                }
            }
            else
            {
                if( in_line.vertical( ) )
                {
                    return( Point( in_line.pt1( ).x( ), y( in_line.pt1( ).x( ) ) ) );
                }
                else if( in_line.horizontal( ) )
                {
                    return( Point( x( in_line.pt1( ).y( ) ), in_line.pt1( ).y( ) ) );
                }
                else
                {
                    Planc x = ( in_line.b( ) - b( ) ) / ( m( ) - in_line.m( ) );

                    return( Point( x, in_line.y( x ) ) );
                }
            }
        }
    }

    Point closest( Point cref in_point, bool in_bounded = true ) const
    {
        return_if( on( in_point, in_bounded ), in_point );

        Point pt = intersection( Line( in_point, in_point + VectorA( angle( ) + RIGHT ) ) );

        return_if( !in_bounded, pt );
        return_if( in_bounds( pt, true ), pt );
        return( in_point.distance_to( pt1( ) ) < in_point.distance_to( pt2( ) ) ? pt1( ) : pt2( ) );
    }

    virtual Line ref transform( Transform cref in_t ) override { m_pt1 = in_t.apply( m_pt1 ); m_pt2 = in_t.apply( m_pt2 ); return( solve_mb( ) ); }

    Line ref operator+=( Vector cref in_vec ) { rethis = ( *this + in_vec ); }
    Line ref operator-=( Vector cref in_vec ) { rethis = ( *this - in_vec ); }

    Line operator+( Vector cref in_vec ) const { return( Line( pt1( ) + in_vec, pt2( ) + in_vec ) ); }
    Line operator-( Vector cref in_vec ) const { return( Line( pt1( ) - in_vec, pt2( ) - in_vec ) ); }

    default_equal( Line );

};

} // namespace geometry
} // namespace axn

#endif /* Line_hpp */
