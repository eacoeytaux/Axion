#ifndef Circle_hpp
#define Circle_hpp

#include "axn/geometry/Planc.hpp"
#include "axn/geometry/Point.hpp"
#include "axn/geometry/Angle.hpp"
#include "axn/geometry/Vector.hpp"
#include "axn/geometry/Line.hpp"
#include "axn/geometry/Path.hpp"
#include "axn/geometry/Polygon.hpp"
#include "axn/geometry/Transform.hpp"

namespace axn
{
namespace geometry
{

class Circle
{

private:

    Planc m_radius;
    Point m_center;

public:

    Circle( Planc cref in_radius = 1.0, Point cref in_center = ORIGIN ) : m_radius( in_radius ), m_center( in_center ) { }

    Planc radius( ) const { return( m_radius ); }
    Planc diameter( ) const { return( x2( m_radius ) ); }

    void radius( dec in_radius ) { Assert( is_pos( in_radius ) ); m_radius = in_radius; }
    void diameter( dec in_diameter ) { Assert( is_pos( in_diameter ) ); m_radius = half( in_diameter ); }

    Point cref center( ) const { return( m_center ); }

    void center( Point cref in_center ) { m_center = in_center; }

    Planc area( ) const { return area( radius( ) ); }
    Planc circumference( ) const { return circumference( radius( ) ); }

    static Planc area( Planc cref in_radius ) { return( square( in_radius ) * PI ); }
    static Planc circumference( Planc cref in_radius ) { return( x2( in_radius ) * PI ); }

    bool contains( Point cref in_point, bool in_inclusive = true ) const { return( center( ).in_distance_range( in_point, radius( ), in_inclusive ) ); }

    bool tangent( Circle cref in_circle ) const { Planc distance = center( ).distance_to( in_circle.center( ) ); return( ( distance == abs( radius( ) + in_circle.radius( ) ) ) || ( distance == abs( radius( ) - in_circle.radius( ) ) ) ); }

    bool overlapped( Circle cref in_circle ) const { return( ( distance( center( ), in_circle.center( ) ) + radius( ) ) <= in_circle.radius( ) ); }
    bool overlaps( Circle cref in_circle ) const { return( in_circle.overlapped( *this ) ); }

    bool intersects( Circle cref in_circle, bool in_inclusive = true ) const { return( in_distance_range( center( ), in_circle.center( ), radius( ) + in_circle.radius( ), in_inclusive ) ); }

    varray<Point> intersection( Circle cref in_circle ) const
    {
        return_if( overlaps( in_circle ) || overlapped( in_circle ), { } );

        Vector v( center( ), in_circle.center( ) );
        Planc d( radius( ) + in_circle.radius( ) );
        Planc m( v.magnitude( ) );

        if( m == d )
        {
            return{ center( ) + VectorA( Angle( center( ), in_circle.center( ) ), radius( ) ) };
        }
        else if( m < d )
        {
            Angle a( acos( ( square( m ) + square( radius( ) ) - square( in_circle.radius( ) ) ) / x2( m * radius( ) ) ) );

            return{ center( ) + VectorA( v.angle( ) + a, radius( ) ),
                    center( ) + VectorA( v.angle( ) - a, radius( ) ) };
        }
        else
        {
            return{ };
        }
    }

    bool tangent( Line cref in_line ) const { return( center( ).distance_to( in_line.closest( center( ), false ) ) == radius( ) ); }

    bool intersects( Line cref in_line, bool in_inclusive = true ) const { return( contains( in_line.closest( center( ), true ), in_inclusive ) ); }

    Line intersection( Line cref in_line ) const
    {
        if( is_zero( radius( ) ) )
        {
            return_if( in_line.on( center( ), true ), Line( center( ), center( ) ) );
        }
        else
        {
            bool b1 = contains( in_line.pt1( ) );
            bool b2 = contains( in_line.pt2( ) );

            return_if( ( b1 && b2 ), in_line );

            Point closest_pt = in_line.closest( center( ), false );
            Planc closest_distance = center( ).distance_to( closest_pt );

            return_if( closest_distance == radius( ), Line( closest_pt, closest_pt ) );

            if( closest_distance < radius( ) )
            {
                Point pt1 = center( );
                Point pt2 = center( );

                if( is_zero( closest_distance ) )
                {
                    pt1 += VectorA( in_line.angle( ), radius( ) );
                    pt2 -= VectorA( in_line.angle( ), radius( ) );
                }
                else
                {
                    Angle a( center( ), closest_pt );
                    Angle d_a( acos( closest_distance / radius( ) ) );

                    pt1 += VectorA( a + d_a, radius( ) );
                    pt2 += VectorA( a - d_a, radius( ) );
                }

                if( b1 || b2 )
                {
                    if( in_line.on( pt1, true ) )
                    {
                        Assert( !in_line.on( pt2, true ) );

                        return_if( b1, Line( in_line.pt1( ), pt1 ) );
                        return_if( b2, Line( pt1, in_line.pt2( ) ) );
                    }
                    else if( in_line.on( pt2, true ) )
                    {
                        Assert( !in_line.on( pt1, true ) );

                        return_if( b1, Line( in_line.pt1( ), pt2 ) );
                        return_if( b2, Line( pt2, in_line.pt2( ) ) );
                    }

                    Assert( "bad circle intersection" );
                }
                else
                {
                    return( in_line.pt1( ).distance_to( pt1 ) <= in_line.pt2( ).distance_to( pt1 ) ?
                            Line( pt1, pt2 ) : Line( pt2, pt1 ) );
                }
            }
        }

        return( Line( ) );
    }

    Polygon polygon( ) const { return( Polygon::circle( radius( ), center( ) ) ); }

    operator Polygon( ) const { return( polygon( ) ); }

    Circle ref operator+=( Vector cref in_vec ) { center( center( ) + in_vec ); rethis; }
    Circle ref operator-=( Vector cref in_vec ) { center( center( ) - in_vec ); rethis; }

    Circle ref operator*=( Planc cref in_p ) { radius( radius( ) * in_p ); rethis; }
    Circle ref operator/=( Planc cref in_p ) { radius( radius( ) / in_p ); rethis; }

    Circle operator+( Vector cref in_vec ) const { return( Circle( radius( ), center( ) + in_vec ) ); }
    Circle operator-( Vector cref in_vec ) const { return( Circle( radius( ), center( ) - in_vec ) ); }

    Circle operator*( Planc cref in_p ) const { return( Circle( radius( ) * in_p, center( ) ) ); }
    Circle operator/( Planc cref in_p ) const { return( Circle( radius( ) / in_p, center( ) ) ); }

    default_equal( Circle );

};

} // namespace geometry
} // namespace axn

#endif /* Circle_hpp */