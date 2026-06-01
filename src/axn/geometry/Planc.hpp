#ifndef Planc_hpp
#define Planc_hpp

#include "axn/system/Utility.hpp"

namespace axn
{
namespace geometry
{

class Planc
{

private:

    dec m_value = 0.0;

    dec get( ) const { return( m_value ); }

    Planc ref set( dec in_value ) { m_value = in_value; rethis; }

public:

    Planc( ) { }

    Planc( dec in_d ) { set( in_d ); }

    operator int( ) const { return( get( ) ); }
    operator uint( ) const { return( get( ) ); }
    operator dec( ) const { return( get( ) ); }

    operator bool( ) const { return( !is_zero( get( ) ) && is_num( get( ) ) ); }

    Planc operator+( ) const { return( Planc( +get( ) ) ); }
    Planc operator-( ) const { return( Planc( -get( ) ) ); }

    #define PlancOpsLeft( type ) \
    Planc ref operator=( type cref in_t ) { return( set( dec( in_t ) ) ); } \
    Planc ref operator+=( type cref in_t ) { return( set( add( get( ), dec( in_t ) ) ) ); } \
    Planc ref operator-=( type cref in_t ) { return( set( subtract( get( ), dec( in_t ) ) ) ); } \
    Planc ref operator*=( type cref in_t ) { return( set( multiply( get( ), dec( in_t ) ) ) ); } \
    Planc ref operator/=( type cref in_t ) { return( set( divide( get( ), dec( in_t ) ) ) ); } \
    Planc operator+( type cref in_t ) const { return( Planc( add( get( ), dec( in_t ) ) ) ); } \
    Planc operator-( type cref in_t ) const { return( Planc( subtract( get( ), dec( in_t ) ) ) ); } \
    Planc operator*( type cref in_t ) const { return( Planc( multiply( get( ), dec( in_t ) ) ) ); } \
    Planc operator/( type cref in_t ) const { return( Planc( divide( get( ), dec( in_t ) ) ) ); } \
    bool operator==( type cref in_t ) const { return( equal( get( ), dec( in_t ) ) ); } \
    bool operator!=( type cref in_t ) const { return( not_equal( get( ), dec( in_t ) ) ); } \
    bool operator>=( type cref in_t ) const { return( greater_or_equal( get( ), dec( in_t ) ) ); } \
    bool operator<=( type cref in_t ) const { return( less_or_equal( get( ), dec( in_t ) ) ); } \
    bool operator>( type cref in_t ) const { return( greater( get( ), dec( in_t ) ) ); } \
    bool operator<( type cref in_t ) const { return( less( get( ), dec( in_t ) ) ); } \

    PlancOpsLeft( int );
    PlancOpsLeft( uint );
    PlancOpsLeft( dec );
    PlancOpsLeft( Planc );

};

#define PlancOpsRight( type ) \
inline type ref operator+=( type ref in_t, Planc cref in_p ) { return( in_t = Planc( in_t ) + in_p ); } \
inline type ref operator-=( type ref in_t, Planc cref in_p ) { return( in_t = Planc( in_t ) - in_p ); } \
inline type ref operator*=( type ref in_t, Planc cref in_p ) { return( in_t = Planc( in_t ) * in_p ); } \
inline type ref operator/=( type ref in_t, Planc cref in_p ) { return( in_t = Planc( in_t ) / in_p ); } \
inline Planc operator+( type cref in_t, Planc cref in_p ) { return( Planc( in_t ) + in_p ); } \
inline Planc operator-( type cref in_t, Planc cref in_p ) { return( Planc( in_t ) - in_p ); } \
inline Planc operator*( type cref in_t, Planc cref in_p ) { return( Planc( in_t ) * in_p ); } \
inline Planc operator/( type cref in_t, Planc cref in_p ) { return( Planc( in_t ) / in_p ); } \
inline bool operator==( type cref in_t, Planc cref in_p ) { return( Planc( in_t ) == in_p ); } \
inline bool operator!=( type cref in_t, Planc cref in_p ) { return( Planc( in_t ) != in_p ); } \
inline bool operator<=( type cref in_t, Planc cref in_p ) { return( Planc( in_t ) <= in_p ); } \
inline bool operator>=( type cref in_t, Planc cref in_p ) { return( Planc( in_t ) >= in_p ); } \
inline bool operator<( type cref in_t, Planc cref in_p ) { return( Planc( in_t ) < in_p ); } \
inline bool operator>( type cref in_t, Planc cref in_p ) { return( Planc( in_t ) > in_p ); } \

PlancOpsRight( int );
PlancOpsRight( uint );
PlancOpsRight( dec );

var_const( Planc P0( 0.0 ) );
var_const( Planc METER( 32.0 ) );

} // namespace geometry
} // namespace axn

#endif /* Planc_hpp */
