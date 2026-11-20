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
    FixedRectangle ref width( Planc cref in_width ) { Assert( ( !is_neg( in_width ) ), "cannot have negative width" ); m_width = in_width; rethis; }

    Planc cref height( ) const { return( m_height ); }
    FixedRectangle ref height( Planc cref in_height ) { Assert( ( !is_neg( in_height ) ), "cannot have negative height" ); m_height = in_height; rethis; }

    Planc area( ) const { return( width( ) * height( ) ); }

    varray<Point> points( ) const { return{ top_right( ), top_left( ), bottom_left( ), bottom_right( ) }; }

    Path perimeter( ) const { return( Path( { top( ), left( ), bottom( ), right( ) } ) ); }

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
        // C! the part of the line inside the rectangle (edges included), clipped against each side in turn (Liang-Barsky)
        // C! works for a rectangle with no width or height too, where the result is a point or a piece of a line
        Vector direction = Vector( in_line.pt1( ), in_line.pt2( ) );

        dec dx = dec( direction.dx( ) );
        dec dy = dec( direction.dy( ) );

        dec x1 = dec( in_line.pt1( ).x( ) );
        dec y1 = dec( in_line.pt1( ).y( ) );

        dec t_min = 0.0;
        dec t_max = 1.0;

        // C! each side as ( p, q ): the line is inside that side where t * p <= q
        varray<dec> ps = { -dx, +dx, -dy, +dy };
        varray<dec> qs = { ( x1 - dec( lower_bound_x( ) ) ), ( dec( upper_bound_x( ) ) - x1 ), ( y1 - dec( lower_bound_y( ) ) ), ( dec( upper_bound_y( ) ) - y1 ) };

        for_range( i, ps.size( ) )
        {
            if( is_zero( ps[ i ] ) )
            {
                return_if( ( qs[ i ] < -DEFAULT_EPSILON ), varray<Line>( ) ); // C! parallel to this side and outside it
            }
            else
            {
                dec t = ( qs[ i ] / ps[ i ] );

                if( is_neg( ps[ i ] ) )
                {
                    t_min = max( t_min, t );
                }
                else
                {
                    t_max = min( t_max, t );
                }
            }
        }

        return_if( ( t_min > ( t_max + DEFAULT_EPSILON ) ), varray<Line>( ) );

        t_max = max( t_min, t_max );

        return{ Line( ( in_line.pt1( ) + ( direction * t_min ) ), ( in_line.pt1( ) + ( direction * t_max ) ) ) };
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

    equal_default( FixedRectangle );

};

} // namespace geometry
} // namespace axn

#endif /* FixedRectangle_hpp */
