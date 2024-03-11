#include "Random.hpp"

#include "Logger.hpp"
#include "Clock.hpp"
#include "Angle.hpp"
#include "Color.hpp"

AXN_NAMESPACES

err Random::seed( ) {
    Clock clock;
    return seed( (uint)( clock.total_ms( ) * clock.total_ms( ) * clock.total_ms( ) ) );
}

err Random::seed( const uint _seed ) {
    ::srand( _seed );
    Logger::log( INFO_LOG, "random seed ............ ( %u )", _seed );
    return no_error;
}

err Random::seed( const string _seed ) {
    uint seed_uint = 0;
    int shift_count = sizeof( uint );

    for_range( _seed.length( ) ) {
        char c = _seed[ i ];
        seed_uint ^= ( c << ( 8 * ( i % shift_count ) ) );
    }

    return seed( seed_uint );
}

bool Random::rng_bool( const ufloat _true_odds ) {
    if( _true_odds >= 1.f )
        return true;
    return ( rng_float( 1.f ) <= _true_odds );
}

int Random::rng_int( const int _h, const int _l ) {
    if( _l == _h )
        return _l;
    int h = _h;
    int l = _l;
    if( l > h )
        swap_values<int>( h, l );
    return ( rand( ) % ( h - l + 1 ) ) + l;
}

float Random::rng_float( const float _h, const float _l ) {
    if( _l == _h )
        return _l;
    float h = _h;
    float l = _l;
    if( l > h )
        swap_values<float>( h, l );
    float rand_float = static_cast<float>( rand( ) );
    float rand_max = static_cast<float>( RAND_MAX );
    float rand_ratio = rand_float / rand_max;
    return ( rand_ratio * ( h - l ) ) + l;
}

Angle Random::r_Angle( ) { return Angle( rng_float( PI_2 ) ); }

Color Random::rng_Color( const bool include_alpha ) {
    return Color::rgb( Random::rng_float( 1.f ), Random::rng_float( 1.f ),
                       Random::rng_float( 1.f ),
                       include_alpha ? Random::rng_float( 1.f ) : 1.f );
}
