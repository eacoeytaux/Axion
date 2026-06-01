#ifndef Utility_hpp
#define Utility_hpp

#include "OS.hpp"
#include "STD.hpp"
#include "STDc.hpp"
#include "STDptr.hpp"

#include "Error.hpp"
#include "Assert.hpp"

#include "Namespaces.hpp"

// -- defines --

#define cref const &

#define typeT template <typename T>
#define classT template <typename T> class

#define rethis return *this

#define break_if( b ) \
    if( b ) { break; }

#define return_if( b, ret ) \
    if( b ) { return ret; }

#define return_true_if( b ) return_if( b, true )
#define return_false_if( b ) return_if( b, false )
#define return_null_if( b ) return_if( b, nullptr )

#define do_until_break( ) \
    while( true )

#define do_count( x ) \
    for( uint do_counter = 0; do_counter < x; ++do_counter )

#define for_range( i, range ) \
    for( uint i = 0; i < (uint)range; ++i )

#define for_each( x, container ) \
    for( auto & x: container )

#define for_each_const( x, container ) \
    for( auto const & x: container )

#define while_max( b, max ) \
    uint while_max_count = 0; \
    while( ( b ) && ( while_max_count++ < max ) )

#define static_once( ) \
    static bool b_set = true; \
    if( b_set && !( b_set = false ) )

#define static_setup( Class, name ) \
    static Class name; \
    static bool name##_not_set = true; \
    if( name##_not_set && !( name##_not_set = false ) )

#define default_equal( Class ) \
    bool operator==( Class cref c ) const = default; \
    bool operator!=( Class cref c ) const { return !( *this == c ); }

#define default_non_equal( Class ) \
    bool operator!=( Class cref c ) const { return !( *this == c ); }

#define default_virtual_equal( Class ) \
    virtual bool operator==( Class cref c ) const = default; \
    virtual bool operator!=( Class cref c ) const { return !( *this == c ); }

#define default_virtual_non_equal( Class ) \
    virtual bool operator!=( Class cref c ) const { return !( *this == c ); }

#define axnclass( Class ) \
    class Class; \
    typedef const Class c##Class; \
    typedef const Class & x##Class; \
    typedef Class & r##Class; \
    typedef Class * p##Class; \
    class Class

#define axnclassT( Class ) \
    class Class; \
    typedef const Class c##Class; \
    typedef const Class & x##Class; \
    typedef Class & r##Class; \
    typedef Class * p##Class; \
    class template <typename T> Class

namespace axn
{

namespace math // todo this should encompass more functions
{

// -- constants --

cdec PI = ( 3.141592653589793236926 );
cdec TAU = ( 6.28318530717958647692 );
cdec RIGHT = ( 1.570796326794896618 );

cdec EULER = ( 2.718281828459045235 );

} // namespace math

namespace utility
{

enum Direction
{
    NO_DIRECTION,
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
    DIRECTIONS
};

inline void nothing( ) { }

typeT inline bool is_null( T * t ) { return ( t == nullptr ); }
typeT inline void safe_delete( T *& t ) { if( t && dynamic_cast<T *>( t ) ) { delete t; } t = nullptr; }

// -- dec util functions --

cdec DEFAULT_EPSILON = ( 0.0001 );

inline bool equal( cdec x, cdec y, cdec e = DEFAULT_EPSILON ) { return ( abs( x - y ) < e ); }
inline bool greater( cdec x, cdec y, cdec e = DEFAULT_EPSILON ) { return ( ( x - e ) > y ); }
inline bool greater( bool eq, cdec x, cdec y, cdec e = DEFAULT_EPSILON ) { return ( ( ( x - e ) > y ) || ( eq && equal( x, y, e ) ) ); }
inline bool greater_or_equal( cdec x, cdec y, cdec e = DEFAULT_EPSILON ) { return greater( true, x, y, e ); }
inline bool less( cdec x, cdec y, cdec e = DEFAULT_EPSILON ) { return ( ( x + e ) < y ); }
inline bool less( bool eq, cdec x, cdec y, cdec e = DEFAULT_EPSILON ) { return ( ( ( x + e ) < y ) || ( eq && equal( x, y, e ) ) ); }
inline bool less_or_equal( cdec x, cdec y, cdec e = DEFAULT_EPSILON ) { return less( true, x, y, e ); }

inline bool is_inf( cdec d ) { return ( d == INFINITY ) || ( d == -INFINITY ); }
inline bool is_nan( cdec d ) { return ( ::isnan( d ) ); }
inline bool is_num( cdec d ) { return ( !is_nan( d ) && !is_inf( d ) ); }
inline bool is_int( cdec d ) { return ( equal( d, ::rint( d ) ) ); }
inline bool is_zero( cdec d ) { return ( equal( d, 0.0 ) ); }
inline bool is_pos( cdec d ) { return ( greater( d, 0.0 ) ); }
inline bool is_neg( cdec d ) { return ( less( d, 0.0 ) ); }
inline bool is_even( cdec d ) { return ( is_zero( (int)( d ) % 2 ) ); }
inline bool is_odd( cdec d ) { return ( is_zero( (int)( d + 1 ) % 2 ) ); }

inline bool is_divisible( cdec x, cdec y ) { return !is_zero( remainder( x, y ) ); }

inline dec remainder_positive( cdec x, cdec y ) { dec r = remainder( x, y ); while( less( r, 0.0 ) ) { r += y; } return r; }

// -- misc. util functions --

typeT inline void swap_values( T & t1, T & t2 ) { T t = t1; t1 = t2; t2 = t; }

typeT inline T inverse( T cref t ) { return ( 1.0 / t ); }

typeT inline T negate( T cref t ) { return -t; }
typeT inline T negate( T cref t, bool f ) { return ( f ? -t : t ); }

typeT inline T half( T cref t ) { return ( t / 2.0 ); }

typeT inline T square( T cref t ) { return ( t * t ); }
typeT inline T cube( T cref t ) { return ( t * t * t ); }

typeT inline T log( T cref t ) { return ::log( (dec)t ); }
typeT inline T log( T cref t, T cref base ) { return ( ::log( (dec)t ) / ::log( (dec)base ) ); }

typeT inline T cref min( T cref t1, T cref t2 ) { return ( t1 < t2 ) ? t1 : t2; }
typeT inline T cref min( const varray<T> & v )
{
    Assert( v.size( ), "varray must not be empty" );

    int min_i = 0;
    for_range( i, v.size( ) - 1 ) { if( v[ i + 1 ] < v[ min_i ] ) { min_i = i + 1; } }
    return v[ min_i ];
}

typeT inline T cref max( T cref t1, T cref t2 ) { return ( t1 > t2 ) ? t1 : t2; }
typeT inline T cref max( const varray<T> & v )
{
    Assert( v.size( ), "varray must not be empty" );

    int max_i = 0;
    for_range( i, v.size( ) - 1 ) { if( v[ i + 1 ] > v[ max_i ] ) { max_i = i + 1; } }
    return v[ max_i ];
}

typeT inline T cref minmax( T cref tmin, T cref t, T cref tmax ) { return min( max( t, tmin ), tmax ); }

inline dec pythagorean( cdec a, cdec b ) { return ( ( a && b ) ? ( sqrt( square( a ) + square( b ) ) ) : abs( a + b ) ); }

inline uint fibonacci( cuint n ) { uint f[ 3 ] = { 1, 0, 1 }; do_count( n ) { f[ 0 ] = f[ 1 ] + f[ 2 ]; f[ 2 ] = f[ 1 ]; f[ 1 ] = f[ 0 ]; } return f[ 0 ]; }

inline uint factorial( cuint n ) { if( n ) { uint f = 1; for_range( i, n - 1 ) { f *= ( i + 2 ); } return f; } else { return 1; } }

inline uint combinations( cuint n, cuint k, bool ordered ) { return ( factorial( n ) / ( factorial( n - k ) * ( ordered ? 1 : factorial( k ) ) ) ); }

inline uint pascal( cuint row, cuint col )
{
    Assert( col <= row );

    if( !row || !col || ( col == row ) )
    {
        return 1;
    }
    else if( ( col == 1 ) || ( col == ( row - 1 ) ) )
    {
        return row;
    }
    else
    {
        return combinations( row, col, true );
    }
}

inline varray<uint> pascal( cint row )
{
    Assert( !is_neg( row ) );

    varray<uint> v( row + 1 );

    for_range( i, half( row ) + 1 )
    {
        v[ i ] = v[ row - i ] = pascal( row, i );
    }

    return v;
}

inline uint curve_point_count( dec length ) { return max<uint>( 4, min<uint>( 60, ceil( length ) ) ); }

// -- util classes --

classT Min
{

private:

    bool m_set = false;

    T m_min = T( );

public:

    Min( ) { }

    Min( T cref t ) { update( t ); }

    operator T( ) const { return m_min; }

    void reset( ) { m_min = T( ); m_set = false; }
    void update( T cref t ) { m_min = ( m_set ? ::min( m_min, t ) : t ); m_set = true; }
    void update( varray<T> cref v ) { for_each( t, v ) { update( t ); } }

    default_equal( Min );

};

classT Max
{

private:

    bool m_set = false;

    T m_max = T( );

public:

    Max( ) { }

    Max( T cref t ) { update( t ); }

    operator T( ) const { return m_max; }

    void reset( ) { m_max = T( ); m_set = false; }
    void update( T cref t ) { m_max = ( m_set ? ::max( m_max, t ) : t ); m_set = true; }
    void update( varray<T> cref v ) { for_each( t, v ) { update( t ); } }

    default_equal( Max );

};

classT Span
{

private:

    T m_min = T( );
    T m_max = T( );

public:

    Span( ) { }
    Span( T cref bound ) : m_min( bound ), m_max( bound ) { }
    Span( T cref bound1, T cref bound2 ) : m_min( ::min( bound1, bound2 ) ), m_max( ::max( bound1, bound2 ) ) { };

    T cref min( ) const { return m_min; }
    T cref max( ) const { return m_max; }

    T range( ) const { return T( max( ) - min( ) ); }

    template <typename T2>
    Span & operator=( const varray<T2> & v )
    {
        Assert( ( v.size( ) == 1 ) || ( v.size( ) == 2 ), "varray must have exactly 1 or 2 elements" );

        m_min = T( v[ 0 ] );
        m_max = ( v.size( ) == 1 ) ? m_min : T( v[ 1 ] );

        Assert( m_min <= m_max );

        rethis;
    }

    default_equal( Span );

};

classT Slider
{

private:

    T m_value = T( );

    T m_bound_min = T( );
    T m_bound_max = T( );

public:

    Slider( ) { }

    Slider( T cref bound ) : m_bound_min( T( ) ), m_bound_max( bound ), m_value( bound ) { }
    Slider( T cref bound1, T cref bound2 ) : m_bound_min( ( bound1 < bound2 ) ? bound1 : bound2 ), m_bound_max( ( bound1 < bound2 ) ? bound2 : bound1 ), m_value( bound1 ) { }

    T cref value( ) const { return m_value; }
    void value( T cref value, bool set_new_bounds = false )
    {
        // todo Assert( ( set_new_bounds || ( ( value >= min( ) ) && ( value <= max( ) ) ) ), "value must be in range" );

        m_value = value;

        if( set_new_bounds )
        {
            if( m_value > max( ) )
            {
                max( m_value );
            }

            if( m_value < min( ) )
            {
                min( m_value );
            }
        }
    }

    T cref min( ) const { return m_bound_min; }
    void min( T cref bound ) { m_bound_min = bound; if( m_value < m_bound_min ) { value( m_bound_min ); } }

    T cref max( ) const { return m_bound_max; }
    void max( T cref bound ) { m_bound_max = bound; if( m_value > m_bound_max ) { value( m_bound_max ); } }

    T range( ) const { return ( max( ) - min( ) ); }

    dec value_percentage( ) const { return ( (dec)( m_value - m_bound_min ) / (dec)( m_bound_max - m_bound_min ) ); }
    void value_percentage( cdec p, bool set_new_bounds = false ) { value( ( (dec)( m_bound_max - m_bound_min ) * p ) + m_bound_min, set_new_bounds ); }

    void delta( T cref delta, bool set_new_bounds = false ) { value( m_value + delta, set_new_bounds ); }

    default_equal( Slider );

};

class Countdown
{

private:

    uint m_countdown_top = 0;
    uint m_countdown_remaining = 0;

public:

    Countdown( ) { }
    Countdown( cuint countdown ) { reset( countdown ); }

    uint remaining( ) const { return m_countdown_remaining; }
    uint duration( ) const { return m_countdown_top; }

    void duration( uint countdown ) { m_countdown_top = countdown; }

    bool tick( ) { return ( !m_countdown_remaining || !m_countdown_remaining-- ); } // returns true is countdown is finished
    void complete( ) { m_countdown_remaining = 0; }

    void reset( cuint countdown ) { duration( countdown ); reset( ); }
    void reset( ) { m_countdown_remaining = m_countdown_top; }

    default_equal( Countdown );

};

// -- range util functions --

typeT inline bool in_range( T cref t, T cref bound1, T cref bound2, bool include_bound1, bool include_bound2 )
{
    return ( ( bound1 < bound2 ) ?
             ( ( t > bound1 ) && ( t < bound2 ) ) :
             ( ( t > bound2 ) && ( t < bound1 ) ) ) ||
        ( include_bound1 && ( t == bound1 ) ) ||
        ( include_bound2 && ( t == bound2 ) );
}

typeT inline bool in_range( T cref t, T cref bound, bool inclusive = true ) { return in_range<T>( t, T( 0.0 ), bound, inclusive, inclusive ); }
typeT inline bool in_range( T cref t, T cref bound1, T cref bound2, bool inclusive = true ) { return in_range<T>( t, bound1, bound2, inclusive, inclusive ); }
typeT inline bool in_range( T cref t, const Span<T> & span, bool inclusive = true ) { return in_range<T>( t, span.min( ), span.max( ), inclusive, inclusive ); }

// identifiers

class Identifiable
{

private:

    uint m_id = 0;

public:

    Identifiable( ) { static uint total_ids = 0; m_id = ++total_ids; Assert( !is_zero( m_id ) ); }

    uint id( ) const { return m_id; }

    operator uint( ) const { return id( ); }

    default_equal( Identifiable );

};

} // namespace utility
} // namespace axn

#endif /* Utility_hpp */
