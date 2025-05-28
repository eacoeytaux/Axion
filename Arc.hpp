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

    Angle m_start;
    Angle m_end;
    Planc m_radius;
    Coordinate m_center;

    Arc( ) { }

    Arc( Coordinate cref center, Planc cref radius, Angle cref start, Angle cref end ) :
        m_center( center ), m_radius( radius ), m_start( start ), m_end( end )
    { Assert( center.valid( ) ); }

public:

    static Arc cw( Coordinate cref center, Planc cref radius, Angle cref start, Angle cref end ) { return Arc( center, radius, end, start ); }
    static Arc ccw( Coordinate cref center, Planc cref radius, Angle cref start, Angle cref end ) { return Arc( center, radius, start, end ); }

    static Arc cw( Planc cref radius, Angle cref start, Angle cref end ) { return Arc( ORIGIN, radius, end, start ); }
    static Arc ccw( Planc cref radius, Angle cref start, Angle cref end ) { return Arc( ORIGIN, radius, start, end ); }
    
    Planc radius( ) const { return m_radius; }
    
    const Coordinate & center( ) const { return m_center; }
    
    Angle start( ) const { return m_start; }
    Angle end( ) const { return m_end; }

    Path path( ) { return path( curve_point_count( half( radius( ) * TAU ) ) ); }
    Path path( uint line_count )
    {
        Angle start_angle = start( );
        Angle end_angle = end( );

        start_angle.truncate( true );
        end_angle.truncate( true );
        
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

        return lines;
    }
    
};

} // namespace geometry
} // namespace axn

#endif /* Arc_hpp */
