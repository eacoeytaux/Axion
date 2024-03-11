#ifndef Coordinate_hpp
#define Coordinate_hpp

#include "axn.hpp"
#include "Planc.hpp"

namespace axn {
namespace geometry {

class Angle;
class Vector;

class Coordinate {
public:
    virtual ~Coordinate( ) {}
    Coordinate( const planc & x = 0.f, const planc & y = 0.f );
    Coordinate( const Vector & v );

    planc x( ) const;
    Coordinate & x( const planc & x );
    planc y( ) const;
    Coordinate & y( const planc & y );
    Coordinate & xy( const planc & x, const planc & y );

    planc distance( const Coordinate & coordinate ) const;

    Coordinate & rotate( const Angle & angle, const Coordinate & origin = Coordinate( 0, 0 ) );
    Coordinate & mirror( const Vector & axis ); // TODO change to line?
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
    planc m_x = 0.f;
    planc m_y = 0.f;
};

const Coordinate ORIGIN( (planc)0.f, (planc)0.f );
const Coordinate COORDINATE_POSITIVE_INFINITY( (planc)POSITIVE_INFINITY,
                                               (planc)POSITIVE_INFINITY );
const Coordinate COORDINATE_NEGATIVE_INFINITY( (planc)NEGATIVE_INFINITY,
                                               (planc)NEGATIVE_INFINITY );

} // namespace geometry
} // namespace axn

#endif /* Coordinate_hpp */
