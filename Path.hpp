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
public:
    virtual ~Path( ) { }

    Path( const varray<Coordinate> & coordinates = { } );
    Path( const varray<Line> & lines );
    Path( const Line & line ) { *this = Path( { line } ); }

    const varray<Line> & lines( ) const { return m_lines; }

    varray<Coordinate> points( ) const;

    uint line_count( ) const { return m_lines.size( ); }
    uint point_count( ) const { return m_lines.size( ) ? m_lines.size( ) + ONE : ZERO; }

    virtual Path & transform( const Transform & t );

    Planc length( ) const;
    bool has_length( ) const;

    bool on( const Coordinate & ) const;
    bool above( const Coordinate &, bool inclusive = false ) const;
    bool below( const Coordinate &, bool inclusive = false ) const;
    bool in_box( const Coordinate &, bool inclusive = true ) const;
    bool intersects( const Line &, bool inclusive = true ) const;

    bool intersects_self( ) const;

    default_equal( Path );

    operator varray<Line>( ) { return m_lines; }

private:
    varray<Line> m_lines;
};

class Arc : public Path
{
private:
    Arc( const Coordinate & center, const Planc & radius, const Angle & start, const Angle & end, bool clockwise );

public:
    virtual ~Arc( ) { }

    static Arc cw( const Coordinate & center, const Planc & radius, const Angle & start, const Angle & end ) { return Arc( center, radius, start, end, true ); }
    static Arc ccw( const Coordinate & center, const Planc & radius, const Angle & start, const Angle & end ) { return Arc( center, radius, start, end, false ); }

    static Arc cw( const Planc & radius, const Angle & start, const Angle & end ) { return Arc( ORIGIN, radius, start, end, true ); }
    static Arc ccw( const Planc & radius, const Angle & start, const Angle & end ) { return Arc( ORIGIN, radius, start, end, false ); }

private:
    static varray<Line> generate( const Coordinate & center, const Planc & radius, const Angle & start, const Angle & end, bool clockwise );
};

class Bezier : public Path
{
public:
    virtual ~Bezier( ) { }
    Bezier( const varray<Coordinate> & control_points, uint point_count );
    Bezier( const varray<Coordinate> & control_points ) { *this = Bezier( control_points, (uint)( control_points.size( ) ) ); }

private:
    static varray<Line> generate( const varray<Coordinate> & control_points, uint _point_count );
};

} // namespace geometry
} // namespace axn

#endif /* Path_hpp */
