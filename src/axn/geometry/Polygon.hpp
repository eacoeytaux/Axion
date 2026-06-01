#ifndef Polygon_hpp
#define Polygon_hpp

#include "axn/geometry/Planc.hpp"
#include "axn/geometry/Point.hpp"
#include "axn/geometry/Angle.hpp"
#include "axn/geometry/Vector.hpp"
#include "axn/geometry/Line.hpp"
#include "axn/geometry/Path.hpp"
#include "axn/geometry/Arc.hpp"
#include "axn/geometry/Triangle.hpp"
#include "axn/geometry/FixedRectangle.hpp"
#include "axn/geometry/Transform.hpp"

namespace axn
{
namespace geometry
{

class Polygon : public Transformable
{

public:

    enum expand_type
    {
        POINTED,
        FLATTENED,
        ROUNDED,
    };

private:

    varray<Point> m_points_raw;
    mutable varray<Point> m_points;

    // stores in 3s
    mutable vvarray<uint> m_triangle_indices;

    mutable bool m_bounds_dirty = false;
    mutable Min<Planc> m_lower_bound_x;
    mutable Min<Planc> m_lower_bound_y;
    mutable Max<Planc> m_upper_bound_x;
    mutable Max<Planc> m_upper_bound_y;

    bool m_convex = true;

public:

    Polygon( ) { }

    Polygon( varray<Point> cref in_points )
    {
        m_points = in_points;

        m_convex = true;

        uint pt_count = m_points.size( );

        if( pt_count )
        {
            m_lower_bound_x.reset( );
            m_lower_bound_y.reset( );

            m_upper_bound_x.reset( );
            m_upper_bound_y.reset( );

            if( pt_count <= 3 )
            {
                for_range( i, pt_count )
                {
                    Point cref pt = m_points[ i ];

                    m_lower_bound_x.update( pt.x( ) );
                    m_lower_bound_y.update( pt.y( ) );

                    m_upper_bound_x.update( pt.x( ) );
                    m_upper_bound_y.update( pt.y( ) );
                }

                if( pt_count == 3 )
                {
                    if( Line( m_points[ 0 ], m_points[ 1 ] ).right( m_points[ 2 ] ) )
                    {
                        // is clockwise
                        m_points.reverse( );
                    }
                }
            }
            else
            {
                // iterate through points and determine xy boundaries, convex, and clockwise see
                // https://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order

                // default to true then check for violations
                bool convex_cw = true;
                bool convex_ccw = true;

                dec edge_curve = 0.0;

                while( ( pt_count > 1 ) && ( m_points.front( ) == m_points.back( ) ) )
                {
                    m_points.remove_back( );
                    pt_count--;
                }

                uint i = 0;
                while( i < pt_count )
                {
                    Point cref pt = m_points[ i ];

                    if( i && ( pt == m_points[ i - 1 ] ) )
                    {
                        m_points.remove_at( i-- );
                        pt_count--;
                    }
                    else
                    {
                        m_lower_bound_x.update( pt.x( ) );
                        m_lower_bound_y.update( pt.y( ) );

                        m_upper_bound_x.update( pt.x( ) );
                        m_upper_bound_y.update( pt.y( ) );

                        Point cref prev_pt = m_points[ ( i + pt_count - 1 ) % pt_count ];

                        edge_curve += dec( ( pt.x( ) - prev_pt.x( ) ) * ( pt.y( ) + prev_pt.y( ) ) );

                        if( convex_cw || convex_ccw )
                        {
                            Line line( m_points[ ( i + pt_count - 2 ) % pt_count ], prev_pt );

                            if( convex_cw && line.left( pt ) )
                            {
                                convex_cw = false;
                            }

                            if( convex_ccw && line.right( pt ) )
                            {
                                convex_ccw = false;
                            }
                        }
                    }

                    i++;
                }

                bool clockwise = is_pos( edge_curve );

                if( clockwise )
                {
                    m_points_raw.reverse( );
                    m_points.reverse( );
                }

                m_convex = clockwise ? convex_cw : convex_ccw;
            }
        }

        m_points_raw = m_points;
    }

    Polygon( FixedRectangle cref in_rect )
    {
        *this = Polygon( in_rect.points( ) );
    }

    static Polygon triangle( Point cref in_pt1, Point cref in_pt2, Point cref in_pt3 ) { return( Polygon( { in_pt1, in_pt2, in_pt3 } ) ); }
    static Polygon isosceles( Point cref in_tip, Point cref in_base_mid, Planc cref in_base_length )
    {
        Angle a( in_base_mid, in_tip );
        Point b1 = in_base_mid + VectorA( a + RIGHT, half( in_base_length ) );
        Point b2 = in_base_mid + VectorA( a - RIGHT, half( in_base_length ) );
        return( Polygon( { in_tip, b1, b2 } ) );
    }

    static Polygon rectangle( Planc cref in_width, Planc cref in_height, Angle cref in_rotation ) { return( rectangle( in_width, in_height ).rotate( in_rotation ) ); }
    static Polygon rectangle( Planc cref in_width, Planc cref in_height, Point cref in_center ) { return( rectangle( in_width, in_height ).move( in_center ) ); }
    static Polygon rectangle( Planc cref in_width, Planc cref in_height, Point cref in_center, Angle cref in_rotation ) { return( rectangle( in_width, in_height ).rotate( in_rotation ).move( in_center ) ); }
    static Polygon rectangle( Planc cref in_width, Planc cref in_height )
    {
        Planc half_width = half( in_width );
        Planc half_height = half( in_height );

        return( Polygon( { Point( +half_width, +half_height ),
                          Point( -half_width, +half_height ),
                          Point( -half_width, -half_height ),
                          Point( +half_width, -half_height ) } ) );
    }

    static Polygon square( Planc cref in_width ) { return( rectangle( in_width, in_width ) ); }
    static Polygon square( Planc cref in_width, Angle cref in_rotation ) { return( rectangle( in_width, in_width, in_rotation ) ); }
    static Polygon square( Planc cref in_width, Point cref in_center ) { return( rectangle( in_width, in_width, in_center ) ); }
    static Polygon square( Planc cref in_width, Point cref in_center, Angle cref in_rotation ) { return( rectangle( in_width, in_width, in_center, in_rotation ) ); }

    static Polygon equilateral( uint in_side_count, Planc cref in_radius, Angle cref in_rotation ) { return( equilateral( in_side_count, in_radius ).rotate( in_rotation ) ); }
    static Polygon equilateral( uint in_side_count, Planc cref in_radius, Point cref in_center ) { return( equilateral( in_side_count, in_radius ).move( in_center ) ); }
    static Polygon equilateral( uint in_side_count, Planc cref in_radius, Point cref in_center, Angle cref in_rotation ) { return( equilateral( in_side_count, in_radius ) ).rotate( in_rotation ).move( in_center ); }
    static Polygon equilateral( uint in_side_count, Planc cref in_radius )
    {
        static umap<uint, Polygon> equilaterals;

        if( !equilaterals.contains( in_side_count ) )
        {
            varray<Point> points( in_side_count );

            Angle start_angle( RIGHT ); // start at the top
            Angle delta_angle( Angle::fraction( in_side_count ) );
            for_each( offset, points )
            {
                offset = VectorA( start_angle );
                start_angle += delta_angle;
            }

            equilaterals.insert( in_side_count, Polygon( points ) );
        }

        return( Polygon( equilaterals[ in_side_count ] ).scale( in_radius ) );
    }

    static Polygon circle( Planc cref in_radius = 1.0, Point cref in_center = ORIGIN ) { uint side_count = curve_point_count( in_radius ); return( Polygon::equilateral( side_count, in_radius, in_center, Angle::fraction( x2( side_count ) ) ) ); }
    static Polygon circle( Point cref in_center ) { return( Polygon::circle( ).move( in_center ) ); }

    varray<Point> cref points( bool in_transformed = true ) const { if( !in_transformed ) { return( m_points_raw ); } else { apply_transform( ); return( m_points ); } }

    Path perimeter( bool in_transformed = true ) const { return( Path( points( in_transformed ), true ) ); }

    vvarray<uint> cref triangle_indices( ) const
    {
        if( !m_triangle_indices.size( ) && ( m_points_raw.size( ) >= 3 ) )
        {
            if( convex( ) )
            {
                for_range( i, m_points_raw.size( ) - 2 )
                {
                    m_triangle_indices.insert_back( { 0, i + 1, i + 2 } );
                }
            }
            else
            {
                varray<Point> temp_points = m_points_raw;

                varray<uint> temp_pt_indices( temp_points.size( ) );
                for_range( i, temp_pt_indices.size( ) )
                {
                    temp_pt_indices[ i ] = i;
                }

                uint i = 0;
                while( temp_points.size( ) > 3 )
                {
                    i %= temp_points.size( );
                    uint i_prev = ( i + ( temp_points.size( ) - 1 ) ) % temp_points.size( );
                    uint i_next = ( i + 1 ) % temp_points.size( );

                    Point cref pt = temp_points[ i ];
                    Point cref pt_prev = temp_points[ i_prev ];
                    Point cref pt_next = temp_points[ i_next ];

                    bool is_ear = false;

                    if( !is_neg( cross( pt_prev, pt, pt_next ) ) )
                    {
                        bool containing = false;

                        for_range( j, temp_points.size( ) )
                        {
                            if( ( j != i ) && ( j != i_prev ) && ( j != i_next ) )
                            {
                                Point cref pt_temp = temp_points[ j ];

                                if( Triangle( pt, pt_next, pt_prev ).contains( pt_temp ) )
                                {
                                    containing = true;
                                    break;
                                }
                            }
                        }

                        is_ear = !containing;
                    }

                    if( is_ear )
                    {
                        m_triangle_indices.insert_back( varray<uint>( { temp_pt_indices[ i ], temp_pt_indices[ i_next ], temp_pt_indices[ i_prev ] } ) );

                        temp_points.remove_at( i );
                        temp_pt_indices.remove_at( i );
                    }
                    else
                    {
                        i++;
                    }
                }

                m_triangle_indices.insert_back( temp_pt_indices );
            }
        }

        return( m_triangle_indices );
    }

    varray<Polygon> triangles( bool in_transformed = true ) const
    {
        varray<Polygon> triangles;
        varray<Point> cref pts = points( in_transformed );

        for_each( indices, triangle_indices( ) )
        {
            triangles.insert_back( Polygon::triangle( pts[ indices[ 0 ] ], pts[ indices[ 1 ] ], pts[ indices[ 2 ] ] ) );
        }

        return( triangles );
    }

    bool convex( ) const { return( m_convex ); }

    uint sides( ) const { return( m_points.size( ) ); }

    Planc area( ) const
    {
        varray<Point> cref pts = m_points_raw;

        if( pts.size( ) >= 3 )
        {
            Planc area = ( ( pts.back( ).x( ) * pts.front( ).y( ) ) - ( pts.back( ).y( ) * pts.front( ).x( ) ) );

            for_range( i, pts.size( ) - 1 )
            {
                Point cref pt1 = pts[ i ];
                Point cref pt2 = pts[ i + 1 ];

                area += ( ( pt1.x( ) * pt2.y( ) ) - ( pt1.y( ) * pt2.x( ) ) );
            }

            return( abs( half( area ) ) * cumulative_transform( ).determinant( ) );
        }
        else
        {
            return( 0.0 );
        }
    }

    Point centroid( ) const
    {
        Point centroid;
        Planc area = 0.0;

        varray<Point> cref pts = m_points_raw;

        for_each( ti, triangle_indices( ) )
        {
            Triangle t( pts[ ti[ 0 ] ], pts[ ti[ 1 ] ], pts[ ti[ 2 ] ] );

            centroid += t.centroid( ) * t.area( );
            area += t.area( );
        }

        return( cumulative_transform( ).apply( area ? ( centroid / area ) : centroid ) );
    }

    bool contains( Point cref in_pt, bool in_inclusive = true ) const
    {
        Point pt_trans = apply_cumulative_transform_inverse( in_pt );

        if( sides( ) )
        {
            if( sides( ) >= 3 )
            {
                varray<Line> lines = perimeter( false ).lines( );

                if( convex( ) )
                {
                    for_each( line, lines )
                    {
                        if( in_inclusive )
                        {
                            if( line.on( pt_trans ) )
                            {
                                return( true );
                            }
                        }

                        if( line.right( pt_trans ) )
                        {
                            return( false );
                        }
                    }

                    return( true );
                }
                else
                {
                    uint intersection_count = 0;

                    for_each( line, lines )
                    {
                        if( Line( pt_trans, Point( in_pt.x( ), line.lower_bound_y( ) - 1.0 ) ).intersects( line ) )
                        {
                            intersection_count++;
                        }
                    }

                    return( is_odd( intersection_count ) );
                }
            }
            else if( in_inclusive )
            {
                if( sides( ) == 1 )
                {
                    return( points( false ).front( ) == pt_trans );
                }
                else if( sides( ) == 2 )
                {
                    return( perimeter( false ).front( ).on( pt_trans ) );
                }
            }
        }

        return( false );
    }

    bool intersects( Line cref in_line, bool in_inclusive = true ) const
    {
        Line line_transformed = in_line;
        line_transformed.transform( cumulative_transform_inverse( ) );

        for_each( line, perimeter( false ).lines( ) )
        {
            if( line.intersects( line_transformed, in_inclusive ) ) // todo this could be a false negative with inclusive = false
            {
                return( true );
            }
        }

        return( contains( in_line.pt1( ) ) || contains( in_line.pt2( ) ) );
    }

    varray<Line> intersection( Line cref in_line ) const
    {
        varray<Point> intersection_points = { };

        bool pt1 = contains( in_line.pt1( ) );
        bool pt2 = contains( in_line.pt2( ) );

        if( pt1 && pt2 )
        {
            if( convex( ) )
            {
                return{ in_line };
            }
        }

        if( pt1 )
        {
            intersection_points.insert_back( in_line.pt1( ) );
        }

        if( pt2 )
        {
            intersection_points.insert_back( in_line.pt2( ) );
        }

        Line line_transformed = in_line;
        line_transformed.transform( cumulative_transform_inverse( ) );

        for_each( line, perimeter( false ).lines( ) )
        {
            if( line.intersects( line_transformed ) )
            {
                intersection_points.insert_back( apply_cumulative_transform( line.intersection( line_transformed ) ) );

                if( convex( ) )
                {
                    if( intersection_points.size( ) )
                    {
                        break;
                    }
                }
            }
        }

        if( is_even( intersection_points.size( ) ) )
        {
            // sort intersections by distance from start of line
            struct LineIntersectionDistance { LineIntersectionDistance( Point cref in_pt ) : origin( in_pt ) { } Point origin; bool operator( )( Point cref in_pt1, Point cref in_pt2 ) { return( origin.closer_than( in_pt1, in_pt2 ) ); } };

            intersection_points.sort( LineIntersectionDistance( in_line.pt1( ) ) );

            varray<Line> intersections( intersection_points.size( ) / 2 );

            for_range( i, intersections.size( ) )
            {
                intersections[ i ] = Line( intersection_points[ ( i * 2 ) ], intersection_points[ ( i * 2 ) + 1 ] );
            }

            return( intersections );
        }
        else
        {
            return{ Line( intersection_points.front( ), intersection_points.front( ) ) };
        }

    }

    Planc lower_bound_x( bool in_tight = false ) const { apply_bounds_transform( in_tight ); return( m_lower_bound_x ); }
    Planc lower_bound_y( bool in_tight = false ) const { apply_bounds_transform( in_tight ); return( m_lower_bound_y ); }
    Planc upper_bound_x( bool in_tight = false ) const { apply_bounds_transform( in_tight ); return( m_upper_bound_x ); }
    Planc upper_bound_y( bool in_tight = false ) const { apply_bounds_transform( in_tight ); return( m_upper_bound_y ); }

    Planc bound_width( bool in_tight = false ) const { return( upper_bound_x( in_tight ) - lower_bound_x( in_tight ) ); }
    Planc bound_height( bool in_tight = false ) const { return( upper_bound_y( in_tight ) - lower_bound_y( in_tight ) ); }

    Point bounds_center( bool in_tight = false ) const { return( bounds( in_tight ).center( ) ); }
    FixedRectangle bounds( bool in_tight = false ) const
    {
        Point top( upper_bound_x( in_tight ), upper_bound_y( in_tight ) );
        Point bottom( lower_bound_x( in_tight ), lower_bound_y( in_tight ) );

        return( FixedRectangle( abs( top.x( ) - bottom.x( ) ), abs( top.y( ) - bottom.y( ) ), midpoint( bottom, top ) ) );
    }

    Polygon convex_hull( ) const { return( convex_from_cloud( points( true ) ) ); }

    static Polygon convex_from_cloud( varray<Point> cref in_points )
    {
        const uint pt_count = in_points.size( );

        if( pt_count )
        {
            // using Graham Scan

            // find pt with minimum y value
            uint min_i = 0;

            for_range( i, pt_count - 1 )
            {
                if( ( in_points[ i + 1 ].y( ) < in_points[ min_i ].y( ) ) ||
                    ( ( in_points[ i + 1 ].x( ) < in_points[ min_i ].x( ) ) &&
                      ( in_points[ i + 1 ].y( ) == in_points[ min_i ].y( ) ) ) )
                {
                    min_i = i + 1;
                }
            }

            // sort points by angle to min
            struct AngleToMin { AngleToMin( Point cref in_pt ) : min( in_pt ) { } Point min; bool operator( )( Point cref in_pt1, Point cref in_pt2 ) { Planc pt = cross( min, in_pt1, in_pt2 ); return( ( pt > 0.0 ) || ( !pt && min.closer_than( in_pt1, in_pt2 ) ) ); } };

            varray<Point> sorted_points = in_points;

            sorted_points.remove_at( min_i );
            sorted_points.sort( AngleToMin( in_points[ min_i ] ) );

            varray<Point> convex_hull = { in_points[ min_i ], sorted_points[ 0 ], sorted_points[ 1 ] };

            for_range( i, sorted_points.size( ) - 2 )
            {
                // remove hull points until current pt is part of hull
                while( ( convex_hull.size( ) > 1 ) && ( cross( convex_hull[ convex_hull.size( ) - 2 ], convex_hull[ convex_hull.size( ) - 1 ], sorted_points[ i + 2 ] ) <= 0.0 ) )
                {
                    convex_hull.remove_back( );
                }

                // add to hull
                convex_hull.insert_back( sorted_points[ i + 2 ] );
            }

            return( Polygon( convex_hull ) );
        }

        return( Polygon( ) );
    }

    Polygon ref invert( ) { *this = inverted( ); }
    Polygon inverted( ) const
    {
        varray<Point> inverted_points = points( true );
        for_range( i, inverted_points.size( ) ) { inverted_points[ i ] = -inverted_points[ i ]; }

        return( Polygon( inverted_points ) );
    }

    Polygon ref expand( Planc cref in_expansion, expand_type in_expand = POINTED ) { *this = expanded( in_expansion, in_expand ); }
    Polygon expanded( Planc cref in_expansion, expand_type in_expand = POINTED ) const
    {
        Assert( !is_neg( in_expansion ) );

        // TODO only works for convex

        return_if( !in_expansion, ( *this ) );

        const varray<Point> pts = points( true );
        const uint pt_count = pts.size( );

        varray<Point> new_points;

        for_range( i, pt_count )
        {
            uint pt_prev_index = ( i ? ( i - 1 ) : ( pt_count - 1 ) );
            uint pt_next_index = ( ( i + 1 ) % pt_count );

            Line line_prev = Line( pts[ pt_prev_index ], pts[ i ] );
            Line line_next = Line( pts[ i ], pts[ pt_next_index ] );

            bool concave = ( ( line_next.angle( ).truncated( ) - line_prev.angle( ).truncated( ) ).truncated( ) > PI );

            if( in_expand == ROUNDED )
            {
                if( !concave )
                {
                    new_points.insert_back( Arc::ccw( in_expansion, pts[ i ], ( line_prev.angle( ) - RIGHT ), ( line_next.angle( ) - RIGHT ) ).points( ) );
                }
                else
                {

                }
            }
            else if( in_expand == POINTED )
            {
                if( !concave )
                {
                    Vector v_prev( line_prev.pt1( ), line_prev.pt2( ) );
                    Vector v_next( line_next.pt1( ), line_next.pt2( ) );

                    v_prev.rotate_right( false );
                    v_next.rotate_right( false );

                    v_prev.magnitude( in_expansion );
                    v_next.magnitude( in_expansion );

                    line_prev += v_prev;
                    line_next += v_next;

                    new_points.insert_back( line_prev.intersection( line_next ) );
                }
                else
                {

                }
            }
            else if( in_expand == FLATTENED )
            {
                if( !concave )
                {
                    new_points.insert_back( line_prev.pt2( ) );
                    new_points.insert_back( line_next.pt1( ) );
                }
                else
                {

                }
            }
            else
            {
                Assert( "no expansion type set" );
            }
        }

        return( Polygon( new_points ) );
    }

    bool intersects( Polygon cref in_polygon, bool in_inclusive = true ) const
    {
        return_false_if( !points( ).size( ) || !in_polygon.points( ).size( ) );
        return_true_if( contains( in_polygon.points( ).front( ), in_inclusive ) );
        return_true_if( in_polygon.contains( points( ).front( ), in_inclusive ) );
        for_each( line1, perimeter( ).lines( ) ) { for_each( line2, in_polygon.perimeter( ).lines( ) ) { return_true_if( line1.intersects( line2, in_inclusive ) ); } }
        return( false );
    }

    bool overlaps( Polygon cref in_polygon, bool in_inclusive = true ) const { return( in_polygon.overlapped( *this, in_inclusive ) ); }
    bool overlapped( Polygon cref in_polygon, bool in_inclusive = true ) const
    {
        return_false_if( !points( ).size( ) || !in_polygon.points( ).size( ) );
        return_false_if( !in_polygon.contains( points( ).front( ), !in_inclusive ) );
        for_each( line1, perimeter( ).lines( ) ) { for_each( line2, in_polygon.perimeter( ).lines( ) ) { return_false_if( line1.intersects( line2, !in_inclusive ) ); } }
        return( true );
    }

    Polygon ref join( Polygon cref in_polygon ) { *this = joined( in_polygon ); }
    Polygon joined( Polygon cref in_polygon ) const
    {
        return_if( !intersects( in_polygon ), *this );
        return_if( overlaps( in_polygon ), *this );
        return_if( overlapped( in_polygon ), in_polygon );

        Polygon cref polygon1 = *this;
        Polygon cref polygon2 = in_polygon;

        varray<Point> intersections;
        varray<Point> points1 = polygon1.points( );
        varray<Point> points2 = polygon2.points( );

        varray<Line> lines1 = polygon1.perimeter( ).lines( );
        varray<Line> lines2 = polygon2.perimeter( ).lines( );

        vvarray<uint> edges1;
        vvarray<uint> edges2;

        bool both_convex = ( polygon1.convex( ) && polygon2.convex( ) );

        for_each( line1, lines1 )
        {
            for_each( line2, lines2 )
            {
                break_if( both_convex && ( intersections.size( ) == 2 ) );
                if( line1.intersects( line2 ) )
                {
                    intersections.insert_back( line1.intersection( line2 ) );
                }
            }
        }

        Assert( intersections.size( ) );

        // TODO!!!

        return( Polygon( ) );
    }

    transform_functions( Polygon );

    Polygon ref operator+=( Vector cref in_vec ) { return( move( +in_vec ) ); }
    Polygon ref operator-=( Vector cref in_vec ) { return( move( -in_vec ) ); }

    Polygon ref operator*=( Planc cref in_p ) { return( scale( in_p ) ); }
    Polygon ref operator/=( Planc cref in_p ) { return( scale( inverse( in_p ) ) ); }

    Polygon operator+( Vector cref in_vec ) const { return( Polygon( *this ).move( +in_vec ) ); }
    Polygon operator-( Vector cref in_vec ) const { return( Polygon( *this ).move( -in_vec ) ); }

    Polygon operator*( Planc cref in_p ) const { return( Polygon( *this ).scale( in_p ) ); }
    Polygon operator/( Planc cref in_p ) const { return( Polygon( *this ).scale( inverse( in_p ) ) ); }

    default_equal( Polygon );

private:

    Polygon cref dirty( ) const override { Transformable::dirty( ); m_bounds_dirty = true; rethis; }
    Polygon cref clean( ) const override { Transformable::clean( ); m_bounds_dirty = false; rethis; }

    void apply_transform( ) const
    {
        if( is_dirty( ) )
        {
            Transform cref t = cumulative_transform( );

            for_range( i, m_points.size( ) )
            {
                m_points[ i ] = t.apply( m_points_raw[ i ] );

                m_lower_bound_x.update( m_points[ i ].x( ) );
                m_lower_bound_y.update( m_points[ i ].y( ) );

                m_upper_bound_x.update( m_points[ i ].x( ) );
                m_upper_bound_y.update( m_points[ i ].y( ) );
            }

            clean( );
        }
    }

    void apply_bounds_transform( bool in_tight ) const
    {
        if( in_tight )
        {
            apply_transform( );
        }
        else if( m_bounds_dirty )
        {
            Transform cref t = transform( );

            Point pt1 = Point( m_lower_bound_x, m_lower_bound_y ).transform( t );
            Point pt2 = Point( m_upper_bound_x, m_lower_bound_y ).transform( t );
            Point pt3 = Point( m_lower_bound_x, m_upper_bound_y ).transform( t );
            Point pt4 = Point( m_upper_bound_x, m_upper_bound_y ).transform( t );

            m_lower_bound_x = min<Planc>( { pt1.x( ), pt2.x( ), pt3.x( ), pt4.x( ) } );
            m_lower_bound_y = min<Planc>( { pt1.y( ), pt2.y( ), pt3.y( ), pt4.y( ) } );
            m_upper_bound_x = max<Planc>( { pt1.x( ), pt2.x( ), pt3.x( ), pt4.x( ) } );
            m_upper_bound_y = max<Planc>( { pt1.y( ), pt2.y( ), pt3.y( ), pt4.y( ) } );

            m_bounds_dirty = false;
        }
    }

};

} // namespace geometry
} // namespace axn

#endif /* Polygon_hpp */
