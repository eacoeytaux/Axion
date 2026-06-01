#include "axn/system/Random.hpp"

#include "axn/system/Logger.hpp"

var_const( uint RANDOM_BUFFER_SIZE = ( 65536 ) );

var_static( bool RANDOM_BUFFER_FILLED = ( false ) );
var_static( uint RANDOM_BUFFER[ RANDOM_BUFFER_SIZE ] );
var_static( uint RANDOM_BUFFER_INDEX = ( 0 ) );

uint Random::seed_value = 0;
default_random_engine Random::engine = default_random_engine{ };

uint Random::random_int( ) { return( RANDOM_BUFFER[ RANDOM_BUFFER_INDEX = ( ( RANDOM_BUFFER_INDEX + 1 ) % RANDOM_BUFFER_SIZE ) ] ); }

error Random::seed( uint in_seed )
{
    if( ( seed_value != in_seed ) || !RANDOM_BUFFER_FILLED )
    {
        seed_value = in_seed;

        ::srand( in_seed );
        engine.seed( in_seed );

        for_range( i, RANDOM_BUFFER_SIZE ) { RANDOM_BUFFER[ i ] = ::rand( ); }
        RANDOM_BUFFER_FILLED = true;
    }

    RANDOM_BUFFER_INDEX = 0;

    Log( INFO_LOG, "random seed ............ ( %u )", seed_value );
    return( no_error );
}
