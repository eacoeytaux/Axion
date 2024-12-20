#include "Player.hpp"
#include "World.hpp"
#include "Arrow.hpp"

namespace
{
const uint PLAYER_HEALTH_START = 100;
}

Player::Player( World * world, const Coordinate & _position ) : Mob( world, _position, PLAYER_HEALTH_START )
{
    god( false );

    terrain_boundaries( true );
}

void Player::render( )
{
    Mob::render( );

    if( god( ) )
    {
        const Planc GLOW_RADIUS = height( );
        const Planc GLOW_RADIUS_RATIO1 = 1.35;
        const Planc GLOW_RADIUS_RATIO2 = 1.75;
        const Planc GLOW_RADIUS_RATIO3 = 2.0;
        const Color GLOW_COLOR = Color( WHITE, 0.125 );

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
    if( god( ) )
    {
        return;
    }

    // if( alive( ) )
    // {
    //     static SoundClip death_cry( "WilhelmScream.wav" );
    //     death_cry.play( );
    // }

    Mob::die( );
}

void Player::hurt( dec _health )
{
    if( god( ) )
    {
        return;
    }

    Mob::hurt( _health );
}

void Player::out_of_bounds( )
{
    #ifndef AXN_DEBUG
    Object::out_of_bounds( );
    #endif
}

void Player::god( const bool _god )
{
    if( _god == m_god )
    {
        return;
    }

    m_god = _god;

    if( m_god )
    {
        invincible_always( true );   // god is invincible
        heal_full( );                // god is never wounded
        no_gravity( );               // god is not pulled on by gravity
        terrain_boundaries( false ); // god is not affected by boundaries
        velocity( V0 );              // god brings all to a halt
    }
    else
    {
        invincible_always( false );
        normal_gravity( );
        terrain_boundaries( true );
    }

    needs_render( true );
}
