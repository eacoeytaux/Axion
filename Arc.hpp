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

class Arc
{
private:
    Angle m_start;
    Angle m_end;
    Planc m_radius;
    Coordinate m_center;
    
    Arc( ) { }

    Arc( Coordinate cref center, Planc cref radius, Angle cref start, Angle cref end, bool clockwise ) :
        m_center( center ), m_radius( radius ), m_start( clockwise ? end : start ), m_end( clockwise ? start : end ) { }

public:
    static Arc cw( Coordinate cref center, Planc cref radius, Angle cref start, Angle cref end ) { return Arc( center, radius, start, end, true ); }
    static Arc ccw( Coordinate cref center, Planc cref radius, Angle cref start, Angle cref end ) { return Arc( center, radius, start, end, false ); }

    static Arc cw( Planc cref radius, Angle cref start, Angle cref end ) { return Arc( ORIGIN, radius, start, end, true ); }
    static Arc ccw( Planc cref radius, Angle cref start, Angle cref end ) { return Arc( ORIGIN, radius, start, end, false ); }

    Path path( ) { return path( 12 ); } // todo 12?
    Path path( uint line_count )
    {
        return_if( ( m_start == m_end ), Path( ) );

        varray<Line> lines;

        Angle d_angle;

        Angle start_angle = m_start;
        Angle end_angle = m_end;

        start_angle.truncate( true );
        end_angle.truncate( true );

        if( start_angle > end_angle )
        {
            d_angle = end_angle + ( TAU - start_angle );
        }
        else
        {
            d_angle = end_angle - start_angle;
        }

        Angle dd_angle = d_angle / (dec)line_count;
        Coordinate start_coordinate = m_center + Vector::A( start_angle, m_radius );

        for_range( i, line_count - 1 )
        {
            Coordinate c = m_center + Vector::A( start_angle + ( dd_angle * (dec)i ), m_radius );
            lines.insert_back( Line( start_coordinate, c ) );
            start_coordinate = c;
        }

        Coordinate end_coordinate = m_center + Vector::A( end_angle, m_radius );
        lines.insert_back( Line( start_coordinate, end_coordinate ) );

        return lines;
    }
};

} // namespace geometry
} // namespace axn

#endif /* Arc_hpp */
