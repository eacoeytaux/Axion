#include "Player.hpp"

using axn::reality::Player;

namespace
{
const uint PASSWORD = 666;
IDumap<bool> gods;
} // namespace

bool Player::god( ) const
{
    if( !gods.contains( *this ) )
        gods[ *this ] = false;
    return gods[ *this ];
}

Player & Player::god( const bool _god_mode, const uint64_t _password )
{
    if( !gods.contains( *this ) )
        gods[ *this ] = false;

    bool is_god = gods[ *this ];

    if( _god_mode && ( ( _password == PASSWORD ) || is_god ) )
    {
        is_god = _god_mode;
        gravity_ratio( 0.0 );        // god feels no gravity
        terrain_boundaries( false ); // god is not affected by boundaries
        velocity( ZERO_VECTOR );     // god brings all to a halt
    }
    else
    {
        // todo move defaults to Player as default settings
        // back to normal
        is_god = _god_mode;
        gravity_ratio( 1.0 );
        terrain_boundaries( true );
    }

    gods[ *this ] = is_god;

    drawing_dirty( true );
    return *this;
}
