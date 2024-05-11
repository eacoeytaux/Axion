#ifndef Angle_hpp
#define Angle_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"

namespace axn
{
namespace geometry
{

const bool TRUNCATE_ANGLE_DEFAULT = false;

class Angle
{
public:
    virtual ~Angle( ) { }
    Angle( double radians = 0, bool truncate = TRUNCATE_ANGLE_DEFAULT );
    Angle( const Planc & dx, const Planc & dy, bool truncate = TRUNCATE_ANGLE_DEFAULT );
    Angle( const Coordinate & c, bool truncate = TRUNCATE_ANGLE_DEFAULT );
    Angle( const Coordinate & from, const Coordinate & to, bool truncate = TRUNCATE_ANGLE_DEFAULT );

    bool truncating( ) const;
    Angle & truncate( bool truncate );
    Angle truncated( );

    double radians( ) const;
    double degrees( ) const;

    Angle & radians( double radians );
    Angle & radians( const Planc & dx, const Planc & dy );

    double sin( double multiplier = 1.0 ) const;
    double cos( double multiplier = 1.0 ) const;

    Quadrant quadrant( ) const;
    Axis axis( ) const;
    bool in_quadrant( Quadrant quadrant ) const;
    bool on_axis( Axis axis ) const;

    Angle & flip( );
    Angle flipped( ) const;

    Angle half( ) const;
    Angle & halve( );

    operator double( ) const { return m_radians; }

    Angle & operator=( double radians );

    Angle operator-( ) const;

    Angle operator+( const Angle & angle ) const;
    Angle operator-( const Angle & angle ) const;

    Angle & operator+=( const Angle & angle );
    Angle & operator-=( const Angle & angle );

    bool operator==( const Angle & angle ) const;
    bool operator!=( const Angle & angle ) const;
    bool operator<( const Angle & angle ) const;
    bool operator<=( const Angle & angle ) const;
    bool operator>( const Angle & angle ) const;
    bool operator>=( const Angle & angle ) const;

    Angle operator+( double radians ) const;
    Angle operator-( double radians ) const;
    Angle operator*( double scale ) const;
    Angle operator/( double scale ) const;

    Angle & operator+=( double radians );
    Angle & operator-=( double radians );
    Angle & operator*=( double scale );
    Angle & operator/=( double scale );

    bool operator==( double radians ) const;
    bool operator!=( double radians ) const;
    bool operator<( double radians ) const;
    bool operator<=( double radians ) const;
    bool operator>( double radians ) const;
    bool operator>=( double radians ) const;

    Angle operator+( int radians ) const;
    Angle operator-( int radians ) const;
    Angle operator*( int scale ) const;
    Angle operator/( int scale ) const;

    Angle & operator+=( int radians );
    Angle & operator-=( int radians );
    Angle & operator*=( int scale );
    Angle & operator/=( int scale );

    bool operator==( int radians ) const;
    bool operator!=( int radians ) const;
    bool operator<( int radians ) const;
    bool operator<=( int radians ) const;
    bool operator>( int radians ) const;
    bool operator>=( int radians ) const;

    Angle operator+( uint radians ) const;
    Angle operator-( uint radians ) const;
    Angle operator*( uint scale ) const;
    Angle operator/( uint scale ) const;

    Angle & operator+=( uint radians );
    Angle & operator-=( uint radians );
    Angle & operator*=( uint scale );
    Angle & operator/=( uint scale );

    bool operator==( uint radians ) const;
    bool operator!=( uint radians ) const;
    bool operator<( uint radians ) const;
    bool operator<=( uint radians ) const;
    bool operator>( uint radians ) const;
    bool operator>=( uint radians ) const;

    static double radians_to_degrees( double radians );
    static double degrees_to_radians( double degrees );

private:
    bool m_truncate = TRUNCATE_ANGLE_DEFAULT; // keeps radians within [ 0, 2 * pi )

    double m_radians = 0.0;
#ifdef AXN_DEBUG
    double m_degrees = 0.0;
#endif
};

const double TAU = 6.283185307179586476925;
const double PI = 3.141592653589793236926;
const double RIGHT_ANGLE = 1.570796326794896618463;
const double RIGHT_ANGLE_0 = RIGHT_ANGLE * 0.0;
const double RIGHT_ANGLE_1 = RIGHT_ANGLE * 1.0;
const double RIGHT_ANGLE_2 = RIGHT_ANGLE * 2.0;
const double RIGHT_ANGLE_3 = RIGHT_ANGLE * 3.0;

} // namespace geometry
} // namespace axn

#endif /* Angle_hpp */
