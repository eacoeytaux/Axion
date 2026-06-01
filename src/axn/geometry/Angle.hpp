#ifndef Angle_hpp
#define Angle_hpp

#include "axn/geometry/Planc.hpp"
#include "axn/geometry/Point.hpp"

namespace axn
{
namespace geometry
{

class Angle
{

private:

    Planc m_radians = 0.0;

public:

    Angle( ) { }

    Angle( dec in_rad ) { radians( in_rad ); }

    Angle( Planc cref in_rad ) { radians( in_rad ); }

    Angle( Planc cref in_dx, Planc cref in_dy ) { radians( atan2( in_dy, in_dx ) ); }

    Angle( Point cref in_pt ) { radians( atan2( in_pt.y( ), in_pt.x( ) ) ); }

    Angle( Point cref in_from, Point cref in_to ) { radians( atan2( ( in_to.y( ) - in_from.y( ) ), ( in_to.x( ) - in_from.x( ) ) ) ); }

    static Angle fraction( Planc cref in_p ) { Assert( !is_zero( in_p ) ); return( Angle( numbers::TAU / in_p ) ); }

    operator dec( ) const { return( radians( ) ); }

    Planc radians( ) const { return( m_radians ); }

    Angle ref radians( Planc cref in_radians ) { m_radians = in_radians; rethis; }
    Angle ref radians( Planc cref in_dx, Planc cref in_dy ) { radians( atan2( in_dy, in_dx ) ); rethis; }

    Planc radians_truncated( ) const { return( truncated( ).m_radians ); }

    Angle ref truncate( ) { m_radians = ( radians( ) - ( floor( radians( ) / numbers::TAU ) * numbers::TAU ) ); rethis; }
    Angle truncated( ) const { return( Angle( radians( ) - ( floor( radians( ) / numbers::TAU ) * numbers::TAU ) ) ); }

    Planc sin( ) const { return( ::sin( radians( ) ) ); }
    Planc cos( ) const { return( ::cos( radians( ) ) ); }
    Planc tan( ) const { return( ::tan( radians( ) ) ); }

    Angle abs( ) const { return( ( truncated( ) > numbers::PI ) ? Angle( numbers::TAU - truncated( ).radians( ) ) : *this ); }

    Angle ref flip( ) { return( radians( radians( ) + numbers::PI ) ); }
    Angle flipped( ) const { return( *this + numbers::PI ); }

    Axis axis( ) const { return( Point( cos( ), sin( ) ).axis( ) ); }
    bool on_axis( Axis cref in_axis ) const { return( axis( ) == in_axis ); }

    Quadrant quadrant( ) const { return( Point( cos( ), sin( ) ).quadrant( ) ); }
    bool in_quadrant( Quadrant cref in_quadrant, bool in_inclusive = false ) const
    {
        if( in_inclusive )
        {
            if( is_zero( radians_truncated( ) ) )
            {
                return_true_if( ( in_quadrant == Q1 ) || ( in_quadrant == Q4 ) );
            }
            else if( radians_truncated( ) == ( numbers::RIGHT * 1.0 ) )
            {
                return_true_if( ( in_quadrant == Q1 ) || ( in_quadrant == Q2 ) );
            }
            else if( radians_truncated( ) == ( numbers::RIGHT * 2.0 ) )
            {
                return_true_if( ( in_quadrant == Q2 ) || ( in_quadrant == Q3 ) );
            }
            else if( radians_truncated( ) == ( numbers::RIGHT * 3.0 ) )
            {
                return_true_if( ( in_quadrant == Q3 ) || ( in_quadrant == Q4 ) );
            }
        }

        return( quadrant( ) == in_quadrant );
    }

    bool right( ) const { return( equal( radians( ), numbers::RIGHT ) ); }
    bool straight( ) const { return( equal( radians( ), numbers::PI ) ); }
    bool acute( ) const { return( is_pos( radians( ) ) && less( radians( ), numbers::RIGHT ) ); }
    bool obtuse( ) const { return( is_pos( radians( ) ) && greater( radians( ), numbers::RIGHT ) && less( radians( ), numbers::PI ) ); }
    bool reflex( ) const { return( is_pos( radians( ) ) && greater( radians( ), numbers::PI ) && less( radians( ), numbers::TAU ) ); }

    Angle reflect( Angle cref in_normal ) const { return( in_normal + ( in_normal - *this ) ); }

    bool in_range( Angle cref in_angle, Angle cref in_delta, bool in_inclusive = true ) const { return( less( in_inclusive, min_distance( in_angle ).abs( ), in_delta.truncated( ) ) ); }
    bool in_semi( Angle cref in_angle, bool in_inclusive = true ) const { return( in_range( in_angle, numbers::RIGHT, in_inclusive ) ); }

    Angle min_distance( Angle cref in_angle ) const
    {
        Angle d1 = ( in_angle - ( *this ) ).truncated( );
        Angle d2 = ( ( -d1 ).truncated( ) );

        return( Angle( ( d1 > d2 ) ? -d2 : d1 ) );
    }

    Angle closest( varray<Angle> cref in_angles ) const
    {
        if( in_angles.size( ) )
        {
            int index = 0;
            Angle closest = min_distance( in_angles.front( ) );

            for_range( i, in_angles.size( ) - 1 )
            {
                Angle min = min_distance( in_angles[ i + 1 ] );

                if( min.abs( ) < closest.abs( ) )
                {
                    index = ( i + 1 );
                    closest = min;
                }
            }

            return( in_angles[ index ] );
        }

        rethis;
    }

    Angle operator+( ) const { return( Angle( +radians( ) ) ); }
    Angle operator-( ) const { return( Angle( -radians( ) ) ); }

    #define AngleOps( type ) \
    Angle ref operator=( type cref in_t ) { return( radians( dec( in_t ) ) ); } \
    Angle ref operator+=( type cref in_t ) { return( radians( add( radians( ), dec( in_t ) ) ) ); } \
    Angle ref operator-=( type cref in_t ) { return( radians( subtract( radians( ), dec( in_t ) ) ) ); } \
    Angle ref operator*=( type cref in_t ) { return( radians( multiply( radians( ), dec( in_t ) ) ) ); } \
    Angle ref operator/=( type cref in_t ) { return( radians( divide( radians( ), dec( in_t ) ) ) ); } \
    Angle operator+( type cref in_t ) const { return( Angle( add( radians( ), dec( in_t ) ) ) ); } \
    Angle operator-( type cref in_t ) const { return( Angle( subtract( radians( ), dec( in_t ) ) ) ); } \
    Angle operator*( type cref in_t ) const { return( Angle( multiply( radians( ), dec( in_t ) ) ) ); } \
    Angle operator/( type cref in_t ) const { return( Angle( divide( radians( ), dec( in_t ) ) ) ); } \
    bool operator==( type cref in_t ) const { return( equal( radians( ), dec( in_t ) ) ); } \
    bool operator!=( type cref in_t ) const { return( not_equal( radians( ), dec( in_t ) ) ); } \
    bool operator>=( type cref in_t ) const { return( greater_or_equal( radians( ), dec( in_t ) ) ); } \
    bool operator<=( type cref in_t ) const { return( less_or_equal( radians( ), dec( in_t ) ) ); } \
    bool operator>( type cref in_t ) const { return( greater( radians( ), dec( in_t ) ) ); } \
    bool operator<( type cref in_t ) const { return( less( radians( ), dec( in_t ) ) ); } \

    AngleOps( int );
    AngleOps( uint );
    AngleOps( dec );
    AngleOps( Planc );
    AngleOps( Angle );

};

var_const( Angle A0( 0.0 ) );
var_const( Angle PI( numbers::PI ) );
var_const( Angle TAU( numbers::TAU ) );
var_const( Angle RIGHT( numbers::RIGHT ) );

inline Angle truncated( Angle cref in_a ) { return( in_a.truncated( ) ); }

} // namespace geometry
} // namespace axn

#endif /* Angle_hpp */
