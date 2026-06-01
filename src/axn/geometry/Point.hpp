#ifndef Point_hpp
#define Point_hpp

#include "axn/geometry/Planc.hpp"

namespace axn
{
namespace geometry
{

enum Axis
{
    NO_AXIS = 0, // origin or quadrant
    X_AXIS = 1,
    Y_AXIS = 2,
    AXES
};

enum Quadrant
{
    NO_QUADRANT = 0, // origin or axes
    Q1 = 1,
    Q2 = 2,
    Q3 = 3,
    Q4 = 4,
    QUADRANTS
};

class Angle;
class Vector;
class Transform;

class Point
{

private:

    Planc m_x = P0;
    Planc m_y = P0;

public:

    Point( ) { }

    Point( Planc cref in_x, Planc cref in_y ) : m_x( in_x ), m_y( in_y ) { }

    Point( Vector cref in_v );

    Planc cref x( ) const { return( m_x ); }
    Planc cref y( ) const { return( m_y ); }

    Point ref x( Planc cref in_x ) { m_x = in_x; rethis; }
    Point ref y( Planc cref in_y ) { m_y = in_y; rethis; }

    Planc distance_to_origin( ) const { return( pythagorean( x( ), y( ) ) ); }
    Planc distance_to( Point cref in_pt ) const { return( pythagorean( x( ) - in_pt.x( ), y( ) - in_pt.y( ) ) ); }
    bool in_distance_range( Point cref in_pt, Planc cref in_distance, bool in_inclusive = true ) const { return( in_distance_xy_range( in_pt, in_distance, in_inclusive ) && in_range( distance_to( in_pt ), in_distance, in_inclusive ) ); }

    Planc distance_xy_to_origin( ) const { return( abs( x( ) ) + abs( y( ) ) ); }
    Planc distance_xy_to( Point cref in_pt ) const { return( abs( x( ) - in_pt.x( ) ) + abs( y( ) - in_pt.y( ) ) ); }
    bool in_distance_xy_range( Point cref in_pt, Planc cref in_distance, bool in_inclusive = true ) const { return( in_range( distance_xy_to( in_pt ), in_distance, in_inclusive ) ); }

    bool closer_than( Point cref in_pt1, Point cref in_pt2 ) const { return( ( ( abs( x( ) - in_pt1.x( ) ) + abs( y( ) - in_pt1.y( ) ) ) < ( abs( x( ) - in_pt2.x( ) ) + abs( y( ) - in_pt2.y( ) ) ) ) || ( distance_to( in_pt1 ) < distance_to( in_pt2 ) ) ); }
    bool further_than( Point cref in_pt1, Point cref in_pt2 ) const { return( ( ( abs( x( ) - in_pt1.x( ) ) + abs( y( ) - in_pt1.y( ) ) ) > ( abs( x( ) - in_pt2.x( ) ) + abs( y( ) - in_pt2.y( ) ) ) ) || ( distance_to( in_pt1 ) > distance_to( in_pt2 ) ) ); }

    Axis axis( ) const { return( x( ) ? ( y( ) ? NO_AXIS : X_AXIS ) : ( y( ) ? Y_AXIS : NO_AXIS ) ); }
    bool on_axis( Axis cref in_axis ) const { return( axis( ) == in_axis ); }

    Quadrant quadrant( ) const { return( !x( ) || !y( ) ) ? NO_QUADRANT : ( is_pos( x( ) ) ? ( is_pos( y( ) ) ? Q1 : Q4 ) : ( is_pos( y( ) ) ? Q2 : Q3 ) ); }
    bool in_quadrant( Quadrant cref in_quadrant, bool in_inclusive = false ) const
    {
        if( in_inclusive )
        {
            return_true_if( is_zero( x( ) ) && is_zero( y( ) ) );

            if( is_zero( x( ) ) )
            {
                if( is_pos( y( ) ) )
                {
                    return_true_if( ( in_quadrant == Q1 ) || ( in_quadrant == Q2 ) );
                }
                else
                {
                    return_true_if( ( in_quadrant == Q3 ) || ( in_quadrant == Q4 ) );
                }
            }
            else if( is_zero( y( ) ) )
            {
                if( is_pos( x( ) ) )
                {
                    return_true_if( ( in_quadrant == Q1 ) || ( in_quadrant == Q4 ) );
                }
                else
                {
                    return_true_if( ( in_quadrant == Q2 ) || ( in_quadrant == Q3 ) );
                }
            }
        }

        return( quadrant( ) == in_quadrant );
    }

    Point ref transform( Transform cref in_trans );

    Point ref move( Planc cref in_dx, Planc cref in_dy );
    Point ref rotate( Angle cref in_angle, Point cref in_origin = Point( 0.0, 0.0 ) );
    Point ref mirror( Vector cref in_axis );
    Point ref mirror_x( );
    Point ref mirror_y( );

    Point ref operator+=( Vector cref in_v );
    Point ref operator-=( Vector cref in_v );

    Point ref operator*=( dec in_d ) { x( x( ) * in_d ); y( y( ) * in_d ); rethis; }
    Point ref operator/=( dec in_d ) { x( x( ) / in_d ); y( y( ) / in_d ); rethis; }

    Point operator+( ) const { return( Point( +x( ), +y( ) ) ); }
    Point operator-( ) const { return( Point( -x( ), -y( ) ) ); }

    Point operator+( Vector cref in_v ) const;
    Point operator-( Vector cref in_v ) const;

    Point operator*( dec in_d ) const { return( Point( ( x( ) * in_d ), ( y( ) * in_d ) ) ); }
    Point operator/( dec in_d ) const { return( Point( ( x( ) / in_d ), ( y( ) / in_d ) ) ); }

    bool operator<=( Point cref in_pt ) const { return( ( x( ) <= in_pt.x( ) ) || ( y( ) <= in_pt.y( ) ) ); }
    bool operator>=( Point cref in_pt ) const { return( ( x( ) >= in_pt.x( ) ) || ( y( ) >= in_pt.y( ) ) ); }
    bool operator<( Point cref in_pt ) const { return( ( x( ) < in_pt.x( ) ) || ( y( ) < in_pt.y( ) ) ); }
    bool operator>( Point cref in_pt ) const { return( ( x( ) > in_pt.x( ) ) || ( y( ) > in_pt.y( ) ) ); }

    default_equal( Point );

};

var_const( Point ORIGIN( 0.0, 0.0 ) );
var_const( Point INVALID_POINT( INFINITY, INFINITY ) );

inline Point PointX( Planc cref in_x ) { return( Point( in_x, P0 ) ); }
inline Point PointY( Planc cref in_y ) { return( Point( P0, in_y ) ); }

inline Point midpoint( Point cref in_pt1, Point cref in_pt2 ) { return( Point( in_pt1.x( ) + half( in_pt2.x( ) - in_pt1.x( ) ), in_pt1.y( ) + half( in_pt2.y( ) - in_pt1.y( ) ) ) ); }

inline Planc distance( Point cref in_pt1, Point cref in_pt2 ) { return( in_pt1.distance_to( in_pt2 ) ); }
inline bool in_distance_range( Point cref in_pt1, Point cref in_pt2, Planc cref in_distance, bool in_inclusive = true ) { return( in_pt1.in_distance_range( in_pt2, in_distance, in_inclusive ) ); }

inline Planc cross( Point cref in_pt1, Point cref in_pt2, Point cref in_pt3 ) { return( ( ( in_pt3.y( ) - in_pt1.y( ) ) * ( in_pt2.x( ) - in_pt1.x( ) ) ) - ( ( in_pt3.x( ) - in_pt1.x( ) ) * ( in_pt2.y( ) - in_pt1.y( ) ) ) ); }

} // namespace geometry
} // namespace axn

#endif /* Point_hpp */
