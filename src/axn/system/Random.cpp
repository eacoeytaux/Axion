#include "axn/system/Random.hpp"

#include "axn/system/Logger.hpp"

var_const( uint BUFFER_SIZE = ( 65536 ) );

var_static( carray<uint, BUFFER_SIZE> buffer );
var_static( uint buffer_index = ( 0 ) );
var_static( bool buffer_filled = ( false ) );

default_random_engine Random::engine = default_random_engine{ };

uint Random::seed_value = 0;

uint Random::next( ) { return( buffer[ buffer_index = ( ( buffer_index + 1 ) % buffer.size( ) ) ] ); }

Error Random::seed( uint in_seed )
{
    try
    {
        if( ( seed_value != in_seed ) || !buffer_filled )
        {
            seed_value = in_seed;

            ::srand( in_seed );
            engine.seed( in_seed );

            for_range( i, buffer.size( ) ) { buffer[ i ] = ::rand( ); }
            buffer_filled = true;
        }

        buffer_index = 0;
    }
    catch_error_return( );

    Log( INFO_LOG, "random seed ............ ( %u )", seed_value );
    return( no_error );
}
