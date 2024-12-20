#ifndef Utility_hpp
#define Utility_hpp

#include "OS.hpp"
#include "STD.hpp"
#include "STDc.hpp"
#include "STDptr.hpp"

#include "Error.hpp"
#include "Assert.hpp"

#include "Namespaces.hpp"

namespace axn
{
namespace utility
{

// -- defines --

#define Xc const &

#define typeT template <typename T>
#define classT template <typename T> class

#define rethis return *this

#define return_if( ret, f ) \
    if( f ) { return ret; }

#define do_until_break( ) \
    while( true )

#define do_count( x ) \
    for( uint do_counter = 0; do_counter < x; ++do_counter)

#define for_range( i, range )   \
    for( uint i = 0; i < range; ++i )

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
    bool operator==( const Class & c ) const = default; \
    bool operator!=( const Class & c ) const { return !( *this == c ); }

#define default_non_equal( Class ) \
    bool operator!=( const Class & c ) const { return !( *this == c ); }

#define default_virtual_equal( Class ) \
    virtual bool operator==( const Class & c ) const = default; \
    virtual bool operator!=( const Class & c ) const { return !( *this == c ); }

#define default_virtual_non_equal( Class ) \
    virtual bool operator!=( const Class & c ) const { return !( *this == c ); }

inline void nothing( ) { }

typeT inline void safe_delete( T *& t ) { if( t && dynamic_cast<T *>( t ) ) { delete t; } t = nullptr; }

// -- dec util functions --

const dec DEFAULT_EPSILON = 0.0001;

inline bool equal( const dec x, const dec y, const dec e = DEFAULT_EPSILON ) { return ( abs( x - y ) < e ); }
inline bool greater( const dec x, const dec y, const dec e = DEFAULT_EPSILON ) { return ( ( x - e ) > y ); }
inline bool greater( bool eq, const dec x, const dec y, const dec e = DEFAULT_EPSILON ) { return ( ( ( x - e ) > y ) || ( eq && equal( x, y, e ) ) ); }
inline bool greater_or_equal( const dec x, const dec y, const dec e = DEFAULT_EPSILON ) { return greater( true, x, y, e ); }
inline bool less( const dec x, const dec y, const dec e = DEFAULT_EPSILON ) { return ( ( x + e ) < y ); }
inline bool less( bool eq, const dec x, const dec y, const dec e = DEFAULT_EPSILON ) { return ( ( ( x + e ) < y ) || ( eq && equal( x, y, e ) ) ); }
inline bool less_or_equal( const dec x, const dec y, const dec e = DEFAULT_EPSILON ) { return less( true, x, y, e ); }

inline bool is_inf( const dec d ) { return ( d == INFINITY_POS ) || ( d == INFINITY_NEG ); }
inline bool is_nan( const dec d ) { return ( ::isnan( d ) ); }
inline bool is_num( const dec d ) { return ( !is_nan( d ) && !is_inf( d ) ); }
inline bool is_int( const dec d ) { return ( equal( d, ::rint( d ) ) ); }
inline bool is_zero( const dec d ) { return ( equal( d, ZERO ) ); }
inline bool is_pos( const dec d ) { return greater( d, ZERO ); }
inline bool is_neg( const dec d ) { return less( d, ZERO ); }
inline bool is_even( const dec d ) { return is_zero( (int)( d ) % 2 ); }
inline bool is_odd( const dec d ) { return is_zero( (int)( d + 1 ) % 2 ); }

inline bool is_divisible( const dec x, const dec y ) { return !is_zero( remainder( x, y ) ); }

inline dec remainder_positive( const dec x, const dec y ) { dec r = remainder( x, y ); while( less( r, ZERO ) ) { r += y; } return r; }

// -- misc. util functions --

typeT inline void swap_values( T & t1, T & t2 ) { T t = t1; t1 = t2; t2 = t; }

typeT inline T inverse( const T & t ) { return ( ONE / t ); }

typeT inline T negate( const T & t ) { return -t; }
typeT inline T negate( const T & t, bool f ) { return ( f ? -t : t ); }

typeT inline T half( const T & t ) { return ( t / TWO ); }
typeT inline T twice( const T & t ) { return ( t * TWO ); }

typeT inline T square( const T & t ) { return ( t * t ); }
typeT inline T cube( const T & t ) { return ( t * t * t ); }

typeT inline T log( const T & base, const T & t ) { return ( ::log( t ) / ::log( base ) ); }

typeT inline const T & min( const T & t1, const T & t2 ) { return ( t1 < t2 ) ? t1 : t2; }
typeT inline const T & min( const varray<T> & list )
{
    Assert( list.size( ), "list must not be empty" );

    int min_i = ZERO;

    for_range( i, list.size( ) - ONE )
    {
        if( list[ i + ONE ] < list[ min_i ] )
        {
            min_i = i + ONE;
        }
    }

    return list[ min_i ];
}

typeT inline const T & max( const T & t1, const T & t2 ) { return ( t1 > t2 ) ? t1 : t2; }
typeT inline const T & max( const varray<T> & list )
{
    Assert( list.size( ), "list must not be empty" );

    int max_i = ZERO;

    for_range( i, list.size( ) - ONE )
    {
        if( list[ i + ONE ] > list[ max_i ] )
        {
            max_i = i + ONE;
        }
    }

    return list[ max_i ];
}

inline uint factorial( const uint n ) { if( n ) { uint f = ONE; for_range( i, n - ONE ) { f *= ( i + TWO ); } return f; } else { return ONE; } }

inline uint combinations( const uint n, const uint k, bool ordered ) { return ( factorial( n ) / ( factorial( n - k ) * ( ordered ? ONE : factorial( k ) ) ) ); }

inline uint pascal( const uint row, const uint col )
{
    Assert( col <= row );

    if( !row || !col || ( col == row ) )
    {
        return ONE;
    }
    else if( ( col == ONE ) || ( col == ( row - ONE ) ) )
    {
        return row;
    }
    else
    {
        return combinations( row, col, true );
    }
}

inline varray<uint> pascal( const int row )
{
    Assert( !is_neg( row ) );

    varray<uint> v( row + ONE );

    for_range( i, half( row ) + ONE )
    {
        v[ i ] = v[ row - i ] = pascal( row, i );
    }

    return v;
}

inline uint fibonacci( const uint n )
{
    Assert( !is_neg( n ) );

    if( n > ONE )
    {
        uint f;

        uint f1 = ZERO;
        uint f2 = ONE;

        do_count( n )
        {
            f = f1 + f2;
            f2 = f1;
            f1 = f;
        }

        return f;
    }
    else
    {
        return ONE;
    }
}

inline dec pythagorean( const dec a, const dec b )
{
    if( a && b )
    {
        return sqrt( square( a ) + square( b ) );
    }
    else
    {
        return abs( a + b );
    }
}

// -- util classes --

class Counter
{
private:
    uint m_countdown_top = ZERO;
    uint m_countdown_remaining = ZERO;

public:
    Counter( ) { }
    Counter( const uint countdown ) { reset( countdown ); }

    bool tick( ) { if( m_countdown_remaining ) { return !m_countdown_remaining--; } else { return true; } }

    void set( const uint countdown ) { m_countdown_top = countdown; }
    void reset( const uint countdown ) { set( countdown ); reset( ); }
    void reset( ) { m_countdown_remaining = m_countdown_top; }

    uint remaining( ) const { return m_countdown_remaining; }

    default_equal( Counter );
};

classT Span
{
private:
    T m_min = T( ZERO );
    T m_max = T( ZERO );

public:
    Span( ) { }
    Span( const T & bound ) : m_min( bound ), m_max( bound ) { }
    Span( const T & bound1, const T & bound2 ) : m_min( ::min( bound1, bound2 ) ), m_max( ::max( bound1, bound2 ) ) { };

    const T & min( ) const { return m_min; }
    const T & max( ) const { return m_max; }

    T range( ) const { return T( max( ) - min( ) ); }

    template <typename T2>
    Span & operator=( const varray<T2> & v )
    {
        Assert( ( v.size( ) == ONE ) || ( v.size( ) == TWO ), "varray must have exactly 1 or 2 elements" );

        m_min = T( v[ ZERO ] );
        m_max = ( v.size( ) == ONE ) ? m_min : T( v[ ONE ] );

        Assert( m_min < m_max );

        rethis;
    }

    default_equal( Span );
};

classT Slider
{
private:
    T m_value = T( ZERO );

    T m_bound_min = T( ZERO );
    T m_bound_max = T( ZERO );

public:
    Slider( ) { }
    Slider( const T & bound1, const T & bound2 = ZERO ) : m_bound_min( ( bound1 < bound2 ) ? bound1 : bound2 ), m_bound_max( ( bound1 < bound2 ) ? bound2 : bound1 ), m_value( bound1 ) { }

    const T & value( ) const { return m_value; }
    void value( const T & value, bool set_new_bounds = false )
    {
        Assert( ( set_new_bounds || ( ( value >= min( ) ) && ( value <= max( ) ) ) ), "value must be in range" );

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

    const T & min( ) const { return m_bound_min; }
    void min( const T & bound ) { m_bound_min = bound; if( m_value < m_bound_min ) { value( m_bound_min ); } }

    const T & max( ) const { return m_bound_max; }
    void max( const T & bound ) { m_bound_max = bound; if( m_value > m_bound_max ) { value( m_bound_max ); } }

    T range( ) const { return ( max( ) - min( ) ); }

    dec value_percentage( ) const { return ( (dec)( m_value - m_bound_min ) / (dec)( m_bound_max - m_bound_min ) ); }
    void value_percentage( const dec p, bool set_new_bounds = false ) { value( ( (dec)( m_bound_max - m_bound_min ) * p ) + m_bound_min, set_new_bounds ); }

    void delta( const T & delta, bool set_new_bounds = false ) { value( m_value + delta, set_new_bounds ); }

    default_equal( Slider );
};

// -- range util functions --

typeT inline bool in_range( const T & t, const T & low, const T & high, bool include_low, bool include_high )
{
    return ( ( low < high ) ?
             ( ( t > low ) && ( t < high ) ) :
             ( ( t > high ) && ( t < low ) ) ) ||
        ( include_low && ( t == low ) ) ||
        ( include_high && ( t == high ) );
}

typeT inline bool in_range( const T & t, const T & high, bool inclusive = true ) { return in_range<T>( t, T( ZERO ), high, inclusive, inclusive ); }
typeT inline bool in_range( const T & t, const T & low, const T & high, bool inclusive = true ) { return in_range<T>( t, low, high, inclusive, inclusive ); }
typeT inline bool in_range( const T & t, const Span<T> & span, bool inclusive = true ) { return in_range<T>( t, span.min( ), span.max( ), inclusive, inclusive ); }

// identifiers

class Identifiable
{
private:
    uint m_id = ZERO;

public:
    struct Hasher { size_t operator( )( const Identifiable & i ) const { return hash<uint>( )( i.id( ) ); } };

    Identifiable( ) { static uint total_ids = ZERO; m_id = ++total_ids; Assert( !is_zero( m_id ) ); }

    uint id( ) const { return m_id; }
    operator uint( ) const { return m_id; }

    default_equal( Identifiable );
};

class IDoset : public oset<Identifiable, Identifiable::Hasher> { };
class IDuset : public uset<Identifiable, Identifiable::Hasher> { };

classT IDomap : public omap<Identifiable, T, Identifiable::Hasher>{ };
classT IDumap : public umap<Identifiable, T, Identifiable::Hasher>{ };

} // namespace utility
} // namespace axn

#endif /* Utility_hpp */
