#ifndef Defines_hpp
#define Defines_hpp

#define macro_concat( s1, s2 ) s1##s2
#define macro_concat_step( s1, s2 ) macro_concat( s1, s2 )
#define macro_concat_line( s1 ) macro_concat_step( s1, __LINE__ )
#define macro_unique( s1 ) macro_concat_line( macro_concat_step( s1, _ ) )

#define macro_end static_assert( true, "macro requires semicolon" )

#define ref &
#define cref const &

#define nostring ( "" )

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

#define delete delete

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

#define equal_default( ... ) bool operator==( __VA_ARGS__ cref ) const = default; bool operator!=( __VA_ARGS__ cref ) const = default; macro_end 
#define equal_always( ... ) bool operator==( __VA_ARGS__ cref ) const { return( true ); } bool operator!=( __VA_ARGS__ cref ) const { return( false ); } macro_end
#define equal_never( ... ) bool operator==( __VA_ARGS__ cref ) const { return( false ); } bool operator!=( __VA_ARGS__ cref ) const { return( true ); } macro_end

#define static_once static bool macro_unique( b_set ) = true; if( macro_unique( b_set ) && !( macro_unique( b_set ) = false ) )
#define static_setup( ... ) static __VA_ARGS__; static_once

#define section( x ) namespace

#define var_static( ... ) namespace { static __VA_ARGS__; } macro_end
#define var_const( ... ) namespace { const __VA_ARGS__; } macro_end

#define lambdaf( func ) auto func = [ & ]

#define enum enum
#define class class
#define struct struct
#define namespace namespace

#define typedef typedef

#define template( ... ) template<__VA_ARGS__>
#define templateN( ... ) template<__VA_ARGS__, uint N>

#define typeT template( typename T1 )
#define classT template( typename T1 ) class
#define structT template( typename T1 ) struct

#define typeTN templateN( typename T1 )
#define classTN templateN( typename T1 ) class
#define structTN templateN( typename T1 ) struct

#define typeT2 template( typename T1, typename T2 )
#define classT2 template( typename T1, typename T2 ) class
#define structT2 template( typename T1, typename T2 ) struct

#define typeT2N templateN( typename T1, typename T2 )
#define classT2N templateN( typename T1, typename T2 ) class
#define structT2N templateN( typename T1, typename T2 ) struct

#define use_std( x ) using std::x

#define use_std_func( x ) typeT inline dec x( T1 cref in_t ) { return( std::x( dec( in_t ) ) ); }
#define use_std_func2( x ) typeT2 inline dec x( T1 cref in_t1, T2 cref in_t2 ) { return( std::x( dec( in_t1 ), dec( in_t2 ) ) ); }

#define use_std_to_string( type ) inline std::string to_string( type in_t ) { return( std::to_string( in_t ) ); }

#pragma warning( disable : 4003 )

#endif /* Defines_hpp */
