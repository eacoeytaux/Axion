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
    Coordinate( const Planc & x = 0.0, const Planc & y = 0.0 );
    Coordinate( const Vector & v );

    Planc x( ) const;
    Coordinate & x( const Planc & x );
    Planc y( ) const;
    Coordinate & y( const Planc & y );
    Coordinate & xy( const Planc & x, const Planc & y );

    Planc distance( const Coordinate & coordinate ) const;

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

    bool operator==( const Coordinate & coordinate ) const;
    bool operator!=( const Coordinate & coordinate ) const;

private:
    Planc m_x = 0.0;
    Planc m_y = 0.0;
};

const Coordinate ORIGIN( 0.0, 0.0 );
const Coordinate COORDINATE_INFINITY_POSITIVE( INFINITY_POSITIVE, INFINITY_POSITIVE );
const Coordinate COORDINATE_INFINITY_NEGATIVE( INFINITY_NEGATIVE, INFINITY_NEGATIVE );

} // namespace geometry
} // namespace axn

#endif /* Coordinate_hpp */
