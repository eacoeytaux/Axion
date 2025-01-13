#ifndef Coordinate_hpp
#define Coordinate_hpp

#include "Planc.hpp"

namespace axn
{
namespace geometry
{

enum Quadrant
{
    No_Quadrant = -1, // origin or axes
    Q1 = 0,
    Q2 = 1,
    Q3 = 2,
    Q4 = 3,
    Quadrants
};

enum Axis
{
    No_Axis = -1, // origin or quadrant
    X_Axis = 0,
    Y_Axis = 1,
    Origin = 2,
    Axes
};

class Angle;
class Vector;
class Transform;

axnclass( Coordinate )
{
private:
    Planc m_x = P0;
    Planc m_y = P0;

public:
    Coordinate( ) { }

    Coordinate( Planc cref x, Planc cref y ) : m_x( x ), m_y( y ) { }

    Coordinate( Vector cref );

    Planc cref x( ) const { return m_x; }
    Planc cref y( ) const { return m_y; }

    Coordinate & x( Planc cref x ) { m_x = x; rethis; }
    Coordinate & y( Planc cref y ) { m_y = y; rethis; }

    Planc distance_to_origin( ) const { return pythagorean( x( ), y( ) ); }
    Planc distance_to( Coordinate cref c ) const { return pythagorean( x( ) - c.x( ), y( ) - c.y( ) ); }
    bool in_distance_range( Coordinate cref c, Planc cref distance, bool inclusive = true ) const { return in_range( distance_to( c ), distance, inclusive ); }

    bool closer_than( Coordinate cref c1, Coordinate cref c2 ) const { return ( ( ( abs( x( ) - c1.x( ) ) + abs( y( ) - c1.y( ) ) ) < ( abs( x( ) - c2.x( ) ) + abs( y( ) - c2.y( ) ) ) ) || ( distance_to( c1 ) < distance_to( c2 ) ) ); }
    bool further_than( Coordinate cref c1, Coordinate cref c2 ) const { return ( ( ( abs( x( ) - c1.x( ) ) + abs( y( ) - c1.y( ) ) ) > ( abs( x( ) - c2.x( ) ) + abs( y( ) - c2.y( ) ) ) ) || ( distance_to( c1 ) > distance_to( c2 ) ) ); }

    Quadrant quadrant( ) const { return ( !x( ) || !y( ) ) ? No_Quadrant : ( is_pos( x( ) ) ? ( is_pos( y( ) ) ? Q1 : Q4 ) : ( is_pos( y( ) ) ? Q2 : Q3 ) ); }
    bool in_quadrant( Quadrant q ) const { return ( quadrant( ) == q ); }

    Axis axis( ) const { return ( x( ) ? ( y( ) ? No_Axis : X_Axis ) : ( y( ) ? Y_Axis : No_Axis ) ); }
    bool on_axis( Axis a ) const { return ( axis( ) == a ); }

    Coordinate & transform( Transform cref );

    Coordinate & move( Planc cref dx, Planc cref dy );
    Coordinate & rotate( Angle cref angle );
    Coordinate & rotate( Angle cref angle, Coordinate cref origin );
    Coordinate & mirror( Vector cref axis );
    Coordinate & mirror_x( );
    Coordinate & mirror_y( );

    Coordinate operator-( ) const { return Coordinate( -x( ), -y( ) ); }

    Coordinate operator*( dec d ) const { return Coordinate( x( ) * d, y( ) * d ); }
    Coordinate operator/( dec d ) const { return Coordinate( x( ) / d, y( ) / d ); }

    Coordinate & operator*=( dec d ) { x( x( ) * d ); y( y( ) * d ); rethis; }
    Coordinate & operator/=( dec d ) { x( x( ) / d ); y( y( ) / d ); rethis; }

    Coordinate operator+( Vector cref ) const;
    Coordinate operator-( Vector cref v ) const;

    Coordinate & operator+=( Vector cref );
    Coordinate & operator-=( Vector cref );

    default_equal( Coordinate );
};

const Coordinate ORIGIN( 0.0, 0.0 );
const Coordinate INVALID_COORDINATE( INFINITY_NEG, INFINITY_NEG );

inline Planc distance( Coordinate cref c1, Coordinate cref c2 ) { return c1.distance_to( c2 ); }
inline bool in_distance_range( Coordinate cref c1, Coordinate cref c2, Planc cref distance, bool inclusive = true ) { return c1.in_distance_range( c2, distance, inclusive ); }

inline Planc cross( Coordinate cref c1, Coordinate cref c2, Coordinate cref c3 ) { return ( ( ( c3.y( ) - c1.y( ) ) * ( c2.x( ) - c1.x( ) ) ) - ( ( c3.x( ) - c1.x( ) ) * ( c2.y( ) - c1.y( ) ) ) ); }

inline Coordinate midpoint( Coordinate cref c1, Coordinate cref c2 ) { return Coordinate( c1.x( ) + half( c2.x( ) - c1.x( ) ), c1.y( ) + half( c2.y( ) - c1.y( ) ) ); }

inline bool contains( Coordinate cref c1, Coordinate cref c2, Coordinate cref c3, Coordinate cref c, bool inclusive = true )
{
    dec a = 0.5 / half( ( c1.x( ) * ( c2.y( ) - c3.y( ) ) ) + ( c1.y( ) * ( c3.x( ) - c2.x( ) ) ) + ( c2.x( ) * c3.y( ) ) + ( c3.x( ) * -c2.y( ) ) );

    dec s = a * ( ( c1.y( ) * c3.x( ) ) - ( c1.x( ) * c3.y( ) ) + ( ( c3.y( ) - c1.y( ) ) * c.x( ) ) + ( ( c1.x( ) - c3.x( ) ) * c.y( ) ) );
    return_false_if( less( !inclusive, s, 0.0 ) );

    dec t = a * ( ( c1.x( ) * c2.y( ) ) - ( c1.y( ) * c2.x( ) ) + ( ( c1.y( ) - c2.y( ) ) * c.x( ) ) + ( ( c2.x( ) - c1.x( ) ) * c.y( ) ) );
    return_false_if( less( !inclusive, t, 0.0 ) );

    return !less( !inclusive, ( 1.0 - s - t ), 0.0 );
}

} // namespace geometry
} // namespace axn

#endif /* Coordinate_hpp */
