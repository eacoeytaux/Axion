#include "Player.hpp"

AXN_NAMESPACES
using axn::reality::Player;

bool is_god = false; // todo allow for individual players

bool Player::god( ) const { return is_god; }

Player & Player::god( const bool _god_mode, const uint64_t _password ) {
    if( _password != 666 )
        return *this;

    is_god = _god_mode;
    if( is_god ) {
        gravity_ratio( 0.f );                         // god feels no gravity
        terrarin_boundaries( false );                 // god is not affected by boundaries
        velocity( Vector( (planc)0.f, (planc)0.f ) ); // god brings all to a halt
    } else {
        // todo move defaults to Player as default settings
        // back to normal
        gravity_ratio( 1.f );
        terrarin_boundaries( true );
    }

    drawing_dirty( true );
    return *this;
}
