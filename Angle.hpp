#ifndef Angle_hpp
#define Angle_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Utility.hpp"

namespace axn
{
namespace geometry
{

const bool TRUNCATE_ANGLE_DEFAULT = false;

class Angle
{
public:
    virtual ~Angle( ) { }
    Angle( dec radians = 0, bool truncate = TRUNCATE_ANGLE_DEFAULT );
    Angle( const Planc & dx, const Planc & dy, bool truncate = TRUNCATE_ANGLE_DEFAULT );
    Angle( const Coordinate & c, bool truncate = TRUNCATE_ANGLE_DEFAULT );
    Angle( const Coordinate & from, const Coordinate & to, bool truncate = TRUNCATE_ANGLE_DEFAULT );

    bool truncating( ) const;
    Angle & truncate( bool truncate );
    Angle truncated( );

    dec radians( ) const;
    dec degrees( ) const;

    Angle & radians( dec radians );
    Angle & radians( const Planc & dx, const Planc & dy );

    dec sin( dec multiplier = 1.0 ) const;
    dec cos( dec multiplier = 1.0 ) const;

    Angle & flip( );
    Angle flipped( ) const;

    Angle half( ) const;
    
    Quadrant quadrant( ) const;
    Axis axis( ) const;
    bool in_quadrant( Quadrant quadrant ) const;
    bool on_axis( Axis axis ) const;
    
    bool right( ) const;
    bool straight( ) const;
    bool acute( ) const;
    bool obtuse( ) const;
    bool reflex( ) const;

    operator dec( ) const { return m_radians; }

    Angle & operator=( dec radians );

    Angle operator-( ) const;

    Angle operator+( const Angle & angle ) const;
    Angle operator-( const Angle & angle ) const;

    Angle & operator+=( const Angle & angle );
    Angle & operator-=( const Angle & angle );

    bool operator==( const Angle & angle ) const;
    bool operator!=( const Angle & angle ) const;
    bool operator<=( const Angle & angle ) const;
    bool operator>=( const Angle & angle ) const;
    bool operator<( const Angle & angle ) const;
    bool operator>( const Angle & angle ) const;

    Angle operator+( dec radians ) const;
    Angle operator-( dec radians ) const;
    Angle operator*( dec scale ) const;
    Angle operator/( dec scale ) const;

    Angle & operator+=( dec radians );
    Angle & operator-=( dec radians );
    Angle & operator*=( dec scale );
    Angle & operator/=( dec scale );

    bool operator==( dec radians ) const;
    bool operator!=( dec radians ) const;
    bool operator<=( dec radians ) const;
    bool operator>=( dec radians ) const;
    bool operator<( dec radians ) const;
    bool operator>( dec radians ) const;

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
    bool operator<=( int radians ) const;
    bool operator>=( int radians ) const;
    bool operator<( int radians ) const;
    bool operator>( int radians ) const;

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
    bool operator<=( uint radians ) const;
    bool operator>=( uint radians ) const;
    bool operator<( uint radians ) const;
    bool operator>( uint radians ) const;

    static dec radians_to_degrees( dec radians );
    static dec degrees_to_radians( dec degrees );

private:
    bool m_truncate = TRUNCATE_ANGLE_DEFAULT; // keeps radians within [ 0, 2 * pi )

    dec m_radians = 0.0;
#ifdef AXN_DEBUG
    dec m_degrees = 0.0;
#endif
};

} // namespace geometry
} // namespace axn

#endif /* Angle_hpp */
