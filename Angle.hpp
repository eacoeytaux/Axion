#ifndef Angle_hpp
#define Angle_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"

namespace axn
{
namespace geometry
{

axnclass( Angle )
{

private:

    Planc m_radians = 0.0;

    // todo use these for fast sin/cos?
    static cint SIN_TABLE_SIZE = 1024;
    static varray<dec> SIN_TABLE;

public:

    Angle( ) { }

    Angle( dec rad ) { radians( rad ); }

    Angle( Planc cref rad ) { radians( rad ); }

    Angle( Angle cref a ) { radians( a.radians( ) ); }

    Angle( Planc cref dx, Planc cref dy ) { radians( atan2( dy, dx ) ); }

    Angle( Coordinate cref c ) { radians( atan2( c.y( ), c.x( ) ) ); }

    Angle( Coordinate cref from, Coordinate cref to ) { radians( atan2( ( to.y( ) - from.y( ) ), ( to.x( ) - from.x( ) ) ) ); }

    operator dec( ) const { return radians( ); }

    dec radians( ) const { return m_radians; }

    Angle & truncate( ) { m_radians = ( radians( ) - ( floor( radians( ) / math::TAU ) * math::TAU ) ); rethis; }
    Angle truncated( ) const { return Angle( radians( ) - ( floor( radians( ) / math::TAU ) * math::TAU ) ); }

    Angle & radians( Planc cref radians ) { m_radians = radians; rethis; }
    Angle & radians( Planc cref dx, Planc cref dy ) { radians( atan2( dy, dx ) ); rethis; }

    Planc sin( ) const { return ::sin( radians( ) ); }
    Planc cos( ) const { return ::cos( radians( ) ); }
    Planc tan( ) const { return ::tan( radians( ) ); }

    Angle abs( ) const { return ( ( truncated( ) > math::PI ) ? Angle( math::TAU - truncated( ).radians( ) ) : *this ); }

    Angle & flip( ) { return radians( radians( ) + math::PI ); }
    Angle flipped( ) const { return ( *this + math::PI ); }

    Quadrant quadrant( ) const { return Coordinate( cos( ), sin( ) ).quadrant( ); }
    bool in_quadrant( Quadrant q ) const { return ( quadrant( ) == q ); }

    Axis axis( ) const { return Coordinate( cos( ), sin( ) ).axis( ); }
    bool on_axis( Axis a ) const { return ( axis( ) == a ); }

    bool right( ) const { return ( equal( radians( ), math::RIGHT ) ); }
    bool straight( ) const { return ( equal( radians( ), math::PI ) ); }
    bool acute( ) const { return ( is_pos( radians( ) ) && less( radians( ), math::RIGHT ) ); }
    bool obtuse( ) const { return ( is_pos( radians( ) ) && greater( radians( ), math::RIGHT ) && less( radians( ), math::PI ) ); }
    bool reflex( ) const { return ( is_pos( radians( ) ) && greater( radians( ), math::PI ) && less( radians( ), math::TAU ) ); }

    Angle min_distance( Angle cref angle )
    {
        Angle d1 = ( angle - ( *this ) ).truncated( );
        Angle d2 = ( ( -d1 ).truncated( ) );

        return Angle( ( d1 > d2 ) ? -d2 : d1 );
    }

    Angle closest( varray<Angle> cref angles )
    {
        if( angles.size( ) )
        {
            int index = 0;
            Angle closest = min_distance( angles.front( ) );

            for_range( i, ( angles.size( ) - 1 ) )
            {
                Angle min = min_distance( angles[ i + 1 ] );

                if( min.abs( ) < closest.abs( ) )
                {
                    index = ( i + 1 );
                    closest = min;
                }
            }

            return angles[ index ];
        }

        rethis;
    }

    Angle operator-( ) const { return Angle( -radians( ) ); }

    #define AngleOps( type ) \
        Angle & operator=( type t ) { return ( radians( (dec)t ) ); } \
        Angle operator+( type t ) const { return ( Angle( radians( ) + t ) ); } \
        Angle operator-( type t ) const { return ( Angle( radians( ) - t ) ); } \
        Angle operator*( type t ) const { return ( Angle( radians( ) * t ) ); } \
        Angle operator/( type t ) const { return ( Angle( radians( ) / t ) ); } \
        Angle & operator+=( type t ) { return ( radians( radians( ) + t ) ); } \
        Angle & operator-=( type t ) { return ( radians( radians( ) - t ) ); } \
        Angle & operator*=( type t ) { return ( radians( radians( ) * t ) ); } \
        Angle & operator/=( type t ) { return ( radians( radians( ) / t ) ); } \
        bool operator==( type t ) const { return ( equal( radians( ), t ) ); } \
        bool operator!=( type t ) const { return ( !equal( radians( ), t ) ); } \
        bool operator<=( type t ) const { return ( less_or_equal( radians( ), t ) ); } \
        bool operator>=( type t ) const { return ( greater_or_equal( radians( ), t ) ); } \
        bool operator<( type t ) const { return ( less( radians( ), t ) ); } \
        bool operator>( type t ) const { return ( greater( radians( ), t ) ); }

    AngleOps( int );
    AngleOps( uint );
    AngleOps( dec );
    AngleOps( Planc );
    AngleOps( Angle );

};

// inline Angle abs( Angle a ) { return ( ( a.truncated( ) > math::PI ) ? Angle( math::TAU - a ) : Angle( a ) ); }

cAngle A0( 0.0 );
cAngle PI( math::PI );
cAngle TAU( math::TAU );
cAngle RIGHT( math::RIGHT );

} // namespace geometry
} // namespace axn

#endif /* Angle_hpp */
