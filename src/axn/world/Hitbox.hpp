#ifndef Hitbox_hpp
#define Hitbox_hpp

#include "axn/geometry/Geometry.hpp"

namespace axn
{
namespace reality
{

class Hitbox
{

private:

    Point m_position = Point( );
    Polygon m_poly = Polygon( );

public:

    Hitbox( ) { }

    Hitbox( Polygon cref in_hitbox, Point cref in_position = ORIGIN ) : m_poly( in_hitbox ), m_position( in_position ) { Assert( is_zero( m_poly.sides( ) ) || ( m_poly.convex( ) && m_poly.contains( ORIGIN ) ) ); }

    Point cref position( ) const { return( m_position ); }
    Polygon polygon( bool in_positioned = true ) const { return( in_positioned ? ( m_poly + m_position ) : m_poly ); }
    FixedRectangle bounds( bool in_positioned = true ) const { return( in_positioned ? ( m_poly.bounds( ) + Vector( m_position ) ) : m_poly.bounds( ) ); } // C! the polygon's own box moved, rather than a moved copy of the polygon

    Point offset( Angle cref in_angle ) const { return( polygon( ).intersection( Line( position( ), VectorA( in_angle, pythagorean( polygon( ).bound_width( ), polygon( ).bound_height( ) ) ) ) ).front( ).pt2( ) ); }

    Hitbox ref expand( Hitbox cref in_hitbox ) { rethis = expanded( in_hitbox ); }
    Hitbox expanded( Hitbox cref in_hitbox ) const
    {
        if( in_hitbox.m_poly.sides( ) ) // C! its own polygon read directly, not a copy of it
        {
            Polygon inverted = in_hitbox.m_poly.inverted( );
            varray<Point> cref inverted_pts = inverted.points( );

            varray<Point> pt_cloud( inverted_pts );

            for_each_const( pt, m_poly.points( ) ) // C! was polygon( false ).points( ), the points of a temporary copy that's gone before the loop runs
            {
                for_each_const( inverted_pt, inverted_pts ) { pt_cloud.insert_back( inverted_pt + Vector( pt ) ); } // C! the inverted polygon moved by pt, without building a polygon for every point
            }

            return( Hitbox( Polygon::convex_from_cloud( pt_cloud ), position( ) ) );
        }
        else
        {
            rethis;
        }
    }

    operator Polygon( ) const { return( polygon( ) ); }

    equal_default( Hitbox );

};

} // namespace reality
} // namespace axn

#endif /* Hitbox_hpp */
