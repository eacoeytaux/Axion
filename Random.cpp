#include "Random.hpp"

#include "Logger.hpp"
#include "Clock.hpp"
#include "Utility.hpp"

#include "Angle.hpp"
#include "Color.hpp"

namespace
{
const uint RANDOM_BUFFER_SIZE = 65536;
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

error Random::rseed( )
{
    return rseed( (uint)( Clock( ).total_ms( ) ) );
}

error Random::rseed( const string _seed )
{
    uint seed_uint = 0;
    for_range( _seed.length( ) ) { seed_uint ^= ( _seed[ i ] << ( 8 * ( i % sizeof( uint ) ) ) ); }
    return rseed( seed_uint );
}

error Random::rseed( const uint _seed )
{
    ::srand( _seed );
    Log( INFO_LOG, "random seed ............ ( %u )", _seed );
    for_range( RANDOM_BUFFER_SIZE ) { RANDOM_BUFFER[ i ] = ::rand( ); }
    return no_error;
}

bool Random::rbool( const double _true_odds )
{
    Assert( _true_odds >= 0.0 );

    if( _true_odds >= 1.0 )
        return true;
    else if( _true_odds == 0.0 )
        return false;
    return ( rdouble( 1.0 ) <= _true_odds );
}

int Random::rint( const int _l, const int _h, const Distribution _distribution )
{
    if( _l == _h )
        return _l;
    int h = _h, l = _l;
    if( l > h )
        swap_values<int>( h, l );
    return ( random_int( ) % ( h - l + 1 ) ) + l;
}

double Random::rdouble( const double _l, const double _h, const Distribution _distribution )
{
    if( _l == _h )
        return _l;
    double h = _h, l = _l;
    if( l > h )
        swap_values( h, l );
    double rand_double = (double)random_int( );
    double rand_max = (double)random_int_max( );
    double rand_ratio = rand_double / rand_max;
    return ( rand_ratio * ( h - l ) ) + l;
}

Planc Random::rPlanc( const Planc & _l, const Planc & _h, const Distribution _distribution )
{
    return Planc( rdouble( _l, _h, _distribution ) );
}

Angle Random::rAngle( const Angle & _low_bound, const Angle & _high_bound, const Distribution _distribution )
{
    return Angle( rdouble( _low_bound.radians( ), _high_bound.radians( ), _distribution ) );
}

Color Random::rColor( const bool include_alpha )
{
    return Color::rgba( rdouble( 1.0 ), rdouble( 1.0 ), rdouble( 1.0 ), include_alpha ? rdouble( 1.0 ) : 1.0 );
}
