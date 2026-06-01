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

axnclass( Path ) : public Transformable, public varray<Line>
{

public:

    Path( ) { }

    Path( varray<Coordinate> cref coordinates, bool loop = false )
    {
        if( coordinates.size( ) == 1 )
        {
            insert_back( Line( coordinates.front( ), coordinates.front( ) ) );
        }
        else if( coordinates.size( ) > 1 )
        {
            for_range( i, coordinates.size( ) - 1 )
            {
                insert_back( Line( coordinates[ i ], coordinates[ i + 1 ] ) );
            }

            if( loop && ( coordinates.size( ) >= 3 ) )
            {
                insert_back( Line( coordinates.back( ), coordinates.front( ) ) );
            }
        }
    }

    Path( varray<Line> cref lines ) : varray<Line>( lines ) { for_range( i, size( ) - 1 ) { Assert( at( i ).c2( ) == at( i + 1 ).c1( ) ); } }

    Path( Line cref line ) : varray<Line>( { line } ) { }

    Path loop( ) { if( size( ) && ( back( ).c2( ) != front( ).c1( ) ) ) { return Path( lines( ) + Line( back( ).c2( ), front( ).c1( ) ) ); } else { rethis; } }

    Path & transform( Transform cref _transform ) { Transformable::transform( _transform ); for_each( line, *this ) { line.transform( _transform ); } rethis; }

    uint line_count( ) const { return size( ); }
    uint point_count( ) const { return size( ) ? size( ) + 1 : 0; }

    varray<Line> cref lines( ) const { return ( *this ); }

    varray<Coordinate> points( ) const { return_if( !line_count( ), { } ); varray<Coordinate> points; for_each( line, *this ) { points.insert_back( line.c1( ) ); } points.insert_back( back( ).c2( ) ); return points; }

    bool has_length( ) const { for_each( line, *this ) { return_true_if( line.length( ) ); } return false; }
    Planc length( ) const { Planc length = 0; for_each( line, *this ) { length += line.length( ); } return length; }

    bool on( Coordinate cref coordinate ) const { for_each( line, *this ) { return_true_if( line.on( coordinate ) ); } return false; }
    bool above( Coordinate cref coordinate, bool inclusive = false ) const { for_each( line, *this ) { return_false_if( !line.above( coordinate, inclusive ) ); } return true; }
    bool below( Coordinate cref coordinate, bool inclusive = false ) const { for_each( line, *this ) { return_false_if( !line.below( coordinate, inclusive ) ); } return true; }

    bool in_bounds( Coordinate cref coordinate ) const
    {
        if( size( ) )
        {
            Planc min_x = front( ).c1( ).x( );
            Planc max_x = front( ).c1( ).x( );
            Planc min_y = front( ).c1( ).y( );
            Planc max_y = front( ).c1( ).y( );

            for_each( line, *this )
            {
                min_x = min<Planc>( min_x, line.c2( ).x( ) );
                max_x = max<Planc>( max_x, line.c2( ).x( ) );
                min_y = min<Planc>( min_y, line.c2( ).y( ) );
                max_y = max<Planc>( max_y, line.c2( ).y( ) );
            }

            return ( in_range( coordinate.x( ), min_x, max_x ) ) &&
                   ( in_range( coordinate.y( ), min_y, max_y ) );
        }

        return false;
    }

    bool intersects( Line cref line, bool inclusive = true ) const { for_range( i, line_count( ) ) { return_true_if( at( i ).intersects( line, inclusive ) ); } return false; }
    bool intersects_self( ) const { for_range( i, line_count( ) - 1 ) { for_range( j, line_count( ) - i - 1 ) { return_true_if( at( i ).intersects( at( i + j + 1 ), false ) && ( j || ( at( i ).intersection( at( i + 1 ) ) != at( i ).c2( ) ) ) ); } } return false; } // todo always using inclusive = false doesn't always work

    varray<Coordinate> intersections( Line cref line ) const
    {
        varray<Coordinate> intersections;

        for_range( i, line_count( ) )
        {
            if( at( i ).intersects( line ) )
            {
                intersections.insert_back( at( i ).intersection( line ) );
            }
        }

        return intersections;
    }

    Path operator+( Line cref l ) const { return Path( lines( ) + l ); }
    Path operator+( Path cref p ) const { return Path( lines( ) + p.lines( ) ); }

    Path & operator+=( Line cref l ) { insert_back( l ); rethis; }
    Path & operator+=( Path cref p ) { insert_back( p.lines( ) ); rethis; }

    Path operator+( Vector cref v ) const { varray<Line> new_lines = ( *this ); for_each( line, new_lines ) { line += v; } return new_lines; }
    Path operator-( Vector cref v ) const { varray<Line> new_lines = ( *this ); for_each( line, new_lines ) { line -= v; } return new_lines; }

    Path & operator+=( Vector cref v ) { rethis = *this + v; }
    Path & operator-=( Vector cref v ) { rethis = *this - v; }

    default_equal( Path );

};

} // namespace geometry
} // namespace axn

#endif /* Path_hpp */
