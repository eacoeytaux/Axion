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

class Arc
{
private:
    Arc( ) { }

    Arc( Planc cref radius, Angle cref start, Angle cref end, bool clockwise ) { }
    Arc( Coordinate cref center, Planc cref radius, Angle cref start, Angle cref end, bool clockwise ) { }

public:
    static Arc cw( Coordinate cref center, Planc cref radius, Angle cref start, Angle cref end ) { return Arc( center, radius, start, end, true ); }
    static Arc ccw( Coordinate cref center, Planc cref radius, Angle cref start, Angle cref end ) { return Arc( center, radius, start, end, false ); }

    static Arc cw( Planc cref radius, Angle cref start, Angle cref end ) { return Arc( ORIGIN, radius, start, end, true ); }
    static Arc ccw( Planc cref radius, Angle cref start, Angle cref end ) { return Arc( ORIGIN, radius, start, end, false ); }

private:
    static varray<Line> generate( Coordinate cref center, Planc cref radius, Angle cref start, Angle cref end, bool clockwise )
    {
        return_if( ( start == end ), { } );

        varray<Line> lines;

        Angle d_angle;

        Angle start_angle = start;
        Angle end_angle = end;

        start_angle.truncate( true );
        end_angle.truncate( true );

        if( clockwise )
        {
            if( start_angle < end_angle )
            {
                d_angle = start_angle + ( TAU - end_angle );
            }
            else
            {
                d_angle = start_angle - end_angle;
            }
        }
        else
        {
            if( start_angle > end_angle )
            {
                d_angle = end_angle + ( TAU - start_angle );
            }
            else
            {
                d_angle = end_angle - start_angle;
            }
        }

        // TODO 60?
        uint line_count = max<uint>( 1, ceil( ( d_angle.radians( ) / TAU ) * 60 ) );
        Angle dd_angle = d_angle / (dec)line_count;

        Coordinate start_coordinate = center + Vector::A( start_angle, radius );

        for_range( i, line_count - 1 )
        {
            Coordinate c = center + Vector::A( negate( start_angle + ( dd_angle * (dec)i ), clockwise ), radius );
            lines.insert_back( Line( start_coordinate, c ) );
            start_coordinate = c;
        }

        Coordinate end_coordinate = center + Vector::A( end_angle, radius );
        lines.insert_back( Line( start_coordinate, end_coordinate ) );

        return lines;
    }
};

} // namespace geometry
} // namespace axn

#endif /* Bezier_hpp */