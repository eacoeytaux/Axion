#ifndef Trapezoid_hpp
#define Trapezoid_hpp

#include "axn/geometry/Planc.hpp"
#include "axn/geometry/Point.hpp"
#include "axn/geometry/Angle.hpp"
#include "axn/geometry/Vector.hpp"
#include "axn/geometry/Polygon.hpp"

namespace axn
{
namespace geometry
{

// C! a base and a parallel top, both centered on the same line across them, height( ) apart: centered on center( ) (the origin unless it's given), with
// C! the base below and the top above, then turned by rotation( ) about its center. As a polygon, a zero width or height drops the points it would repeat:
// C! one zero width makes a triangle, a zero height or both widths zero a line, and everything zero a point
class Trapezoid : public Stringable, public Transformable
{

    strinfo( Trapezoid, "base: " + string( base_width( ) ) + " top: " + string( top_width( ) ) + " height: " + string( height( ) ) + " center: " + string( center( ) ) + " rotation: " + string( rotation( ) ) );

public:

    Trapezoid( Planc cref in_base_width, Planc cref in_top_width, Planc cref in_height, Angle cref in_rotation = A0 ) { base_width( in_base_width ); top_width( in_top_width ); height( in_height ); rotation( in_rotation ); }
    Trapezoid( Planc cref in_base_width, Planc cref in_top_width, Planc cref in_height, Point cref in_center, Angle cref in_rotation = A0 ) : Trapezoid( in_base_width, in_top_width, in_height, in_rotation ) { center( in_center ); }

    Planc cref base_width( ) const { return( m_base_width ); }
    Trapezoid ref base_width( Planc cref in_base_width ) { Assert( !is_neg( in_base_width ) ); m_base_width = in_base_width; rethis; }

    Planc cref top_width( ) const { return( m_top_width ); }
    Trapezoid ref top_width( Planc cref in_top_width ) { Assert( !is_neg( in_top_width ) ); m_top_width = in_top_width; rethis; }

    Planc cref height( ) const { return( m_height ); }
    Trapezoid ref height( Planc cref in_height ) { Assert( !is_neg( in_height ) ); m_height = in_height; rethis; }

    Point cref center( ) const { return( m_center ); }
    Trapezoid ref center( Point cref in_center ) { m_center = in_center; rethis; }

    Angle cref rotation( ) const { return( m_rotation ); }
    Trapezoid ref rotation( Angle cref in_rotation ) { m_rotation = in_rotation; rethis; }

    Planc area( ) const { return( half( base_width( ) + top_width( ) ) * height( ) ); }

    Polygon polygon( ) const
    {
        const bool c_flat = is_zero( height( ) );
        const bool c_no_base = is_zero( base_width( ) );
        const bool c_no_top = is_zero( top_width( ) );
        const Planc c_half_height = half( height( ) );

        varray<Point> pts; // C! around the origin, counterclockwise from the base's left end

        if( c_flat && c_no_base && c_no_top ) // C! a point
        {
            pts.insert_back( ORIGIN );
        }
        else if( c_flat ) // C! a line as wide as the wider of the two
        {
            const Planc c_half = half( max( base_width( ), top_width( ) ) );
            pts.insert_back( Point( -c_half, P0 ) );
            pts.insert_back( Point( c_half, P0 ) );
        }
        else
        {
            // C! with both widths zero, a line up the middle
            if( c_no_base ) { pts.insert_back( Point( P0, -c_half_height ) ); }
            else { pts.insert_back( Point( -half( base_width( ) ), -c_half_height ) ); pts.insert_back( Point( half( base_width( ) ), -c_half_height ) ); }

            if( c_no_top ) { pts.insert_back( Point( P0, c_half_height ) ); }
            else { pts.insert_back( Point( half( top_width( ) ), c_half_height ) ); pts.insert_back( Point( -half( top_width( ) ), c_half_height ) ); }
        }

        Polygon poly( pts );
        poly.rotate( rotation( ) ).move( Vector( ORIGIN, center( ) ) ).transform( transform( ) ); // C! turned and centered, then its own transform( ) from transform_functions

        return( poly );
    }

    operator Polygon( ) const { return( polygon( ) ); }

    transform_functions( Trapezoid );

private:

    Planc m_base_width = P0;
    Planc m_top_width = P0;
    Planc m_height = P0;
    Point m_center = ORIGIN;
    Angle m_rotation = A0;

};

} // namespace geometry
} // namespace axn

#endif /* Trapezoid_hpp */
