#include "Random.hpp"
#include "Logger.hpp"

namespace
{

cuint RANDOM_BUFFER_SIZE = 65536;
uint RANDOM_BUFFER[ RANDOM_BUFFER_SIZE ];
uint RANDOM_BUFFER_INDEX = 0;

} // namespace

uint Random::seed_value = 0;
rng_engine Random::engine = rng_engine { };

uint Random::random_int( ) { return RANDOM_BUFFER[ RANDOM_BUFFER_INDEX = ( ( RANDOM_BUFFER_INDEX + 1 ) % RANDOM_BUFFER_SIZE ) ]; }

error Random::set( cuint seed )
{
    seed_value = seed;
    
    ::srand( seed );
    engine.seed( seed );
    
    for_range( i, RANDOM_BUFFER_SIZE ) { RANDOM_BUFFER[ i ] = ::rand( ); }
    RANDOM_BUFFER_INDEX = 0;
    
    Log( INFO_LOG, "random seed ............ ( %u )", seed_value );
    return no_error;
}
