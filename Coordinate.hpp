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

class Coordinate
{
private:
    Planc m_x = P0;
    Planc m_y = P0;

public:
    Coordinate( ) { }

    Coordinate( const Planc & x, const Planc & y ) : m_x( x ), m_y( y ) { }

    Coordinate( const Vector & );

    const Planc & x( ) const { return m_x; }
    const Planc & y( ) const { return m_y; }

    Coordinate & x( const Planc & x ) { m_x = x; rethis; }
    Coordinate & y( const Planc & y ) { m_y = y; rethis; }

    Planc distance_to_origin( ) const { return pythagorean( x( ), y( ) ); }
    Planc distance_to( const Coordinate & c ) const { return pythagorean( x( ) - c.x( ), y( ) - c.y( ) ); }
    bool in_distance_range( const Coordinate & c, const Planc & distance, bool inclusive = true ) const { return in_range( distance_to( c ), distance, inclusive ); }

    Quadrant quadrant( ) const { return ( !x( ) || !y( ) ) ? No_Quadrant : ( is_pos( x( ) ) ? ( is_pos( y( ) ) ? Q1 : Q4 ) : ( is_pos( y( ) ) ? Q2 : Q3 ) ); }
    bool in_quadrant( const Quadrant & q ) const { return ( quadrant( ) == q ); }

    Axis axis( ) const { return ( x( ) ? ( y( ) ? No_Axis : X_Axis ) : ( y( ) ? Y_Axis : No_Axis ) ); }
    bool on_axis( const Axis & a ) const { return ( axis( ) == a ); }

    Coordinate & transform( const Transform & );

    Coordinate & move( const Planc & dx, const Planc & dy );
    Coordinate & rotate( const Angle & angle );
    Coordinate & rotate( const Angle & angle, const Coordinate & origin );
    Coordinate & mirror( const Vector & axis );
    Coordinate & mirror_x( );
    Coordinate & mirror_y( );

    Coordinate operator-( ) const { return Coordinate( -x( ), -y( ) ); }

    Coordinate operator*( dec d ) const { return Coordinate( x( ) * d, y( ) * d ); }
    Coordinate operator/( dec d ) const { return Coordinate( x( ) / d, y( ) / d ); }

    Coordinate & operator*=( dec d ) { x( x( ) * d ); y( y( ) * d ); rethis; }
    Coordinate & operator/=( dec d ) { x( x( ) / d ); y( y( ) / d ); rethis; }

    Coordinate operator+( const Vector & ) const;
    Coordinate operator-( const Vector & v ) const;

    Coordinate & operator+=( const Vector & );
    Coordinate & operator-=( const Vector & );

    default_equal( Coordinate );
};

const Coordinate ORIGIN( ZERO, ZERO );
const Coordinate INVALID_COORDINATE( INFINITY_NEG, INFINITY_NEG );

inline Coordinate midpoint( const Coordinate & c1, const Coordinate & c2 ) { return Coordinate( c1.x( ) + half( c2.x( ) - c1.x( ) ), c1.y( ) + half( c2.y( ) - c1.y( ) ) ); }

inline Planc distance( const Coordinate & c1, const Coordinate & c2 ) { return c1.distance_to( c2 ); }
inline bool in_distance_range( const Coordinate & c1, const Coordinate & c2, const Planc & distance, bool inclusive = true ) { return c1.in_distance_range( c2, distance, inclusive ); }

inline Planc cross( const Coordinate & c1, const Coordinate & c2, const Coordinate & c3 ) { return ( ( ( c3.y( ) - c1.y( ) ) * ( c2.x( ) - c1.x( ) ) ) - ( ( c3.x( ) - c1.x( ) ) * ( c2.y( ) - c1.y( ) ) ) ); }

inline bool contains( const Coordinate & c1, const Coordinate & c2, const Coordinate & c3, const Coordinate & c, bool inclusive = true )
{
    dec a = 0.5 / half( ( c1.x( ) * ( c2.y( ) - c3.y( ) ) ) + ( c1.y( ) * ( c3.x( ) - c2.x( ) ) ) + ( c2.x( ) * c3.y( ) ) + ( c3.x( ) * -c2.y( ) ) );

    dec s = a * ( ( c1.y( ) * c3.x( ) ) - ( c1.x( ) * c3.y( ) ) + ( ( c3.y( ) - c1.y( ) ) * c.x( ) ) + ( ( c1.x( ) - c3.x( ) ) * c.y( ) ) );
    return_if( false, less( !inclusive, s, ZERO ) );

    dec t = a * ( ( c1.x( ) * c2.y( ) ) - ( c1.y( ) * c2.x( ) ) + ( ( c1.y( ) - c2.y( ) ) * c.x( ) ) + ( ( c2.x( ) - c1.x( ) ) * c.y( ) ) );
    return_if( false, less( !inclusive, t, ZERO ) );

    return !less( !inclusive, ( 1.0 - s - t ), ZERO );
}

} // namespace geometry
} // namespace axn

#endif /* Coordinate_hpp */
