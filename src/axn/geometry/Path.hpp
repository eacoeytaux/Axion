#ifndef Path_hpp
#define Path_hpp

#include "axn/geometry/Planc.hpp"
#include "axn/geometry/Point.hpp"
#include "axn/geometry/Angle.hpp"
#include "axn/geometry/Vector.hpp"
#include "axn/geometry/Line.hpp"
#include "axn/geometry/Transform.hpp"

namespace axn
{
namespace geometry
{

class Path : public Transformable, public varray<Line>
{

public:

    Path( ) { }

    Path( varray<Point> cref in_points, bool in_loop = false )
    {
        if( is_one( in_points.size( ) ) )
        {
            insert_back( Line( in_points.front( ), in_points.front( ) ) );
        }
        else if( in_points.size( ) > 1 )
        {
            for_range( i, in_points.size( ) - 1 )
            {
                insert_back( Line( in_points[ i ], in_points[ i + 1 ] ) );
            }

            if( in_loop && ( in_points.size( ) >= 3 ) )
            {
                insert_back( Line( in_points.back( ), in_points.front( ) ) );
            }
        }
    }

    Path( varray<Line> cref in_lines ) : varray<Line>( in_lines ) { if( !empty( ) ) { for_range( i, size( ) - 1 ) { Assert( at( i ).pt2( ) == at( i + 1 ).pt1( ) ); } } }

    Path( Line cref in_line ) : varray<Line>( { in_line } ) { }

    Path loop( ) { if( size( ) && ( back( ).pt2( ) != front( ).pt1( ) ) ) { return( Path( lines( ) + Line( back( ).pt2( ), front( ).pt1( ) ) ) ); } else { rethis; } }

    Path ref transform( Transform cref in_transform ) override { Transformable::transform( in_transform ); for_each( line, *this ) { line.transform( in_transform ); } rethis; }

    uint line_count( ) const { return( size( ) ); }
    uint point_count( ) const { return( size( ) ? size( ) + 1 : 0 ); }

    varray<Line> cref lines( ) const { rethis; }

    varray<Point> points( ) const { return_if( !line_count( ), { } ); varray<Point> points; for_each( line, *this ) { points.insert_back( line.pt1( ) ); } points.insert_back( back( ).pt2( ) ); return( points ); }

    bool has_length( ) const { for_each( line, *this ) { return_true_if( line.length( ) ); } return( false ); }
    Planc length( ) const { Planc length = P0; for_each( line, *this ) { length += line.length( ); } return( length ); }

    bool on( Point cref in_pt ) const { for_each( line, *this ) { return_true_if( line.on( in_pt ) ); } return( false ); }
    bool right( Point cref in_pt, bool in_inclusive = false ) const { for_each( line, *this ) { return_false_if( !line.right( in_pt, in_inclusive ) ); } return( true ); }
    bool left( Point cref in_pt, bool in_inclusive = false ) const { for_each( line, *this ) { return_false_if( !line.left( in_pt, in_inclusive ) ); } return( true ); }

    bool in_bounds( Point cref in_pt ) const { if( size( ) ) { MinMaxPoint bounds( front( ).pt1( ) ); for_each( line, *this ) { bounds.update( line.pt2( ) ); } return( bounds.in_bounds( in_pt ) ); } return( false ); }

    bool intersects( Line cref in_line, bool in_inclusive = true ) const { for_range( i, line_count( ) ) { return_true_if( at( i ).intersects( in_line, in_inclusive ) ); } return( false ); }
    bool intersects_self( ) const { for_range( i, line_count( ) - 1 ) { for_range( j, line_count( ) - i - 1 ) { return_true_if( at( i ).intersects( at( i + j + 1 ), false ) && ( j || ( at( i ).intersection( at( i + 1 ) ) != at( i ).pt2( ) ) ) ); } } return( false ); } // todo always using inclusive = false doesn't always work

    varray<Point> intersections( Line cref in_line ) const
    {
        varray<Point> intersections;

        for_each( line, *this )
        {
            if( line.intersects( in_line ) )
            {
                intersections.insert_back( line.intersection( in_line ) );
            }
        }

        return( intersections );
    }

    Point point( dec in_percent ) const { return_if( empty( ), ORIGIN ); Planc total_length = P0; varray<Planc> lengths( size( ) ); for_range( i, lengths.size( ) ) { total_length += lengths[ i ] = at( i ).length( ); } uint i = 0; Planc remaining = ( total_length * in_percent ); while( remaining > lengths[ i ] ) { remaining -= lengths[ i ]; ++i; } Line cref line = at( i ); return( line.point( remaining / line.length( ) ) ); }

    Path ref operator+=( Line cref in_line ) { insert_back( in_line ); rethis; }
    Path ref operator+=( Path cref in_path ) { insert_back( in_path.lines( ) ); rethis; }

    Path ref operator+=( Vector cref in_vec ) { rethis = ( *this + in_vec ); }
    Path ref operator-=( Vector cref in_vec ) { rethis = ( *this - in_vec ); }

    Path operator+( Line cref in_line ) const { return( Path( lines( ) + in_line ) ); }
    Path operator+( Path cref in_path ) const { return( Path( lines( ) + in_path.lines( ) ) ); }

    Path operator+( Vector cref in_vec ) const { varray<Line> new_lines = ( *this ); for_each( line, new_lines ) { line += in_vec; } return( new_lines ); }
    Path operator-( Vector cref in_vec ) const { varray<Line> new_lines = ( *this ); for_each( line, new_lines ) { line -= in_vec; } return( new_lines ); }

    equal_default( Path );

};

} // namespace geometry
} // namespace axn

#endif /* Path_hpp */
