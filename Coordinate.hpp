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
class Line;

class Coordinate
{
public:
    virtual ~Coordinate( ) { }

    Coordinate( );
    Coordinate( const Planc & x, const Planc & y );
    Coordinate( const Vector & v );

    Planc x( ) const;
    Coordinate & x( const Planc & x );
    Planc y( ) const;
    Coordinate & y( const Planc & y );
    Coordinate & xy( const Planc & x, const Planc & y );

    Planc distance_to_origin( ) const { return distance_to( Coordinate( ) ); }
    Planc distance_to( const Coordinate & coordinate ) const;
    bool in_distance_range( const Coordinate & coordinate, Planc distance, bool inclusive = true ) const;

    Coordinate & move( const Planc &, const Planc & );
    Coordinate & move( const Coordinate & );
    Coordinate & rotate( const Angle & angle, const Coordinate & origin = Coordinate( 0, 0 ) );
    Coordinate & mirror( const Vector & axis );
    Coordinate & mirror( const Line & axis );
    Coordinate & mirror_x( );
    Coordinate & mirror_y( );

    Quadrant quadrant( ) const;
    Axis axis( ) const;

    bool in_quadrant( const Quadrant & quadrant ) const;
    bool on_axis( const Axis & axis ) const;

    Coordinate operator+( const Vector & vector ) const;
    Coordinate operator-( const Vector & vector ) const;

    Coordinate & operator+=( const Vector & vector );
    Coordinate & operator-=( const Vector & vector );

    default_equal( Coordinate );

private:
    Planc m_x = ZERO;
    Planc m_y = ZERO;
};

inline Planc distance( const Coordinate & c1, const Coordinate & c2 )
{
    return c1.distance_to( c2 );
}

inline bool in_distance_range( const Coordinate & c1, const Coordinate & c2, const Planc & distance, bool inclusive = true )
{
    return c1.in_distance_range( c2, distance, inclusive );
}

inline Coordinate midpoint( const Coordinate & c1, const Coordinate & c2 )
{
    Planc dx = c2.x( ) - c1.x( );
    Planc dy = c2.y( ) - c1.y( );
    return Coordinate( c1.x( ) + half( dx ), c1.y( ) + half( dy ) );
}

const Coordinate ORIGIN( ZERO, ZERO );
const Coordinate COORDINATE_INFINITY_POSITIVE( INFINITY_POSITIVE, INFINITY_POSITIVE );
const Coordinate COORDINATE_INFINITY_NEGATIVE( INFINITY_NEGATIVE, INFINITY_NEGATIVE );

} // namespace geometry
} // namespace axn

#endif /* Coordinate_hpp */
