#include "Random.hpp"
#include "Logger.hpp"
#include "Clock.hpp"
#include "Utility.hpp"
#include "Angle.hpp"
#include "Color.hpp"

namespace
{
cuint RANDOM_BUFFER_SIZE = 65536;
uint RANDOM_BUFFER[ RANDOM_BUFFER_SIZE ];
uint RANDOM_BUFFER_INDEX = 0;
} // namespace

const Random::Distribution Random::DEFAULT_DISTRIBUTION = Random::UNIFORM;

uint random_int( )
{
    RANDOM_BUFFER_INDEX %= RANDOM_BUFFER_SIZE;
    return RANDOM_BUFFER[ RANDOM_BUFFER_INDEX++ ];
}

uint random_int_max( )
{
    return RAND_MAX;
}

error Random::seed( const string _seed )
{
    uint seed_uint = 0;
    for_range( i, _seed.length( ) ) { seed_uint ^= ( _seed[ i ] << ( 8 * ( i % sizeof( uint ) ) ) ); }
    return seed( seed_uint );
}

error Random::seed( )
{
    return seed( milliseconds( ) );
}

error Random::seed( cuint _seed )
{
    ::srand( _seed );
    Log( INFO_LOG, "random seed ............ ( %u )", _seed );
    for_range( i, RANDOM_BUFFER_SIZE ) { RANDOM_BUFFER[ i ] = ::rand( ); }
    return no_error;
}

bool Random::rbool( cdec _true_odds )
{
    Assert( _true_odds >= 0.0 );

    if( _true_odds >= 1.0 )
    {
        return true;
    }
    else if( _true_odds == 0.0 )
    {
        return false;
    }

    return ( rdec( 1.0 ) <= _true_odds );
}

int Random::rint( cint _l, cint _h, const Distribution _distribution )
{
    if( _l == _h )
    {
        return _l;
    }

    int l = _l;
    int h = _h;

    if( l > h )
    {
        swap_values<int>( h, l );
    }

    return ( random_int( ) % ( h - l + 1 ) ) + l;
}

dec Random::rdec( cdec _l, cdec _h, const Distribution _distribution )
{
    if( _l == _h )
    {
        return _l;
    }

    dec l = _l;
    dec h = _h;

    if( l > h )
    {
        swap_values( h, l );
    }

    dec rand_dec = (dec)random_int( );
    dec rand_max = (dec)random_int_max( );
    dec rand_ratio = rand_dec / rand_max;
    return ( rand_ratio * ( h - l ) ) + l;
}

Planc Random::rPlanc( Planc cref _l, Planc cref _h, const Distribution _distribution )
{
    return Planc( rdec( _l, _h, _distribution ) );
}

Angle Random::rAngle( Angle cref _low_bound, Angle cref _high_bound, const Distribution _distribution )
{
    return Angle( rdec( _low_bound.radians( ), _high_bound.radians( ), _distribution ) );
}

Color Random::rColor( cbool include_alpha )
{
    return Color::rgba( rdec( 1.0 ), rdec( 1.0 ), rdec( 1.0 ), include_alpha ? rdec( 1.0 ) : 1.0 );
}
