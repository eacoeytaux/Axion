#ifndef Utility_hpp
#define Utility_hpp

#include "axn/system/OS.hpp"
#include "axn/system/STD.hpp"
#include "axn/system/STDc.hpp"
#include "axn/system/STDptr.hpp"
#include "axn/system/Defines.hpp"
#include "axn/system/Namespaces.hpp"
#include "axn/system/Assert.hpp"
#include "axn/system/Error.hpp"

namespace axn
{

namespace numbers
{

var_const( dec PI = ( 3.141592653589793236926 ) );
var_const( dec TAU = ( 6.28318530717958647692 ) );
var_const( dec RIGHT = ( 1.570796326794896618 ) );

var_const( dec EULER = ( 2.718281828459045235 ) );

} // namespace numbers

namespace utility
{

typeT inline T1 copy( T1 cref in_x ) { return( T1( in_x ) ); }

typeT inline bool is_null( ptr<T1> cref in_x ) { return( in_x.null( ) ); }

typeT inline void safe_delete( ptr<T1> cref in_x ) { if( !is_null( in_x ) ) { if( cast( in_x, T1 ) ) { delete( in_x ); } else { Assert( "bad pointer delete" ); } } }

typeT inline void safe_delete( varray<T1> ref in_x, bool in_shrink = false ) { for_each( x, in_x ) { safe_delete( x ); } in_x.clear( in_shrink ); }
typeT inline void safe_delete( vvarray<T1> ref in_x, bool in_shrink = false ) { for_each( x, in_x ) { for_each( sub_x, x ) { safe_delete( sub_x ); } } in_x.clear( in_shrink ); }
typeT inline void safe_delete( queue<T1> ref in_x ) { while( in_x.size( ) ) { safe_delete( in_x.front( ) ); in_x.pop( ); } }
typeT inline void safe_delete( stack<T1> ref in_x ) { while( in_x.size( ) ) { safe_delete( in_x.top( ) ); in_x.pop( ); } }
typeT inline void safe_delete( list<T1> ref in_x ) { for_each( x, in_x ) { safe_delete( x ); } in_x.clear( ); }
typeT inline void safe_delete( oset<T1> ref in_x ) { for_each( x, in_x ) { safe_delete( x ); } in_x.clear( ); }
typeT inline void safe_delete( uset<T1> ref in_x ) { for_each( x, in_x ) { safe_delete( x ); } in_x.clear( ); }
typeT2 inline void safe_delete( omap<T1, T2> ref in_x ) { for_each( x, in_x ) { safe_delete( x ); } in_x.clear( ); }
typeT2 inline void safe_delete( umap<T1, T2> ref in_x ) { for_each( x, in_x ) { safe_delete( x ); } in_x.clear( ); }

typeT inline bool is_empty( T1 cref in_x ) { return( in_x.empty( ) ); }

typeT inline bool equal( T1 cref in_x, varray<T1> cref in_varray ) { for_each( x, in_varray ) { return_true_if( x == in_x ); } return( false ); }
typeT inline bool not_equal( T1 cref in_x, varray<T1> cref in_varray ) { for_each( x, in_varray ) { return_false_if( x == in_x ); } return( true ); }

typeT inline T1 cref min( T1 cref in_x1, T1 cref in_x2 ) { return( ( in_x1 < in_x2 ) ? in_x1 : in_x2 ); }
typeT inline T1 cref min( varray<T1> cref in_varray ) { Assert( !is_empty( in_varray ), "varray must not be empty" ); int min_i = 0; for_range( i, in_varray.size( ) - 1 ) { if( in_varray[ i + 1 ] < in_varray[ min_i ] ) { min_i = i + 1; } } return( in_varray[ min_i ] ); }

typeT inline T1 cref max( T1 cref in_x1, T1 cref in_x2 ) { return( ( in_x1 > in_x2 ) ? in_x1 : in_x2 ); }
typeT inline T1 cref max( varray<T1> cref in_varray ) { Assert( !is_empty( in_varray ), "varray must not be empty" ); int max_i = 0; for_range( i, in_varray.size( ) - 1 ) { if( in_varray[ i + 1 ] > in_varray[ max_i ] ) { max_i = i + 1; } } return( in_varray[ max_i ] ); }

typeT inline T1 cref minmax( T1 cref in_x, T1 cref in_min, T1 cref in_max ) { Assert( in_min <= in_max ); return( min( max( in_x, in_min ), in_max ) ); }

typeT inline void swap_values( T1 ref in_x1, T1 ref in_x2 ) { T1 x = in_x1; in_x1 = in_x2; in_x2 = x; }
typeT inline void swap_values_if( bool in_swap, T1 ref in_x1, T1 ref in_x2 ) { if( in_swap ) { swap_values( in_x1, in_x2 ); } }

// -- math utility functions -- //

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

// truncated (default) keeps the sign of in_x like fmod: mod( -1, 4 ) = -1, floored keeps the sign of in_y: mod( -1, 4, true ) = 3
typeT inline T1 mod( T1 cref in_x, T1 cref in_y, bool in_floored = false ) { dec result = fmod( dec( in_x ), dec( in_y ) ); if( in_floored && ( result != 0.0 ) && ( ( result < 0.0 ) != ( dec( in_y ) < 0.0 ) ) ) { result += dec( in_y ); } return( T1( result ) ); }

inline dec pythagorean( dec in_a, dec in_b ) { return( ( !is_zero( in_a ) && !is_zero( in_b ) ) ? ( sqrt( square( in_a ) + square( in_b ) ) ) : abs( in_a + in_b ) ); }

inline dec fibonacci( uint in_n ) { if( in_n ) { uint f[ 3 ] = { 1, 0, 1 }; do_count( in_n + 1 ) { f[ 0 ] = f[ 1 ] + f[ 2 ]; f[ 2 ] = f[ 1 ]; f[ 1 ] = f[ 0 ]; } return( f[ 0 ] ); } else { return( 1 ); } }

inline dec factorial( uint in_n ) { if( in_n ) { uint f = 1; for_range( i, in_n - 1 ) { f *= ( i + 2 ); } return( f ); } else { return( 1 ); } }
inline dec additorial( uint in_n ) { return( half( in_n * ( in_n + 1 ) ) ); } // factorial but with addition

inline dec permutations( uint in_n, uint in_k ) { return( factorial( in_n ) / factorial( in_n - in_k ) ); }
inline dec combinations( uint in_n, uint in_k ) { return( permutations( in_n, in_k ) / factorial( in_k ) ); }

inline dec pascal_triangle( uint in_row, uint in_col ) { Assert( in_col <= in_row ); if( is_zero( in_row ) || is_zero( in_col ) || ( in_col == in_row ) ) { return( 1 ); } else if( is_one( in_col ) || ( in_col == ( in_row - 1 ) ) ) { return( in_row ); } else { return( combinations( in_row, in_col ) ); } }
inline varray<dec> pascal_triangle( uint in_row ) { varray<dec> v( in_row + 1 ); for_range( i, half( in_row ) + 1 ) { v[ i ] = v[ in_row - i ] = pascal_triangle( in_row, i ); } return( v ); }

inline dec quadratic( dec in_a, dec in_b, dec in_c, bool in_plus_or_minus ) { return( !is_zero( in_a ) ? ( ( -in_b - negated( ( square( in_b ) - ( 4.0 * in_a * in_c ) ), in_plus_or_minus ) ) / x2( in_a ) ) : ( !is_zero( in_b ) ? ( -in_c / in_b ) : ( 0.0 ) ) ); }

// -- Stringable -- //

#define strinfo( labelstr, ... ) public: virtual string label( ) const override { return( #labelstr ); } virtual string stringified( ) const override { return( "[ " + string( __VA_ARGS__ ) + " ]" ); } macro_end

class Stringable
{

protected:

    Stringable( ) { }

public:

    static bool labeling;

    operator string( ) const { return( ( labeling ? label( ) : nostring ) + stringified( ) ); }

    virtual string label( ) const = 0;

    virtual string stringified( ) const = 0;
    virtual string stringified_with_space( string in_space = ( " " ) ) const { string spaced_str = stringified( ); if( spaced_str.length( ) ) { spaced_str += in_space; } return( spaced_str ); }

    equal_default( Stringable );

};

var_const( string DEFAULT_STRINGIFY_DELIM = ( ", " ) );

inline string stringify( bool in_b ) { return( in_b ? "true" : "false" ); }

inline string stringify( int in_x ) { return( to_string( in_x ) ); }
inline string stringify( uint in_x ) { return( to_string( in_x ) ); }
inline string stringify( dec in_x ) { return( to_string( in_x ) ); }

inline string stringify( Stringable cref in_stringable ) { return( in_stringable.stringified( ) ); }

inline string stringify( varray<string> cref in_strings, string cref in_delim = DEFAULT_STRINGIFY_DELIM ) { string str; for_range( i, in_strings.size( ) ) { if( i ) { str += in_delim; } str += in_strings[ i ]; } return( str ); }
inline string stringify( vvarray<string> cref in_strings, string cref in_delim = DEFAULT_STRINGIFY_DELIM ) { string str; for_range( i, in_strings.size( ) ) { if( i ) { str += in_delim; } str += "[ " + stringify( in_strings[ i ], in_delim ) + " ]"; } return( str ); }
inline string stringify( queue<string> cref in_strings, string cref in_delim = DEFAULT_STRINGIFY_DELIM ) { string str; queue<string> strs = in_strings; for_range( i, strs.size( ) ) { if( i ) { str += in_delim; } str += strs.pop( ); } return( str ); }
inline string stringify( stack<string> cref in_strings, string cref in_delim = DEFAULT_STRINGIFY_DELIM ) { string str;  stack<string> strs = in_strings; for_range( i, strs.size( ) ) { if( i ) { str += in_delim; } str += strs.pop( ); } return( str ); }
inline string stringify( list<string> cref in_strings, string cref in_delim = DEFAULT_STRINGIFY_DELIM ) { string str; for_range( i, in_strings.size( ) ) { if( i ) { str += in_delim; } str += in_strings[ i ]; } return( str ); }

inline const char * charify( string in_string ) { return( in_string.c_str( ) ); }

// -- Identifiable -- //

class Identifiable : public Stringable
{

    strinfo( ID, "id: " + stringify( id( ) ) );

private:

    const uint m_id;

    static uint total_ids;
    static uint total_types;

public:

    Identifiable( ) : m_id( ++total_ids ) { Assert( is_pos( m_id ) ); }

    uint id( ) const { return( m_id ); }

    operator uint( ) const { return( id( ) ); }

    typeT static uint type( ) { static uint type = ( ++total_types ); return( type ); }
    typeT static uint type( T1 cref in_x ) { return( type<T1>( ) ); }
    typeT static uint type( ptr<T1> in_x ) { return( type<T1>( ) ); }

    equal_default( Identifiable );

};

// -- utility classes -- //

class and_bool
{

private:

    bool m_bool;

public:

    and_bool( bool in_bool = true ) : m_bool( in_bool ) { }

    operator bool( ) const { return( m_bool ); }

    and_bool ref operator=( bool in_bool ) { m_bool = m_bool && in_bool; rethis; }

    equal_default( and_bool );

};

class or_bool
{

private:

    bool m_bool;

public:

    or_bool( bool in_bool = false ) : m_bool( in_bool ) { }

    operator bool( ) const { return( m_bool ); }

    or_bool ref operator=( bool in_bool ) { m_bool = m_bool || in_bool; rethis; }

    equal_default( or_bool );

};

template( uint N ) class int_mod
{

private:

    int m_int;

public:

    int_mod( int in_int = 0 ) : m_int( in_int ) { }

    operator int( ) const { return( m_int % N ); }

    int_mod ref operator=( int in_int ) { m_int = in_int; rethis; }

    equal_default( int_mod );

};

classT span : public Stringable
{

    strinfo( span, "min: " + stringify( min( ) ) + " max: " + stringify( max( ) ) );

private:

    T1 m_min = T1( );
    T1 m_max = T1( );

public:

    span( ) { }

    span( T1 cref in_bound ) : m_min( in_bound ), m_max( in_bound ) { }
    span( T1 cref in_bound1, T1 cref in_bound2 ) : m_min( ::min( in_bound1, in_bound2 ) ), m_max( ::max( in_bound1, in_bound2 ) ) { };

    T1 cref min( ) const { return( m_min ); }
    T1 cref max( ) const { return( m_max ); }

    T1 range( ) const { return( T1( max( ) - min( ) ) ); }

    template( typename T2 ) span ref operator=( varray<T2> cref in_varray )
    {
        Assert( is_one( in_varray.size( ) ) || ( in_varray.size( ) == 2 ), "varray must have exactly 1 or 2 elements" );

        m_min = T1( in_varray[ 0 ] );
        m_max = is_one( in_varray.size( ) ) ? m_min : T1( in_varray[ 1 ] );

        Assert( m_min <= m_max );

        rethis;
    }

    equal_default( span );

};

classT slider : public Stringable
{

    strinfo( span, "min: " + stringify( min( ) ) + " value: " + stringify( value( ) ) + " max: " + stringify( max( ) ) );

private:

    T1 m_value = T1( );

    T1 m_bound_min = T1( );
    T1 m_bound_max = T1( );

public:

    slider( ) { }

    slider( T1 cref in_bound ) : m_bound_min( T1( ) ), m_bound_max( in_bound ), m_value( in_bound ) { }
    slider( T1 cref in_bound1, T1 cref in_bound2 ) : m_bound_min( ( in_bound1 < in_bound2 ) ? in_bound1 : in_bound2 ), m_bound_max( ( in_bound1 < in_bound2 ) ? in_bound2 : in_bound1 ), m_value( in_bound1 ) { }

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

    equal_default( slider );

};

classT MinOrMax : public Stringable
{

    strinfo( MinOrMax, "value: " + stringify( value( ) ) );

private:

    bool m_set = false;

    T1 m_x = T1( );

protected:

    MinOrMax( ) { }
    MinOrMax( T1 cref in_x ) { update( in_x ); }
    MinOrMax( varray<T1> cref in_varray ) { update( in_varray ); }

    virtual T1 cref choose( T1 cref in_x1, T1 cref in_x2 ) const = 0;

public:

    operator T1( ) const { return( value( ) ); }

    T1 cref value( ) const { return( m_x ); }

    void update( T1 cref in_x ) { m_x = ( m_set ? choose( m_x, in_x ) : in_x ); m_set = true; }
    void update( varray<T1> cref in_varray ) { for_each( x, in_varray ) { update( x ); } }

    void reset( ) { m_x = T1( ); m_set = false; }
    void reset( T1 cref in_x ) { reset( ); update( in_x ); }
    void reset( varray<T1> cref in_varray ) { reset( ); update( in_varray ); }

    equal_default( MinOrMax );

};

classT Min : public MinOrMax<T1>{ strinfo( Min, "min: " + stringify( MinOrMax<T1>::value( ) ) ); public: Min( ) { } Min( T1 cref in_x ) : MinOrMax<T1>( in_x ) { } private: virtual T1 cref choose( T1 cref in_x1, T1 cref in_x2 ) const override { return( min( in_x1, in_x2 ) ); } equal_default( Min ); };
classT Max : public MinOrMax<T1>{ strinfo( Max, "max: " + stringify( MinOrMax<T1>::value( ) ) ); public: Max( ) { } Max( T1 cref in_x ) : MinOrMax<T1>( in_x ) { } private: virtual T1 cref choose( T1 cref in_x1, T1 cref in_x2 ) const override { return( max( in_x1, in_x2 ) ); } equal_default( Max ); };

classT MinMax : public Stringable
{

    strinfo( MinOrMax, "min: " + stringify( min( ) ) + " max: " + ::stringify( max( ) ) );

private:

    Min<T1> m_min;
    Max<T1> m_max;

public:

    MinMax( ) { }
    MinMax( T1 cref in_x ) { update( in_x ); }
    MinMax( varray<T1> cref in_varray ) { update( in_varray ); }

    operator span<T1>( ) const { return( span<T1>( min( ), max( ) ) ); }

    T1 cref min( ) const { return( m_min.value( ) ); }
    T1 cref max( ) const { return( m_max.value( ) ); }

    void update( T1 cref in_x ) { m_min.update( in_x ); m_max.update( in_x ); }
    void update( varray<T1> cref in_varray ) { for_each( x, in_varray ) { update( x ); } }

    void reset( ) { m_min.reset( ); m_max.reset( ); }
    void reset( T1 cref in_x ) { m_min.reset( in_x ); m_max.reset( in_x ); }
    void reset( varray<T1> cref in_varray ) { m_min.reset( in_varray ); m_max.reset( in_varray ); }

    equal_default( MinMax );

};

// -- utility classes -- //

class Countdown : public Stringable
{

    strinfo( MinOrMax, "countdown: " + stringify( remaining( ) ) + " / " + stringify( duration( ) ) + " span: " + ::stringify( duration_range( ) ) );

private:

    ticks m_countdown_remaining = 0.0;
    ticks m_countdown_duration = 0.0;
    span<ticks> m_countdown_duration_range = { 0.0, 0.0 };

public:

    Countdown( ) { }

    Countdown( ticks in_countdown ) { reset( in_countdown ); }
    Countdown( span<ticks> in_countdown_range ) { reset( in_countdown_range ); }

    dec percentage( ) const { return( is_zero( m_countdown_duration ) ? 0.0 : dec( m_countdown_remaining ) / dec( m_countdown_duration ) ); }

    ticks remaining( ) const { return( m_countdown_remaining ); }
    ticks duration( ) const { return( m_countdown_duration ); }
    span<ticks> duration_range( ) const { return( m_countdown_duration_range ); }

    void duration( ticks in_countdown ) { duration( { in_countdown, in_countdown } ); }
    void duration( span<ticks> in_countdown_range ) { m_countdown_duration_range = in_countdown_range; }

    void complete( ) { m_countdown_remaining = 0; }
    bool completed( ) const { return( is_zero( m_countdown_remaining ) ); }

    void reset( ); // defined in Random.hpp
    void reset( ticks in_countdown ) { duration( in_countdown ); reset( ); }
    void reset( span<ticks> in_countdown_range ) { duration( in_countdown_range ); reset( ); }

    void randomize( ); // start at a random point in the current duration, defined in Random.hpp

    // returns true is countdown is finished
    bool tick( ) { return( !is_pos( m_countdown_remaining -= min( 1.0, m_countdown_remaining ) ) ); }
    bool tick_auto_reset( ) { if( tick( ) ) { reset( ); return( true ); } else { return( false ); } }

    equal_default( Countdown );

};

// -- range utility functions -- //

typeT inline bool in_range( T1 cref in_x, T1 cref in_bound1, T1 cref in_bound2, bool in_include_bound1, bool in_include_bound2 )
{
    return( ( in_include_bound1 && ( in_x == in_bound1 ) ) ||
            ( in_include_bound2 && ( in_x == in_bound2 ) ) ||
            ( ( in_bound1 < in_bound2 ) ?
              ( ( in_x > in_bound1 ) && ( in_x < in_bound2 ) ) :
              ( ( in_x > in_bound2 ) && ( in_x < in_bound1 ) ) ) );
}

typeT inline bool in_range( T1 cref in_x, T1 cref in_bound, bool in_inclusive = true ) { return( in_range( in_x, T1( 0.0 ), in_bound, in_inclusive, in_inclusive ) ); }
typeT inline bool in_range( T1 cref in_x, T1 cref in_bound1, T1 cref in_bound2, bool in_inclusive = true ) { return( in_range( in_x, in_bound1, in_bound2, in_inclusive, in_inclusive ) ); }
typeT inline bool in_range( T1 cref in_x, span<T1> cref in_span, bool in_inclusive1, bool in_inclusive2 ) { return( in_range( in_x, in_span.min( ), in_span.max( ), in_inclusive1, in_inclusive2 ) ); }
typeT inline bool in_range( T1 cref in_x, span<T1> cref in_span, bool in_inclusive = true ) { return( in_range( in_x, in_span, in_inclusive, in_inclusive ) ); }

typeT inline bool in_range_0_to_1( T1 cref in_x, bool in_0_inclusive, bool in_1_inclusive ) { return( in_range( in_x, T1( 0.0 ), T1( 1.0 ), in_0_inclusive, in_1_inclusive ) ); }
typeT inline bool in_range_0_to_1( T1 cref in_x, bool in_inclusive = true ) { return( in_range_0_to_1( in_x, in_inclusive, in_inclusive ) ); }

// -- misc enums -- //

enum Direction
{
    NO_DIRECTION = 0,
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
};

} // namespace utility
} // namespace axn

#endif /* Utility_hpp */
