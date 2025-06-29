#include "Player.hpp"
#include "World.hpp"
#include "Arrow.hpp"

namespace
{

cuint PLAYER_HEALTH_START = 100;
}

Player::Player( cuint _player_number, Room * room, Coordinate cref _position ) : Mob( room, _position, PLAYER_HEALTH_START ), m_player_number( _player_number )
{
    god( true );

    terrain_bound( true );
}

void Player::render( )
{
    Mob::render( );

    if( god( ) )
    {
        cPlanc GLOW_RADIUS = height( );
        cPlanc GLOW_RADIUS_RATIO1 = 1.35;
        cPlanc GLOW_RADIUS_RATIO2 = 1.75;
        cPlanc GLOW_RADIUS_RATIO3 = 2.0;
        cColor GLOW_COLOR = Color( WHITE, 0.125 );

        Drawing god_drawing;
        god_drawing.draw( GLOW_COLOR, Polygon::circle( GLOW_RADIUS * GLOW_RADIUS_RATIO1 ) );
        god_drawing.draw( GLOW_COLOR, Polygon::circle( GLOW_RADIUS * GLOW_RADIUS_RATIO2 ) );
        god_drawing.draw( GLOW_COLOR, Polygon::circle( GLOW_RADIUS * GLOW_RADIUS_RATIO3 ) );
        draw( god_drawing );
    }
}

void Player::update( )
{
    Mob::update( );

    clear_light_sources( );
    add_light_source( position( ), light_sight( ) );

    if( god( ) )
    {
        add_light_source( position( ), 512 ); // the all seeing eye
    }
}

void Player::update_movement( )
{
    Mob::update_movement( );
}

void Player::die( )
{
    // static SoundClip death_cry( "WilhelmScream.wav" );
    // death_cry.play( );

    Mob::die( );
}

bool Player::damages( Damage cref _damage ) const
{
    return ( !god( ) && Mob::damages( _damage ) );
}

void Player::out_of_bounds( )
{
    #ifndef AXN_DEBUG
    Object::out_of_bounds( );
    #endif
}

void Player::god( cbool _god )
{
    return_if( _god == m_god );

    m_god = _god;

    if( m_god )
    {
        if( dead( ) )
        {
            revive( );
        }

        invincible_always( true );   // god is invincible
        heal_full( );                // god is never wounded
        no_gravity( );               // god is not pulled on by gravity
        terrain_bound( false );      // god is not affected by boundaries
        stationary( false );         // god moves at its own whim
        velocity( V0 );              // god brings all to a halt
    }
    else
    {
        invincible_always( false );
        normal_gravity( );
        terrain_bound( true );
    }

    needs_render( true );
}
