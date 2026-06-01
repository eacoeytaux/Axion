#ifndef FixedRectangle_hpp
#define FixedRectangle_hpp

#include "axn/geometry/Planc.hpp"
#include "axn/geometry/Point.hpp"
#include "axn/geometry/Angle.hpp"
#include "axn/geometry/Vector.hpp"
#include "axn/geometry/Line.hpp"
#include "axn/geometry/Path.hpp"
#include "axn/geometry/Transform.hpp"

namespace axn
{
namespace geometry
{

class FixedRectangle
{

private:

    Planc m_width = P0;
    Planc m_height = P0;

    Point m_center = ORIGIN;

public:

    FixedRectangle( ) { }

    FixedRectangle( Planc cref in_width, Planc cref in_height ) { width( in_width ); height( in_height ); }
    FixedRectangle( Planc cref in_width, Planc cref in_height, Point cref in_center ) { width( in_width ); height( in_height ); center( in_center ); }

    FixedRectangle( Point cref in_center ) { center( in_center ); }

    FixedRectangle( Point cref in_bottom, Point cref in_top ) { width( abs( in_top.x( ) - in_bottom.x( ) ) ); height( abs( in_top.y( ) - in_bottom.y( ) ) ); center( midpoint( in_bottom, in_top ) ); }

    Point cref center( ) const { return( m_center ); }
    FixedRectangle ref center( Point cref in_center ) { m_center = in_center; rethis; }

    Planc cref width( ) const { return( m_width ); }
    FixedRectangle ref width( Planc cref in_width ) { Assert( !is_neg( in_width ), "cannot have negative width" ); m_width = in_width; rethis; }

    Planc cref height( ) const { return( m_height ); }
    FixedRectangle ref height( Planc cref in_height ) { Assert( !is_neg( in_height ), "cannot have negative height" ); m_height = in_height; rethis; }

    Planc area( ) const { return( width( ) * height( ) ); }

    varray<Point> points( ) const { return{ top_right( ), top_left( ), bottom_left( ), bottom_right( ) }; }

    Path perimeter( ) const { Path( { top( ), left( ), bottom( ), right( ) } ); }

    Planc upper_bound_x( ) const { return( center( ).x( ) + half( width( ) ) ); }
    Planc upper_bound_y( ) const { return( center( ).y( ) + half( height( ) ) ); }

    Planc lower_bound_x( ) const { return( center( ).x( ) - half( width( ) ) ); }
    Planc lower_bound_y( ) const { return( center( ).y( ) - half( height( ) ) ); }

    Point upper( ) const { return( top_right( ) ); }
    Point lower( ) const { return( bottom_left( ) ); }

    Point top_right( ) const { return( center( ) + half( Vector( +width( ), +height( ) ) ) ); }
    Point top_left( ) const { return( center( ) + half( Vector( -width( ), +height( ) ) ) ); }
    Point bottom_left( ) const { return( center( ) + half( Vector( -width( ), -height( ) ) ) ); }
    Point bottom_right( ) const { return( center( ) + half( Vector( +width( ), -height( ) ) ) ); }

    Line top( ) const { return( Line( top_right( ), top_left( ) ) ); }
    Line bottom( ) const { return( Line( bottom_left( ), bottom_right( ) ) ); }
    Line left( ) const { return( Line( top_left( ), bottom_left( ) ) ); }
    Line right( ) const { return( Line( bottom_right( ), top_right( ) ) ); }

    bool contains( Point cref in_pt, bool in_inclusive = true ) const { return( in_range( in_pt.x( ), lower_bound_x( ), upper_bound_x( ), in_inclusive ) && in_range( in_pt.y( ), lower_bound_y( ), upper_bound_y( ), in_inclusive ) ); }

    bool intersects( Line cref in_line ) const { return( intersection( in_line ).size( ) ); }

    varray<Line> intersection( Line cref in_line ) const
    {
        if( is_pos( width( ) ) && is_pos( height( ) ) )
        {
            bool b1 = contains( in_line.pt1( ) );
            bool b2 = contains( in_line.pt2( ) );

            if( b1 && b2 )
            {
                return{ in_line };
            }

            // TODO!!!

            if( b1 || b2 )
            {
                return{ in_line }; // TODO NOT RIGHT!!!
            }
        }
        else
        {
            if( is_pos( width( ) ) )
            {
                Line line( center( ) - VectorX( half( width( ) ) ), center( ) + VectorX( half( width( ) ) ) );
                if( line.intersects( line ) )
                {
                    Point pt = line.intersection( line );
                    return{ Line( pt, pt ) };
                }
            }
            else if( is_pos( height( ) ) )
            {
                Line line( center( ) - VectorY( half( height( ) ) ), center( ) + VectorY( half( height( ) ) ) );
                if( line.intersects( line ) )
                {
                    Point pt = line.intersection( line );
                    return{ Line( pt, pt ) };
                }
            }
            else
            {
                if( in_line.on( center( ), true ) )
                {
                    return{ Line( center( ), center( ) ) };
                }
            }
        }

        return{ };
    }

    bool intersects( FixedRectangle cref in_rect ) const
    {
        return( ( lower_bound_x( ) < in_rect.upper_bound_x( ) ) &&
                ( upper_bound_x( ) > in_rect.lower_bound_x( ) ) &&
                ( upper_bound_y( ) > in_rect.lower_bound_y( ) ) &&
                ( lower_bound_y( ) < in_rect.upper_bound_y( ) ) );
    }

    FixedRectangle intersection( FixedRectangle cref in_rect ) const
    {
        if( !intersects( in_rect ) )
        {
            return( FixedRectangle( ) );
        }
        else
        {
            return( FixedRectangle( Point( max( lower_bound_x( ), in_rect.lower_bound_x( ) ),
                                           max( lower_bound_y( ), in_rect.lower_bound_y( ) ) ),
                                    Point( min( upper_bound_x( ), in_rect.upper_bound_x( ) ),
                                           min( upper_bound_y( ), in_rect.upper_bound_y( ) ) ) ) );
        }
    }
    FixedRectangle ref intersect( FixedRectangle cref in_rect )
    {
        if( !intersects( in_rect ) )
        {
            rethis = FixedRectangle( );
        }
        else
        {
            rethis = FixedRectangle( Point( max( lower_bound_x( ), in_rect.lower_bound_x( ) ),
                                            max( lower_bound_y( ), in_rect.lower_bound_y( ) ) ),
                                     Point( min( upper_bound_x( ), in_rect.upper_bound_x( ) ),
                                            min( upper_bound_y( ), in_rect.upper_bound_y( ) ) ) );
        }
    }

    FixedRectangle ref union_with( FixedRectangle cref in_rect )
    {
        rethis = FixedRectangle( Point( min( lower_bound_x( ), in_rect.lower_bound_x( ) ),
                                        min( lower_bound_y( ), in_rect.lower_bound_y( ) ) ),
                                 Point( max( upper_bound_x( ), in_rect.upper_bound_x( ) ),
                                        max( upper_bound_y( ), in_rect.upper_bound_y( ) ) ) );
    }

    FixedRectangle ref expand( Planc cref in_d_width_and_height ) { return( expand( in_d_width_and_height, in_d_width_and_height ) ); }
    FixedRectangle ref shrink( Planc cref in_d_width_and_height ) { return( shrink( in_d_width_and_height, in_d_width_and_height ) ); }

    FixedRectangle ref expand( Planc cref in_x, Planc cref in_y ) { expand_width( in_x ); expand_height( in_y ); rethis; }
    FixedRectangle ref shrink( Planc cref in_x, Planc cref in_y ) { shrink_width( in_x ); shrink_height( in_y ); rethis; }

    FixedRectangle ref expand_width( Planc cref in_p ) { Assert( ( !is_neg( in_p ) ), "cannot expand by negative amount, use shrink instead" ); return( width( width( ) + in_p ) ); }
    FixedRectangle ref shrink_width( Planc cref in_p ) { Assert( ( !is_neg( in_p ) ), "cannot shrink by negative amount, use expand instead" ); return( width( max( P0, width( ) - in_p ) ) ); }

    FixedRectangle ref expand_height( Planc cref in_p ) { Assert( ( !is_neg( in_p ) ), "cannot expand by negative amount, use shrink instead" ); return( height( height( ) + in_p ) ); }
    FixedRectangle ref shrink_height( Planc cref in_p ) { Assert( ( !is_neg( in_p ) ), "cannot shrink by negative amount, use expand instead" ); return( height( max( P0, height( ) - in_p ) ) ); }

    FixedRectangle ref operator+=( Vector cref in_vec ) { return( center( center( ) + in_vec ) ); }
    FixedRectangle ref operator-=( Vector cref in_vec ) { return( center( center( ) - in_vec ) ); }

    FixedRectangle ref operator*=( Planc cref in_p ) { width( width( ) * in_p ); height( height( ) * in_p ); rethis; }
    FixedRectangle ref operator/=( Planc cref in_p ) { width( width( ) / in_p ); height( height( ) / in_p ); rethis; }

    FixedRectangle operator+( Vector cref in_vec ) const { return( FixedRectangle( width( ), height( ), center( ) + in_vec ) ); }
    FixedRectangle operator-( Vector cref in_vec ) const { return( FixedRectangle( width( ), height( ), center( ) - in_vec ) ); }

    FixedRectangle operator*( Planc cref in_p ) const { return( FixedRectangle( ( width( ) * in_p ), ( height( ) * in_p ), center( ) ) ); }
    FixedRectangle operator/( Planc cref in_p ) const { return( FixedRectangle( ( width( ) / in_p ), ( height( ) / in_p ), center( ) ) ); }

    default_equal( FixedRectangle );

};

} // namespace geometry
} // namespace axn

#endif /* FixedRectangle_hpp */
