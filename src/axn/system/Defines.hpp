#ifndef Defines_hpp
#define Defines_hpp

#define macro_concat( str1, str2 ) str1##str2
#define macro_concat_step( str1, str2 ) macro_concat( str1, str2 )
#define macro_concat_line( str1 ) macro_concat_step( str1, __LINE__ )
#define macro_unique( str1 ) macro_concat_line( macro_concat_step( str1, _ ) )

#define ref &
#define cref const &

#define if if
#define else else

#define for for

#define switch switch
#define a_case case
#define b_case break; case
#define d_case default: case
#define no_default default: break
#define default default

#define break break
#define break_if( b ) if( b ) break

#define continue continue
#define continue_if( b ) if( b ) continue

#define return return

#define return_if( b, x ) if( b ) return x

#define return_true_if( b ) return_if( b, true )
#define return_false_if( b ) return_if( b, false )

#define rethis return( *this )
#define rethis_if( b ) return_if( b, *this )

#define try try
#define catch catch
#define throw throw

#define using using

#define do do
#define while while
#define do_while do
#define do_until_break( ) while( true )
#define do_count( i ) uint macro_unique( macro_do_count ) = i; for( uint macro_do_counter = 0; macro_do_counter < macro_unique( macro_do_count ); macro_do_counter++ )

#define while_max( b, max ) uint macro_unique( macro_while_max_count ) = 0; uint macro_unique( macro_while_max ) = max; while( b && ( macro_unique( macro_while_max_count )++ < macro_unique( macro_while_max ) ) )

#define for_range( i, range ) uint macro_unique( macro_range_max ) = range; for( uint i = 0; i < macro_unique( macro_range_max ); i++ )

#define for_each( x, container ) for( auto ref x : container )
#define for_each_const( x, container ) for( auto cref x : container )

#define default_equal( ... ) bool operator==( __VA_ARGS__ cref in_other ) const = default; bool operator!=( __VA_ARGS__ cref in_other ) const = default

#define static_once static bool macro_unique( b_set ) = true; if( macro_unique( b_set ) && !( macro_unique( b_set ) = false ) )
#define static_setup( ... ) static __VA_ARGS__; static_once

#define section( x ) namespace

#define var_static( ... ) namespace { static __VA_ARGS__; }
#define var_const( ... ) namespace { const __VA_ARGS__; }

#define lambdaf( func ) auto func = [ & ]

#define enum enum
#define class class
#define struct struct
#define namespace namespace

#define template template
#define templated template<>

#define typeT template <typename T1>
#define classT template <typename T1> class
#define structT template <typename T1> struct

#define typeT2 template <typename T1, typename T2>
#define classT2 template <typename T1, typename T2> class
#define structT2 template <typename T1, typename T2> struct

#define use_std( x ) using std::x

#define use_std_func( x ) typeT dec x( T1 cref in_t ) { return( std::x( dec( in_t ) ) ); }
#define use_std_func2( x ) typeT2 dec x( T1 cref in_t1, T2 cref in_t2 ) { return( std::x( dec( in_t1 ), dec( in_t2 ) ) ); }

#pragma warning( disable : 4003 )

#endif /* Defines_hpp */
