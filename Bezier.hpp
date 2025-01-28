#ifndef Bezier_hpp
#define Bezier_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"
#include "Vector.hpp"
#include "Transform.hpp"
#include "Line.hpp"
#include "Path.hpp"

namespace axn
{
namespace geometry
{

class Bezier
{

private:

    varray<Coordinate> m_control_points;

public:

    Bezier( ) { }

    Bezier( varray<Coordinate> cref control_points ) : m_control_points( control_points ) { }

    Coordinate position( dec t )
    {
        Assert( in_range( t, 0.0, 1.0, true ) );

        uint control_point_count = m_control_points.size( );

        Vector v;
        varray<uint> p = pascal( control_point_count - 1 );

        for_range( i, control_point_count )
        {
            v += (Vector)( m_control_points[ i ] ) * (dec)p[ i ] * pow( t, i ) * pow( ( 1.0 - t ), control_point_count - i - 1 );
        }

        return v;
    }

    Path path( ) { return path( m_control_points.size( ) + 1 ); }
    Path path( uint point_count )
    {
        uint control_point_count = m_control_points.size( );

        if( control_point_count )
        {
            if( control_point_count <= 2 )
            {
                return { Line( m_control_points.front( ), m_control_points.back( ) ) };
            }
            else
            {
                varray<Line> lines;
                Coordinate last = m_control_points.front( );

                dec dt = 1.0 / point_count;

                for_range( x, point_count - 1 )
                {
                    Coordinate c = position( dt * ( x + 1.0 ) );
                    lines.insert_back( Line( last, c ) );
                    last = c;
                }

                lines.insert_back( Line( last, m_control_points.back( ) ) );

                return lines;
            }
        }

        return { };
    }
    
};


} // namespace geometry
} // namespace axn

#endif /* Bezier_hpp */
