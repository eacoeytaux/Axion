#ifndef Helpers_hpp
#define Helpers_hpp

#include "Primitives.hpp"

namespace axn {
namespace utility {

// DEFINES

#define for_each( x, v ) for( auto & x: v )

#define for_range_selector( x, r, i, func, ... ) func
#define for_range_i( r ) for( uint i = 0; i < (uint)r; ++i )
#define for_range_x( i, r ) for( uint i = 0; i < (uint)r; ++i )
#define for_range( ... )                                                                       \
    for_range_selector(, ##__VA_ARGS__, for_range_x( __VA_ARGS__ ), for_range_i( __VA_ARGS__ ) )

#define static_once( )                                                                         \
    static bool b_set_##__COUNTER__ = true;                                                    \
    if( b_set_##__COUNTER__ && !( b_set_##__COUNTER__ = false ) || b_set_##__COUNTER__ )
#define static_setup( Class, name )                                                            \
    static Class name;                                                                         \
    static bool name##_not_set = true;                                                         \
    if( name##_not_set && !( name##_not_set = false ) || name##_not_set )

// LABELS

template <typename T> inline string Label( const T & t ) { return typeid( t ).name( ); }
template <typename T> inline string Label( ) {
    static const string label = Label( T( ) );
    return label;
}

// FLOATS

const float DEFAULT_FLOAT_EPSILON = 0.0001;

inline bool is_infinity( const float f ) {
    return ( f == POSITIVE_INFINITY ) || ( f == NEGATIVE_INFINITY );
}

inline bool float_eq( const float x, const float y,
                      const float epsilon = DEFAULT_FLOAT_EPSILON ) {
    if( ::fabs( x - y ) < epsilon )
        return true; // equals!
    else
        return false;
}

inline bool float_gt( const float x, const float y,
                      const float epsilon = DEFAULT_FLOAT_EPSILON ) {
    return ( ( x - epsilon ) > y );
}

inline bool float_ge( const float x, const float y,
                      const float epsilon = DEFAULT_FLOAT_EPSILON ) {
    return ( ( ( x - epsilon ) > y ) || float_eq( x, y ) );
}

inline bool float_lt( const float x, const float y,
                      const float epsilon = DEFAULT_FLOAT_EPSILON ) {
    return ( ( x + epsilon ) < y );
}

inline bool float_le( const float x, const float y,
                      const float epsilon = DEFAULT_FLOAT_EPSILON ) {
    return ( ( ( x + epsilon ) < y ) || float_eq( x, y ) );
}

// UTIL

template <typename T> void swap_values( T & t1, T & t2 ) {
    T tmp = t1;
    t1 = t2;
    t2 = tmp;
}

template <typename T>
bool in_range( const T & t, const T & high, const T & low = T( 0 ), bool inclusive = true ) {
    T h = high;
    T l = low;
    if( l > h )
        swap_values<T>( h, l );
    return ( ( t > l ) && ( t < h ) ) || ( inclusive && ( ( t == l ) || ( t == h ) ) );
}

template <typename T> T abs( const T & t ) {
    if( t < T( 0 ) )
        return t * T( -1 );
    else
        return t;
}

template <typename T> T pow( const T & t, const uint p ) {
    if( !p )
        return T( );
    T ret = t;
    for_range( p - 1 ) ret *= t;
    return ret;
}

template <typename T> T min( const T & t1, const T & t2 ) {
    if( t1 <= t2 )
        return t1;
    else
        return t2;
}

template <typename T> T min( const varray<T> & list ) {
    Assert( list.size( ), "list must not be empty" );
    T ret = list[ 0 ];
    for_each( t, list ) if( t < ret ) ret = t;
    return ret;
}

template <typename T> T max( const T & t1, const T & t2 ) {
    if( t1 >= t2 )
        return t1;
    else
        return t2;
}

template <typename T> T max( const varray<T> & list ) {
    Assert( list.size( ), "list must not be empty" );
    T ret = list[ 0 ];
    for_each( t, list ) if( t > ret ) ret = t;
    return ret;
}

} // namespace utility
} // namespace axn

#endif /* Helpers_hpp */
