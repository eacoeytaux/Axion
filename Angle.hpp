#ifndef Angle_hpp
#define Angle_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"

namespace axn
{
namespace geometry
{

class Angle
{

private:

    dec m_radians = 0.0;
    bool m_truncate = false; // keeps radians within [ 0, 2 * pi )

public:

    Angle( ) { }

    Angle( bool truncate ) : m_truncate( truncate ) { }

    Angle( dec radians ) { Angle::radians( radians ); }
    Angle( dec radians, bool truncate ) : m_truncate( truncate ) { Angle::radians( radians ); }

    Angle( Planc cref dx, Planc cref dy ) { Angle::radians( atan2( dy, dx ) ); }
    Angle( Planc cref dx, Planc cref dy, bool truncate ) : m_truncate( truncate ) { Angle::radians( atan2( dy, dx ) ); }

    Angle( Coordinate cref c ) { Angle::radians( atan2( c.y( ), c.x( ) ) ); }
    Angle( Coordinate cref c, bool truncate ) : m_truncate( truncate ) { Angle::radians( atan2( c.y( ), c.x( ) ) ); }

    Angle( Coordinate cref from, Coordinate cref to ) { Angle::radians( atan2( ( to.y( ) - from.y( ) ), ( to.x( ) - from.x( ) ) ) ); }
    Angle( Coordinate cref from, Coordinate cref to, bool truncate ) : m_truncate( truncate ) { Angle::radians( atan2( ( to.y( ) - from.y( ) ), ( to.x( ) - from.x( ) ) ) ); }

    operator dec( ) const { return radians( ); }

    bool truncating( ) const { return m_truncate; }
    Angle & truncate( bool truncate ) { if( m_truncate != truncate ) { m_truncate = truncate; radians( radians( ) ); } rethis; }
    Angle truncated( ) { return Angle( radians( ), true ); }

    dec radians( ) const { return m_radians; }

    Angle & radians( dec radians ) { if( m_radians != radians ) { m_radians = ( !is_num( radians ) ? 0.0 : ( truncating( ) ? ( radians - ( floor( radians / TAU ) * TAU ) ) : radians ) ); } rethis; }
    Angle & radians( Planc cref dx, Planc cref dy ) { radians( atan2( dy, dx ) ); rethis; }

    dec sin( ) const { return ::sin( radians( ) ); }
    dec cos( ) const { return ::cos( radians( ) ); }
    dec tan( ) const { return ::tan( radians( ) ); }

    Angle & flip( ) { return radians( radians( ) + PI ); }
    Angle flipped( ) const { return ( *this + PI ); }

    Quadrant quadrant( ) const { return Coordinate( cos( ), sin( ) ).quadrant( ); }
    bool in_quadrant( Quadrant q ) const { return ( quadrant( ) == q ); }

    Axis axis( ) const { return Coordinate( cos( ), sin( ) ).axis( ); }
    bool on_axis( Axis a ) const { return ( axis( ) == a ); }

    bool right( ) const { return equal( radians( ), RIGHT ); }
    bool straight( ) const { return equal( radians( ), PI ); }
    bool acute( ) const { return ( is_pos( radians( ) ) && less( radians( ), RIGHT ) ); }
    bool obtuse( ) const { return ( is_pos( radians( ) ) && greater( radians( ), RIGHT ) && less( radians( ), PI ) ); }
    bool reflex( ) const { return ( is_pos( radians( ) ) && greater( radians( ), PI ) && less( radians( ), TAU ) ); }

    Angle operator-( ) const { return Angle( -radians( ), truncating( ) ); }

    #define AngleOps( type ) \
        Angle & operator=( type t ) { return radians( (dec)t ); } \
        Angle operator+( type t ) const { return Angle( radians( ) + t, truncating( ) ); } \
        Angle operator-( type t ) const { return Angle( radians( ) - t, truncating( ) ); } \
        Angle operator*( type t ) const { return Angle( radians( ) * t, truncating( ) ); } \
        Angle operator/( type t ) const { return Angle( radians( ) / t, truncating( ) ); } \
        Angle & operator+=( type t ) { return radians( radians( ) + t ); } \
        Angle & operator-=( type t ) { return radians( radians( ) - t ); } \
        Angle & operator*=( type t ) { return radians( radians( ) * t ); } \
        Angle & operator/=( type t ) { return radians( radians( ) / t ); } \
        bool operator==( type t ) const { return equal( radians( ), t ); } \
        bool operator!=( type t ) const { return !equal( radians( ), t ); } \
        bool operator<=( type t ) const { return less_or_equal( radians( ), t ); } \
        bool operator>=( type t ) const { return greater_or_equal( radians( ), t ); } \
        bool operator<( type t ) const { return less( radians( ), t ); } \
        bool operator>( type t ) const { return greater( radians( ), t ); }

    AngleOps( int );
    AngleOps( uint );
    AngleOps( dec );
    AngleOps( Angle );
    
};

const Angle A0 = Angle( );

} // namespace geometry
} // namespace axn

#endif /* Angle_hpp */
