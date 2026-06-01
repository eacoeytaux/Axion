#ifndef Bezier_hpp
#define Bezier_hpp

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

class Bezier
{

private:

    varray<Point> m_control_points;

public:

    Bezier( ) { }

    Bezier( varray<Point> cref in_control_points ) : m_control_points( in_control_points ) { for_each( point, in_control_points ) { } }

    varray<Point> cref control_points( ) const { return( m_control_points ); }

    Point position( dec in_perc )
    {
        Assert( in_range_0_to_1( in_perc, true ) );

        Vector v = V0;

        if( is_pos( control_points( ).size( ) ) )
        {
            varray<dec> p = pascal_triangle( control_points( ).size( ) - 1 );

            for_range( i, control_points( ).size( ) )
            {
                v += Vector( control_points( )[ i ] ) * p[ i ] * pow( in_perc, i ) * pow( one_minus( in_perc ), control_points( ).size( ) - i - 1 );
            }
        }

        return( v );
    }

    Path path( )
    {
        Planc length = P0;

        if( is_pos( control_points( ).size( ) ) )
        {
            for_range( i, control_points( ).size( ) - 1 )
            {
                length += distance( control_points( )[ i ], control_points( )[ i + 1 ] );
            }
        }

        return( path( min<uint>( 2, ceil( length ) ) ) );
    }

    Path path( uint in_point_count )
    {
        uint control_point_count = m_control_points.size( );

        if( is_pos( control_point_count ) )
        {
            if( control_point_count <= 2 )
            {
                return{ Line( control_points( ).front( ), control_points( ).back( ) ) };
            }
            else
            {
                dec dt = inverse( dec( in_point_count ) );

                Point last = control_points( ).front( );

                varray<Line> lines;

                for_range( i, in_point_count - 1 )
                {
                    Point pt = position( dt * ( i + 1.0 ) );
                    lines.insert_back( Line( last, pt ) );
                    last = pt;
                }

                lines.insert_back( Line( last, control_points( ).back( ) ) );

                return( lines );
            }
        }

        return{ };
    }

};


} // namespace geometry
} // namespace axn

#endif /* Bezier_hpp */
