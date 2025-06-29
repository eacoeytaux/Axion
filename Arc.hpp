#ifndef Arc_hpp
#define Arc_hpp

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

axnclass( Arc )
{

private:

    Planc m_radius = P0;

    Coordinate m_center = ORIGIN;

    Angle m_start = A0;
    Angle m_end = A0;

    bool m_ccw = true;

    Arc( ) { }

    Arc( Planc cref radius, Coordinate cref center, Angle cref start, Angle cref end, bool ccw ) :
        m_radius( radius ),
        m_center( center ),
        m_start( start ),
        m_end( end ),
        m_ccw( ccw )
    {
        // Assert( center.valid( ) );
    }

public:

    static Arc ccw( Planc cref radius, Coordinate cref center, Angle cref start, Angle cref end ) { return Arc( radius, center, start, end, true ); }
    static Arc cw( Planc cref radius, Coordinate cref center, Angle cref start, Angle cref end ) { return Arc( radius, center, end, start, false ); }

    static Arc ccw( Planc cref radius, Angle cref start, Angle cref end ) { return Arc( radius, ORIGIN, start, end, true ); }
    static Arc cw( Planc cref radius, Angle cref start, Angle cref end ) { return Arc( radius, ORIGIN, end, start, false ); }

    static Arc semi_ccw( Planc cref radius, Coordinate cref center, Angle cref start = A0 ) { return Arc( radius, center, start, start + PI, true ); }
    static Arc semi_cw( Planc cref radius, Coordinate cref center, Angle cref start = A0 ) { return Arc( radius, center, start - PI, start, false ); }

    static Arc semi_ccw( Planc cref radius, Angle cref start = A0 ) { return Arc( radius, ORIGIN, start, start + PI, true ); }
    static Arc semi_cw( Planc cref radius, Angle cref start = A0 ) { return Arc( radius, ORIGIN, start - PI, start, false ); }

    Planc radius( ) const { return m_radius; }

    const Coordinate & center( ) const { return m_center; }

    Angle start( ) const { return m_start; }
    Angle end( ) const { return m_end; }

    Path path( ) { return path( curve_point_count( half( radius( ) * TAU ) ) ); } // todo should point count be scaled by d_angle?
    Path path( uint line_count )
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

        Angle dd_angle = d_angle / (dec)line_count;
        Coordinate start_coordinate = center( ) + VectorA( start_angle, radius( ) );

        varray<Line> lines;

        for_range( i, line_count )
        {
            Coordinate c = m_center + VectorA( start_angle + ( dd_angle * (dec)i ), radius( ) );
            lines.insert_back( Line( start_coordinate, c ) );
            start_coordinate = c;
        }

        Coordinate end_coordinate = center( ) + VectorA( end_angle, radius( ) );
        lines.insert_back( Line( start_coordinate, end_coordinate ) );

        return ( m_ccw ? lines : lines.reversed( ) );
    }

};

} // namespace geometry
} // namespace axn

#endif /* Arc_hpp */
