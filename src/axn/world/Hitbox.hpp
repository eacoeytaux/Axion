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
    Polygon m_polygon = Polygon( );

public:

    Hitbox( ) { }

    Hitbox( Polygon cref in_hitbox, Point cref in_position = ORIGIN ) : m_polygon( in_hitbox ), m_position( in_position ) { Assert( is_zero( m_polygon.sides( ) ) || ( m_polygon.convex( ) && m_polygon.contains( ORIGIN ) ) ); }

    Point position( ) const { return( m_position ); }
    Polygon polygon( bool in_positioned = true ) const { return( in_positioned ? ( m_polygon + m_position ) : m_polygon ); }
    FixedRectangle bounds( bool in_positioned = true ) const { return( polygon( in_positioned ).bounds( ) ); }

    Point offset( Angle cref in_angle ) const { return( polygon( ).intersection( Line( position( ), VectorA( in_angle, pythagorean( polygon( ).bound_width( ), polygon( ).bound_height( ) ) ) ) ).front( ).pt2( ) ); }

    Hitbox ref expand( Hitbox cref in_hitbox ) { *this = expanded( in_hitbox ); }
    Hitbox expanded( Hitbox cref in_hitbox ) const
    {
        if( in_hitbox.polygon( false ).sides( ) )
        {
            Polygon inverted = in_hitbox.polygon( false ).inverted( );

            varray<Point> pt_cloud( inverted.points( ) );

            for_each( pt, polygon( false ).points( ) )
            {
                pt_cloud.insert_back( ( inverted + pt ).points( ) );
            }

            return( Hitbox( Polygon::convex_from_cloud( pt_cloud ), position( ) ) );
        }
        else
        {
            rethis;
        }
    }

    operator Polygon( ) const { return( polygon( ) ); }

    default_equal( Hitbox );

};

} // namespace reality
} // namespace axn

#endif /* Hitbox_hpp */
