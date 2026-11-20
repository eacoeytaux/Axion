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

    Path( varray<Point> cref in_pts = { }, bool in_loop = false )
    {
        if( is_one( in_pts.size( ) ) )
        {
            insert_back( Line( in_pts.front( ), in_pts.front( ) ) );
        }
        else if( in_pts.size( ) > 1 )
        {
            for_range( i, in_pts.size( ) - 1 )
            {
                insert_back( Line( in_pts[ i ], in_pts[ i + 1 ] ) );
            }

            if( in_loop && ( in_pts.size( ) >= 3 ) )
            {
                insert_back( Line( in_pts.back( ), in_pts.front( ) ) );
            }
        }
    }

    Path( varray<Line> cref in_lines ) : varray<Line>( in_lines ) { if( !empty( ) ) { for_range( i, size( ) - 1 ) { Assert( at( i ).pt2( ) == at( i + 1 ).pt1( ) ); } } }

    Path( Line cref in_line ) : varray<Line>( { in_line } ) { }

    Path loop( ) { if( size( ) && ( back( ).pt2( ) != front( ).pt1( ) ) ) { return( Path( lines( ) + Line( back( ).pt2( ), front( ).pt1( ) ) ) ); } else { rethis; } }

    virtual Path ref transform( Transform cref in_transform ) override { Transformable::transform( in_transform ); for_each( line, *this ) { line.transform( in_transform ); } rethis; }

    uint line_count( ) const { return( size( ) ); }
    uint point_count( ) const { return( size( ) ? size( ) + 1 : 0 ); }

    varray<Line> cref lines( ) const { rethis; }

    varray<Point> points( ) const { return_if( !line_count( ), { } ); varray<Point> pts; for_each( line, *this ) { pts.insert_back( line.pt1( ) ); } pts.insert_back( back( ).pt2( ) ); return( pts ); }

    bool has_length( ) const { for_each( line, *this ) { return_true_if( line.length( ) ); } return( false ); }
    Planc length( ) const { Planc length = P0; for_each( line, *this ) { length += line.length( ); } return( length ); }

    bool on( Point cref in_pt ) const { for_each( line, *this ) { return_true_if( line.on( in_pt ) ); } return( false ); }
    bool right( Point cref in_pt, bool in_inclusive = false ) const { for_each( line, *this ) { return_false_if( !line.right( in_pt, in_inclusive ) ); } return( true ); }
    bool left( Point cref in_pt, bool in_inclusive = false ) const { for_each( line, *this ) { return_false_if( !line.left( in_pt, in_inclusive ) ); } return( true ); }

    bool in_bounds( Point cref in_pt ) const { if( size( ) ) { MinMaxPoint bounds( front( ).pt1( ) ); for_each( line, *this ) { bounds.update( line.pt2( ) ); } return( bounds.in_bounds( in_pt ) ); } return( false ); }

    bool intersects( Line cref in_line, bool in_inclusive = true ) const { for_range( i, line_count( ) ) { return_true_if( at( i ).intersects( in_line, in_inclusive ) ); } return( false ); }
    bool intersects_self( ) const
    {
        // C! lines that aren't neighbours cross if they touch at all; neighbours always share a point, so they only count
        // C! when one folds back over the other (the last and first lines are neighbours too when the path loops)
        return_false_if( line_count( ) < 2 );

        bool loops = ( back( ).pt2( ) == front( ).pt1( ) );

        for_range( i, line_count( ) )
        {
            for_range( k, line_count( ) )
            {
                continue_if( k <= i );

                Line cref line1 = at( i );
                Line cref line2 = at( k );

                bool has_length1 = !is_zero( line1.length( ) );
                bool has_length2 = !is_zero( line2.length( ) );

                if( k == ( i + 1 ) )
                {
                    return_true_if( ( has_length2 && line1.on( line2.pt2( ) ) ) || ( has_length1 && line2.on( line1.pt1( ) ) ) );
                }
                else if( loops && is_zero( i ) && ( k == ( line_count( ) - 1 ) ) )
                {
                    return_true_if( ( has_length2 && line1.on( line2.pt1( ) ) ) || ( has_length1 && line2.on( line1.pt2( ) ) ) );
                }
                else
                {
                    return_true_if( line1.intersects( line2, true ) );
                }
            }
        }

        return( false );
    }

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

    Path ref segment( uint in_segments ) { rethis = segmented( in_segments ); }
    Path segmented( uint in_segments ) const
    {
        return_if( !is_pos( in_segments ), { } );
        return_if( empty( ), ( *this ) );

        const uint c_segments = max( in_segments, uint( 1 ) );

        varray<Point> pts;
        pts.insert_back( front( ).pt1( ) );
        for_range( i, c_segments - 1 ) { pts.insert_back( point( dec( i + 1 ) / dec( c_segments ) ) ); }
        pts.insert_back( back( ).pt2( ) );

        return( Path( pts ) );
    }

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

inline Path Line::segmented( uint in_segments ) const { return( Path( *this ).segmented( in_segments ) ); }

} // namespace geometry
} // namespace axn

#endif /* Path_hpp */
