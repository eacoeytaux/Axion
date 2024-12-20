#ifndef Planc_hpp
#define Planc_hpp

#include "Utility.hpp"

namespace axn
{
namespace geometry
{

class Planc
{
private:
    dec m_value = ZERO;

    dec get( ) const { return m_value; }
    Planc & set( dec value ) { m_value = value; rethis; }

public:
    Planc( ) { }

    Planc( dec d ) { set( d ); }

    operator int( ) const { return get( ); }
    operator uint( ) const { return get( ); }
    operator dec( ) const { return get( ); }

    operator bool( ) const { return !is_zero( get( ) ) && is_num( get( ) ); }

    Planc operator-( ) const { return Planc( -get( ) ); }

    #define PlancOpsLeft( type ) \
        Planc & operator=( type t ) { return set( (dec)t ); } \
        Planc operator+( type t ) const { return Planc( get( ) + (dec)t ); } \
        Planc operator-( type t ) const { return Planc( get( ) - (dec)t ); } \
        Planc operator*( type t ) const { return Planc( get( ) * (dec)t ); } \
        Planc operator/( type t ) const { return Planc( get( ) / (dec)t ); } \
        Planc & operator+=( type t ) { return set( get( ) + (dec)t ); } \
        Planc & operator-=( type t ) { return set( get( ) - (dec)t ); } \
        Planc & operator*=( type t ) { return set( get( ) * (dec)t ); } \
        Planc & operator/=( type t ) { return set( get( ) / (dec)t ); } \
        bool operator==( type t ) const { return equal( get( ), (dec)t ); } \
        bool operator!=( type t ) const { return !equal( get( ), (dec)t ); } \
        bool operator<=( type t ) const { return less_or_equal( get( ), (dec)t ); } \
        bool operator>=( type t ) const { return greater_or_equal( get( ), (dec)t ); } \
        bool operator<( type t ) const { return less( get( ), (dec)t ); } \
        bool operator>( type t ) const { return greater( get( ), (dec)t ); }

    PlancOpsLeft( int );
    PlancOpsLeft( uint );
    PlancOpsLeft( dec );
    PlancOpsLeft( Planc );
};

#define PlancOpsRight( type ) \
    inline Planc operator+( type t, const Planc & p ) { return ( (Planc)t + p ); } \
    inline Planc operator-( type t, const Planc & p ) { return ( (Planc)t - p ); } \
    inline Planc operator*( type t, const Planc & p ) { return ( (Planc)t * p ); } \
    inline Planc operator/( type t, const Planc & p ) { return ( (Planc)t / p ); } \
    inline type & operator+=( type & t, const Planc & p ) { return ( t = (Planc)t + p ); } \
    inline type & operator-=( type & t, const Planc & p ) { return ( t = (Planc)t - p ); } \
    inline type & operator*=( type & t, const Planc & p ) { return ( t = (Planc)t * p ); } \
    inline type & operator/=( type & t, const Planc & p ) { return ( t = (Planc)t / p ); } \
    inline bool operator==( type t, const Planc & p ) { return ( (Planc)t == p ); } \
    inline bool operator!=( type t, const Planc & p ) { return ( (Planc)t != p ); } \
    inline bool operator<=( type t, const Planc & p ) { return ( (Planc)t <= p ); } \
    inline bool operator>=( type t, const Planc & p ) { return ( (Planc)t >= p ); } \
    inline bool operator<( type t, const Planc & p ) { return ( (Planc)t < p ); } \
    inline bool operator>( type t, const Planc & p ) { return ( (Planc)t > p ); }

PlancOpsRight( int );
PlancOpsRight( uint );
PlancOpsRight( dec );

const Planc P0 = Planc( ZERO );

} // namespace geometry
} // namespace axn

#endif /* Planc_hpp */
