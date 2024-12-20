#ifndef Path_hpp
#define Path_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"
#include "Vector.hpp"
#include "Transform.hpp"
#include "Line.hpp"

namespace axn
{
namespace geometry
{

class Path : public Transformable
{
private:
    varray<Line> m_lines;

public:
    Path( ) { }

    Path( const varray<Coordinate> & coordinates, bool loop = false )
    {
        if( coordinates.size( ) == ONE )
        {
            m_lines = { Line( coordinates.front( ), coordinates.front( ) ) };
        }
        else if( coordinates.size( ) > ONE )
        {
            for_range( i, coordinates.size( ) - ONE )
            {
                m_lines.insert_back( Line( coordinates[ i ], coordinates[ i + 1 ] ) );
            }

            if( loop && ( coordinates.size( ) >= 3 ) )
            {
                m_lines.insert_back( Line( coordinates.back( ), coordinates.front( ) ) );
            }
        }
    }

    Path( const varray<Line> & lines ) { m_lines = lines; for_range( i, m_lines.size( ) - ONE ) { Assert( m_lines[ i ].c2( ) == m_lines[ i + ONE ].c1( ) ); } }

    Path( const Line & line ) { m_lines = { line }; }

    operator const varray<Line> & ( ) { return m_lines; }

    Path & transform( const Transform & _transform ) { Transformable::transform( _transform ); for_each( line, m_lines ) { line.transform( _transform ); } rethis; }

    uint line_count( ) const { return m_lines.size( ); }
    uint point_count( ) const { return m_lines.size( ) ? m_lines.size( ) + ONE : ZERO; }

    const varray<Line> & lines( ) const { return m_lines; }
    varray<Coordinate> points( ) const { return_if( { }, !line_count( ) );        varray<Coordinate> points;        for_each( line, m_lines ) { points.insert_back( line.c1( ) ); } points.insert_back( m_lines.back( ).c2( ) ); return points; }

    bool has_length( ) const { for_each( line, m_lines ) { return_if( true, line.length( ) ); } return false; }
    Planc length( ) const { Planc length = 0; for_each( line, m_lines ) { length += line.length( ); } return length; }

    bool on( const Coordinate & coordinate ) const { for_each( line, m_lines ) { return_if( true, line.on( coordinate ) ); } return false; }
    bool above( const Coordinate & coordinate, bool inclusive = false ) const { for_each( line, m_lines ) { return_if( true, line.above( coordinate, inclusive ) ); } return false; }
    bool below( const Coordinate & coordinate, bool inclusive = false ) const { for_each( line, m_lines ) { return_if( true, line.below( coordinate, inclusive ) ); }        return false; }

    bool in_bounds( const Coordinate & coordinate, bool inclusive = false ) const
    {
        Planc min_x = INFINITY_POS;
        Planc max_x = INFINITY_NEG;
        Planc min_y = INFINITY_POS;
        Planc max_y = INFINITY_NEG;

        if( m_lines.size( ) )
        {
            min_x = min<Planc>( min_x, m_lines.front( ).c1( ).x( ) );
            max_x = max<Planc>( max_x, m_lines.front( ).c1( ).x( ) );
            min_y = min<Planc>( min_y, m_lines.front( ).c1( ).y( ) );
            max_y = max<Planc>( max_y, m_lines.front( ).c1( ).y( ) );

            for_each( line, m_lines )
            {
                min_x = min<Planc>( min_x, line.c2( ).x( ) );
                max_x = max<Planc>( max_x, line.c2( ).x( ) );
                min_y = min<Planc>( min_y, line.c2( ).y( ) );
                max_y = max<Planc>( max_y, line.c2( ).y( ) );
            }

            return ( in_range( coordinate.x( ), min_x, max_x, inclusive ) ) &&
                ( in_range( coordinate.y( ), min_y, max_y, inclusive ) );
        }

        return false;
    }

    bool intersects( const Line & line, bool inclusive = false ) const { for_each( line, m_lines ) { return_if( true, line.intersects( line, inclusive ) ); } return false; }
    bool intersects_self( ) const { for_range( i, line_count( ) - 1 ) { for_range( j, line_count( ) - i ) { return_if( true, m_lines[ i ].intersects( m_lines[ i + j + 1 ] ) ); } } return false; } default_equal( Path );
};

class Arc : public Path
{
private:
    Arc( const Planc & radius, const Angle & start, const Angle & end, bool clockwise ) : Path( generate( ORIGIN, radius, start, end, clockwise ) ) { }
    Arc( const Coordinate & center, const Planc & radius, const Angle & start, const Angle & end, bool clockwise ) : Path( generate( center, radius, start, end, clockwise ) ) { }

public:
    static Arc cw( const Coordinate & center, const Planc & radius, const Angle & start, const Angle & end ) { return Arc( center, radius, start, end, true ); }
    static Arc ccw( const Coordinate & center, const Planc & radius, const Angle & start, const Angle & end ) { return Arc( center, radius, start, end, false ); }

    static Arc cw( const Planc & radius, const Angle & start, const Angle & end ) { return Arc( ORIGIN, radius, start, end, true ); }
    static Arc ccw( const Planc & radius, const Angle & start, const Angle & end ) { return Arc( ORIGIN, radius, start, end, false ); }

private:
    static varray<Line> generate( const Coordinate & center, const Planc & radius, const Angle & start, const Angle & end, bool clockwise )
    {
        return_if( { }, ( start == end ) );

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
        uint line_count = max( ONE, ceil( ( d_angle.radians( ) / TAU ) * 60 ) );
        Angle dd_angle = d_angle / (dec)line_count;

        Coordinate start_coordinate = center + Vector::A( start_angle, radius );

        for_range( i, line_count - ONE )
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

class Bezier : public Path
{
public:
    Bezier( const varray<Coordinate> & control_points, uint point_count ) : Path( generate( control_points, point_count ) ) { }
    Bezier( const varray<Coordinate> & control_points ) : Path( generate( control_points, control_points.size( ) ) ) { }

private:
    static varray<Line> generate( const varray<Coordinate> & control_points, uint point_count )
    {
        uint control_point_count = control_points.size( );

        if( !control_point_count )
        {
            return { };
        }
        else if( control_point_count == ONE )
        {
            return { Line( control_points.front( ), control_points.front( ) ) };
        }
        else if( control_point_count == TWO )
        {
            return { Line( control_points.front( ), control_points.back( ) ) };
        }

        varray<Line> lines;
        Coordinate last = control_points.front( );

        dec dt = ONE / point_count;

        for_range( x, point_count - ONE )
        {
            dec t = dt * ( x + ONE );

            Vector v;
            varray<uint> p = pascal( control_point_count - ONE );
            for_range( i, control_point_count )
            {
                v += (Vector)( control_points[ i ] ) * (dec)p[ i ] * pow( t, i ) * pow( ( ONE - t ), control_point_count - i - ONE );
            }

            lines.insert_back( Line( last, v ) );
            last = v;
        }

        lines.insert_back( Line( last, control_points.back( ) ) );

        return lines;
    }
};

} // namespace geometry
} // namespace axn

#endif /* Path_hpp */
