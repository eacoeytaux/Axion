#ifndef Utility_hpp
#define Utility_hpp

#include "axn/system/OS.hpp"
#include "axn/system/STD.hpp"
#include "axn/system/STDc.hpp"
#include "axn/system/STDio.hpp"
#include "axn/system/STDptr.hpp"
#include "axn/system/Defines.hpp"

#include "axn/system/Error.hpp"
#include "axn/system/Assert.hpp"

#include "axn/system/Namespaces.hpp"

namespace axn
{

namespace numbers
{

var_const( dec PI = ( 3.141592653589793236926 ) );
var_const( dec TAU = ( 6.28318530717958647692 ) );
var_const( dec RIGHT = ( 1.570796326794896618 ) );

var_const( dec EULER = ( 2.718281828459045235 ) );

} // namespace numbers

typedef dec ticks;

namespace utility
{

typeT inline bool is_null( ptr<T1> cref in_x ) { return( in_x.null( ) ); }

typeT inline void safe_delete( ptr<T1> cref in_x ) { if( !is_null( in_x ) ) { if( cast( T1, in_x ) ) { in_x.del( ); } else { Assert( "bad pointer delete" ); } } }

typeT inline void safe_delete( varray<T1> ref in_x, bool in_shrink = false ) { for_each( x, in_x ) { safe_delete( x ); } in_x.clear( in_shrink ); }
typeT inline void safe_delete( vvarray<T1> ref in_x, bool in_shrink = false ) { for_each( x, in_x ) { for_each( sub_x, x ) { safe_delete( sub_x ); } } in_x.clear( in_shrink ); }
typeT inline void safe_delete( queue<T1> ref in_x ) { while( in_x.size( ) ) { safe_delete( in_x.front( ) ); in_x.pop( ); } }
typeT inline void safe_delete( stack<T1> ref in_x ) { while( in_x.size( ) ) { safe_delete( in_x.top( ) ); in_x.pop( ); } }
typeT inline void safe_delete( list<T1> ref in_x ) { for_each( x, in_x ) { safe_delete( x ); } in_x.clear( ); }
typeT inline void safe_delete( oset<T1> ref in_x ) { for_each( x, in_x ) { safe_delete( x ); } in_x.clear( ); }
typeT inline void safe_delete( uset<T1> ref in_x ) { for_each( x, in_x ) { safe_delete( x ); } in_x.clear( ); }
typeT2 inline void safe_delete( omap<T1, T2> ref in_x ) { for_each( x, in_x ) { safe_delete( x ); } in_x.clear( ); }
typeT2 inline void safe_delete( umap<T1, T2> ref in_x ) { for_each( x, in_x ) { safe_delete( x ); } in_x.clear( ); }

typeT inline bool equal( T1 cref in_x, varray<T1> cref in_varray ) { for_each( x, in_varray ) { return_true_if( x == in_x ); } return( false ); }
typeT inline bool not_equal( T1 cref in_x, varray<T1> cref in_varray ) { for_each( x, in_varray ) { return_false_if( x == in_x ); } return( true ); }

typeT inline T1 cref min( T1 cref in_x1, T1 cref in_x2 ) { return( ( in_x1 < in_x2 ) ? in_x1 : in_x2 ); }
typeT inline T1 cref min( varray<T1> cref in_varray ) { Assert( in_varray.size( ), "varray must not be empty" ); int min_i = 0; for_range( i, in_varray.size( ) - 1 ) { if( in_varray[ i + 1 ] < in_varray[ min_i ] ) { min_i = i + 1; } } return( in_varray[ min_i ] ); }

typeT inline T1 cref max( T1 cref in_x1, T1 cref in_x2 ) { return( ( in_x1 > in_x2 ) ? in_x1 : in_x2 ); }
typeT inline T1 cref max( varray<T1> cref in_varray ) { Assert( in_varray.size( ), "varray must not be empty" ); int max_i = 0; for_range( i, in_varray.size( ) - 1 ) { if( in_varray[ i + 1 ] > in_varray[ max_i ] ) { max_i = i + 1; } } return( in_varray[ max_i ] ); }

typeT inline T1 cref minmax( T1 cref in_x, T1 cref in_min, T1 cref in_max ) { Assert( in_min <= in_max ); return( min( max( in_x, in_min ), in_max ) ); }

typeT inline void swap_values( T1 ref in_x1, T1 ref in_x2 ) { T1 x = in_x1; in_x1 = in_x2; in_x2 = x; }
typeT inline void swap_values_if( bool in_swap, T1 ref in_x1, T1 ref in_x2 ) { if( in_swap ) { swap_values( in_x1, in_x2 ); } }

// -- dec utility functions --

var_const( dec DEFAULT_EPSILON = ( 0.0001 ) );

inline dec add( dec in_x, dec in_y ) { return( in_x + in_y ); }
inline dec subtract( dec in_x, dec in_y ) { return( in_x - in_y ); }
inline dec multiply( dec in_x, dec in_y ) { return( in_x * in_y ); }
inline dec divide( dec in_x, dec in_y ) { return( in_x / in_y ); }

inline bool equal( dec in_x, dec in_y, dec in_e = DEFAULT_EPSILON ) { return( abs( in_x - in_y ) < in_e ); }
inline bool not_equal( dec in_x, dec in_y, dec in_e = DEFAULT_EPSILON ) { return( !equal( in_x, in_y, in_e ) ); }
inline bool greater( dec in_x, dec in_y, dec in_e = DEFAULT_EPSILON ) { return( ( in_x - in_e ) > in_y ); }
inline bool greater( bool in_eq, dec in_x, dec in_y, dec in_e = DEFAULT_EPSILON ) { return( ( ( in_x - in_e ) > in_y ) || ( in_eq && equal( in_x, in_y, in_e ) ) ); }
inline bool greater_or_equal( dec in_x, dec in_y, dec in_e = DEFAULT_EPSILON ) { return( greater( true, in_x, in_y, in_e ) ); }
inline bool less( dec in_x, dec in_y, dec in_e = DEFAULT_EPSILON ) { return( ( in_x + in_e ) < in_y ); }
inline bool less( bool in_eq, dec in_x, dec in_y, dec in_e = DEFAULT_EPSILON ) { return( ( ( in_x + in_e ) < in_y ) || ( in_eq && equal( in_x, in_y, in_e ) ) ); }
inline bool less_or_equal( dec in_x, dec in_y, dec in_e = DEFAULT_EPSILON ) { return( less( true, in_x, in_y, in_e ) ); }

inline bool is_inf( dec in_x ) { return( equal( in_x, { INFINITY, -INFINITY } ) ); }
inline bool is_nan( dec in_x ) { return( ::isnan( in_x ) ); }
inline bool is_num( dec in_x ) { return( !is_nan( in_x ) && !is_inf( in_x ) ); }
inline bool is_int( dec in_x ) { return( equal( in_x, ::rint( in_x ) ) ); }
inline bool is_zero( dec in_x ) { return( equal( in_x, 0.0 ) ); }
inline bool is_one( dec in_x ) { return( equal( in_x, 1.0 ) ); }
inline bool is_pos( dec in_x ) { return( greater( in_x, 0.0 ) ); }
inline bool is_neg( dec in_x ) { return( less( in_x, 0.0 ) ); }
inline bool is_even( dec in_x ) { return( is_zero( int( in_x ) % 2 ) ); }
inline bool is_odd( dec in_x ) { return( is_zero( int( in_x + 1 ) % 2 ) ); }

inline bool is_divisible( dec in_x, dec in_y ) { return( !is_zero( in_x ) && !is_zero( in_y ) && is_zero( remainder( in_x, in_y ) ) ); }

inline bool is_prime( dec in_x ) { return_true_if( in_x == 2 ); return_false_if( !is_int( in_x ) || ( in_x < 2 ) || !is_odd( in_x ) ); for_range( i, floor( sqrt( in_x ) ) - 1 ) { return_false_if( is_divisible( in_x, i + 2 ) ); } return( true ); }
inline bool is_composite( dec in_x ) { return( is_int( in_x ) && ( in_x > 2 ) && !is_prime( in_x ) ); }

// -- misc. utility functions --

typeT inline T1 inverse( T1 cref in_x ) { return( T1( 1.0 / in_x ) ); }

typeT inline T1 negated( T1 cref in_x, bool in_negate = true ) { return( in_negate ? -in_x : in_x ); }

typeT inline T1 x2( T1 cref in_x ) { return( T1( in_x * 2.0 ) ); }
typeT inline T1 half( T1 cref in_x ) { return( T1( in_x / 2.0 ) ); }

typeT inline T1 one_plus( T1 cref in_x ) { return( T1( 1.0 + in_x ) ); }
typeT inline T1 one_minus( T1 cref in_x ) { return( T1( 1.0 - in_x ) ); }

typeT inline T1 root( T1 cref in_x, T1 cref in_root ) { return( T1( pow( in_x, inverse( in_root ) ) ) ); }

typeT inline T1 square( T1 cref in_x ) { return( in_x * in_x ); }
typeT inline T1 cube( T1 cref in_x ) { return( in_x * in_x * in_x ); }

typeT inline T1 log( T1 cref in_x ) { return( T1( ::log( dec( in_x ) ) ) ); }
typeT inline T1 log( T1 cref in_x, T1 cref in_base ) { return( T1( ::log( dec( in_x ) ) / ::log( dec( in_base ) ) ) ); }

typeT inline T1 mod( T1 cref in_x, T1 cref in_y, bool in_floored = false ) { return( T1( fmod( dec( in_x ), dec( in_y ) ) ) ); } // TODO make use of floored https://www.geeksforgeeks.org/c/modulus-on-negative-numbers/

inline dec pythagorean( dec in_a, dec in_b ) { return( ( !is_zero( in_a ) && !is_zero( in_b ) ) ? ( sqrt( square( in_a ) + square( in_b ) ) ) : abs( in_a + in_b ) ); }

inline dec fibonacci( uint in_n ) { if( in_n ) { uint f[ 3 ] = { 1, 0, 1 }; do_count( in_n + 1 ) { f[ 0 ] = f[ 1 ] + f[ 2 ]; f[ 2 ] = f[ 1 ]; f[ 1 ] = f[ 0 ]; } return( f[ 0 ] ); } else { return( 1 ); } }

inline dec factorial( uint in_n ) { if( in_n ) { uint f = 1; for_range( i, in_n - 1 ) { f *= ( i + 2 ); } return( f ); } else { return( 1 ); } }
inline dec additorial( uint in_n ) { return( half( in_n * ( in_n + 1 ) ) ); } // factorial but with addition

inline dec permutations( uint in_n, uint in_k ) { return( factorial( in_n ) / factorial( in_n - in_k ) ); }
inline dec combinations( uint in_n, uint in_k ) { return( permutations( in_n, in_k ) / factorial( in_k ) ); }

inline dec pascal_triangle( uint in_row, uint in_col ) { Assert( in_col <= in_row ); if( is_zero( in_row ) || is_zero( in_col ) || ( in_col == in_row ) ) { return( 1 ); } else if( is_one( in_col ) || ( in_col == ( in_row - 1 ) ) ) { return( in_row ); } else { return( combinations( in_row, in_col ) ); } }
inline varray<dec> pascal_triangle( uint in_row ) { varray<dec> v( in_row + 1 ); for_range( i, half( in_row ) + 1 ) { v[ i ] = v[ in_row - i ] = pascal_triangle( in_row, i ); } return( v ); }

// -- utility classes --

enum direction
{
    NO_DIRECTION = 0,
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
    DIRECTIONS
};

class Identifiable
{

private:

    uint m_id = 0;

public:

    Identifiable( ) { static uint sids = 0; m_id = ++sids; Assert( is_pos( m_id ) ); }

    uint id( ) const { return( m_id ); }

    operator uint( ) const { return( id( ) ); }

    default_equal( Identifiable );

};

classT MinMax
{

private:

    bool m_set = false;

    T1 m_x = T1( );

protected:

    MinMax( ) { }

    MinMax( T1 cref in_x ) { update( in_x ); }

    virtual T1 cref choose( T1 cref in_x1, T1 cref in_x2 ) const = 0;

public:

    T1 cref value( ) const { return( m_x ); }

    void reset( ) { m_x = T1( ); m_set = false; }

    void update( T1 cref in_x ) { m_x = ( m_set ? choose( m_x, in_x ) : in_x ); m_set = true; }
    void update( varray<T1> cref in_varray ) { for_each( x, in_varray ) { update( x ); } }

    operator T1( ) const { return( value( ) ); }

    default_equal( MinMax );

};

classT Min : public MinMax<T1>{ public: Min( ) { } Min( T1 cref in_x ) : MinMax<T1>( in_x ) { } private: virtual T1 cref choose( T1 cref in_x1, T1 cref in_x2 ) const override { return( min( in_x1, in_x2 ) ); } default_equal( Min ); };
classT Max : public MinMax<T1>{ public: Max( ) { } Max( T1 cref in_x ) : MinMax<T1>( in_x ) { } private: virtual T1 cref choose( T1 cref in_x1, T1 cref in_x2 ) const override { return( max( in_x1, in_x2 ) ); } default_equal( Max ); };

classT Span
{

private:

    T1 m_min = T1( );
    T1 m_max = T1( );

public:

    Span( ) { }

    Span( T1 cref in_bound ) : m_min( in_bound ), m_max( in_bound ) { }
    Span( T1 cref in_bound1, T1 cref in_bound2 ) : m_min( ::min( in_bound1, in_bound2 ) ), m_max( ::max( in_bound1, in_bound2 ) ) { };

    T1 cref min( ) const { return( m_min ); }
    T1 cref max( ) const { return( m_max ); }

    T1 range( ) const { return( T1( max( ) - min( ) ) ); }

    templated( typename T2 )
    Span ref operator=( varray<T2> cref in_varray )
    {
        Assert( is_one( in_varray.size( ) ) || ( in_varray.size( ) == 2 ), "varray must have exactly 1 or 2 elements" );

        m_min = T1( in_varray[ 0 ] );
        m_max = is_one( in_varray.size( ) ) ? m_min : T1( in_varray[ 1 ] );

        Assert( m_min <= m_max );

        rethis;
    }

    default_equal( Span );

};

classT Slider
{

private:

    T1 m_value = T1( );

    T1 m_bound_min = T1( );
    T1 m_bound_max = T1( );

public:

    Slider( ) { }

    Slider( T1 cref in_bound ) : m_bound_min( T1( ) ), m_bound_max( in_bound ), m_value( in_bound ) { }
    Slider( T1 cref in_bound1, T1 cref in_bound2 ) : m_bound_min( ( in_bound1 < in_bound2 ) ? in_bound1 : in_bound2 ), m_bound_max( ( in_bound1 < in_bound2 ) ? in_bound2 : in_bound1 ), m_value( in_bound1 ) { }

    T1 cref value( ) const { return( m_value ); }
    void value( T1 cref in_value, bool in_set_new_bounds = false )
    {
        // todo Assert( ( set_new_bounds || ( ( value >= min( ) ) && ( value <= max( ) ) ) ), "value must be in range" );

        m_value = in_value;

        if( in_set_new_bounds )
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

    T1 cref min( ) const { return( m_bound_min ); }
    void min( T1 cref in_bound ) { m_bound_min = in_bound; if( m_value < m_bound_min ) { value( m_bound_min ); } }

    T1 cref max( ) const { return( m_bound_max ); }
    void max( T1 cref in_bound ) { m_bound_max = in_bound; if( m_value > m_bound_max ) { value( m_bound_max ); } }

    T1 range( ) const { return( max( ) - min( ) ); }

    dec value_percentage( ) const { return( dec( m_value - m_bound_min ) / dec( m_bound_max - m_bound_min ) ); }
    void value_percentage( dec in_perc, bool in_set_new_bounds = false ) { value( ( dec( m_bound_max - m_bound_min ) * in_perc ) + m_bound_min, in_set_new_bounds ); }

    void delta( T1 cref in_delta, bool in_set_new_bounds = false ) { value( m_value + in_delta, in_set_new_bounds ); }

    default_equal( Slider );

};

class Countdown
{

private:

    ticks m_countdown_top = 0;
    ticks m_countdown_remaining = 0;

public:

    Countdown( ) { }

    Countdown( ticks in_countdown ) { reset( in_countdown ); }

    dec percentage( ) const { return( is_zero( m_countdown_top ) ? 0.0 : dec( m_countdown_remaining ) / dec( m_countdown_top ) ); }

    ticks remaining( ) const { return( m_countdown_remaining ); }
    ticks duration( ) const { return( m_countdown_top ); }

    void duration( ticks in_countdown ) { m_countdown_top = in_countdown; }

    void complete( ) { m_countdown_remaining = 0; }
    bool completed( ) const { return( is_zero( m_countdown_remaining ) ); }

    void reset( ticks in_countdown ) { duration( in_countdown ); reset( ); }
    void reset( ) { m_countdown_remaining = m_countdown_top; }

    bool tick( ticks in_ticks = 1.0 ) { return( !is_pos( m_countdown_remaining -= min( in_ticks, m_countdown_remaining ) ) ); } // returns true is countdown is finished

    default_equal( Countdown );

};

// -- range utility functions --

typeT inline bool in_range( T1 cref in_x, T1 cref in_bound1, T1 cref in_bound2, bool in_include_bound1, bool in_include_bound2 )
{
    return( ( in_include_bound1 && ( in_x == in_bound1 ) ) ||
            ( in_include_bound2 && ( in_x == in_bound2 ) ) ||
            ( ( in_bound1 < in_bound2 ) ?
              ( ( in_x > in_bound1 ) && ( in_x < in_bound2 ) ) :
              ( ( in_x > in_bound2 ) && ( in_x < in_bound1 ) ) ) );
}

typeT inline bool in_range( T1 cref in_x, T1 cref in_bound, bool in_inclusive = true ) { return( in_range<T1>( in_x, T1( 0.0 ), in_bound, in_inclusive, in_inclusive ) ); }
typeT inline bool in_range( T1 cref in_x, T1 cref in_bound1, T1 cref in_bound2, bool in_inclusive = true ) { return( in_range<T1>( in_x, in_bound1, in_bound2, in_inclusive, in_inclusive ) ); }
typeT inline bool in_range( T1 cref in_x, Span<T1> cref in_span, bool in_inclusive = true ) { return( in_range<T1>( in_x, in_span.min( ), in_span.max( ), in_inclusive, in_inclusive ) ); }

typeT inline bool in_range_0_to_1( T1 cref in_x, bool in_inclusive = true ) { return( in_range<T1>( in_x, 0.0, 1.0, in_inclusive, in_inclusive ) ); }

} // namespace utility
} // namespace axn

#endif /* Utility_hpp */
