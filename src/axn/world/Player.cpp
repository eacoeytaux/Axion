#include "axn/world/Player.hpp"

#include "axn/world/World.hpp"
#include "axn/world/Door.hpp"

var_const( uint PLAYER_HEALTH_START = ( 100 ) );

var_const( Color PLAYER_COLOR_DEFAULT = ( BLUE ) );

var_const( Planc LIGHT_RADIUS = ( METER * 8.0 ) );
var_const( Planc GLOW_RADIUS = ( METER * 3.0 ) );
var_const( Planc GLOW_RADIUS_RATIO_1 = ( 1.35 ) );
var_const( Planc GLOW_RADIUS_RATIO_2 = ( 1.75 ) );
var_const( Planc GLOW_RADIUS_RATIO_3 = ( 2.0 ) );
var_const( Color GLOW_COLOR = ( Color( WHITE, 0.125 ) ) );

Player::Player( uint in_player_number, ptr<Room> in_room, Point cref in_position ) : Mob( in_room, in_position, PLAYER_HEALTH_START ), m_player_number( in_player_number )
{
    terrain_bound( true );

    player_color( PLAYER_COLOR_DEFAULT );
}

void Player::render( )
{
    Mob::render( );

    if( god( ) )
    {
        Drawing god_drawing;

        god_drawing.draw( GLOW_COLOR, Polygon::circle( GLOW_RADIUS * GLOW_RADIUS_RATIO_1 ) );
        god_drawing.draw( GLOW_COLOR, Polygon::circle( GLOW_RADIUS * GLOW_RADIUS_RATIO_2 ) );
        god_drawing.draw( GLOW_COLOR, Polygon::circle( GLOW_RADIUS * GLOW_RADIUS_RATIO_3 ) );

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
        add_light_source( position( ), LIGHT_RADIUS ); // the all seeing eye
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

bool Player::damages( Damage cref in_damage ) const
{
    return( !god( ) && Mob::damages( in_damage ) );
}

bool Player::collide( ptr<Object> in_object )
{
    if( opening_door( ) )
    {
        if( cast_as( door, Door, in_object ) )
        {
            if( !door->locked( ) )
            {
                velocity( V0 );
                clear_input( );

                door->open( );

                return( true );
            }
        }
    }

    return( false );
}

void Player::out_of_bounds( )
{
    #if !defined( AXN_DEBUG )
    // Object::out_of_bounds( );
    #endif
}

void Player::god( bool in_god )
{
    return_if( m_god == in_god );

    m_god = in_god;

    if( m_god )
    {
        if( dead( ) ) { revive( ); }

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
