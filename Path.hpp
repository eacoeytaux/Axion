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

    Path( varray<Coordinate> cref coordinates, bool loop = false )
    {
        if( coordinates.size( ) == 1 )
        {
            m_lines = { Line( coordinates.front( ), coordinates.front( ) ) };
        }
        else if( coordinates.size( ) > 1 )
        {
            for_range( i, coordinates.size( ) - 1 )
            {
                m_lines.insert_back( Line( coordinates[ i ], coordinates[ i + 1 ] ) );
            }

            if( loop && ( coordinates.size( ) >= 3 ) )
            {
                m_lines.insert_back( Line( coordinates.back( ), coordinates.front( ) ) );
            }
        }
    }

    Path( const varray<Line> & lines ) { m_lines = lines; for_range( i, m_lines.size( ) - 1 ) { Assert( m_lines[ i ].c2( ) == m_lines[ i + 1 ].c1( ) ); } }

    Path( Line cref line ) { m_lines = { line }; }

    operator const varray<Line> & ( ) { return m_lines; }

    Path & transform( Transform cref _transform ) { Transformable::transform( _transform ); for_each( line, m_lines ) { line.transform( _transform ); } rethis; }

    uint line_count( ) const { return m_lines.size( ); }
    uint point_count( ) const { return m_lines.size( ) ? m_lines.size( ) + 1 : 0; }

    const varray<Line> & lines( ) const { return m_lines; }
    varray<Coordinate> points( ) const { return_if( !line_count( ), { } ); varray<Coordinate> points; for_each( line, m_lines ) { points.insert_back( line.c1( ) ); } points.insert_back( m_lines.back( ).c2( ) ); return points; }

    bool has_length( ) const { for_each( line, m_lines ) { return_true_if( line.length( ) ); } return false; }
    Planc length( ) const { Planc length = 0; for_each( line, m_lines ) { length += line.length( ); } return length; }

    bool on( Coordinate cref coordinate ) const { for_each( line, m_lines ) { return_true_if( line.on( coordinate ) ); } return false; }
    bool above( Coordinate cref coordinate, bool inclusive = false ) const { for_each( line, m_lines ) { return_true_if( line.above( coordinate, inclusive ) ); } return false; }
    bool below( Coordinate cref coordinate, bool inclusive = false ) const { for_each( line, m_lines ) { return_true_if( line.below( coordinate, inclusive ) ); } return false; }

    bool in_bounds( Coordinate cref coordinate, bool inclusive = false ) const
    {
        if( m_lines.size( ) )
        {
            Planc min_x = m_lines.front( ).c1( ).x( );
            Planc max_x = m_lines.front( ).c1( ).x( );
            Planc min_y = m_lines.front( ).c1( ).y( );
            Planc max_y = m_lines.front( ).c1( ).y( );

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

    bool intersects( Line cref line, bool inclusive = false ) const { for_each( line, m_lines ) { return_true_if( line.intersects( line, inclusive ) ); } return false; }
    bool intersects_self( ) const { for_range( i, line_count( ) - 1 ) { for_range( j, line_count( ) - i ) { return_true_if( m_lines[ i ].intersects( m_lines[ i + j + 1 ] ) ); } } return false; } default_equal( Path );
};

} // namespace geometry
} // namespace axn

#endif /* Path_hpp */
