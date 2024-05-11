#include "Player.hpp"

#include "World.hpp"

#include "Arrow.hpp"

using axn::reality::Player;

Player::Player( World * world, const Coordinate & _position ) : Mob( world, _position, 100 ) { god( god( ) ); }

const Player & Player::render( ) const
{
    Mob::render( );

    if( god( ) )
    {
        static_setup( Drawing, god_drawing )
        {
            const Color GLOW_COLOR = Color( WHITE, 0.125 );
            const Planc GLOW_RADIUS = height( );

            god_drawing.draw( GLOW_COLOR, Shape::circle( GLOW_RADIUS * 2.0 ) );
            god_drawing.draw( GLOW_COLOR, Shape::circle( GLOW_RADIUS * 1.7 ) );
            god_drawing.draw( GLOW_COLOR, Shape::circle( GLOW_RADIUS * 1.3 ) );
        }

        draw( god_drawing );
    }

    return *this;
}

Player & Player::update( )
{
    Mob::update( );

    clear_light_sources( );
    add_light_source( position( ), light_sight( ) );

    if( god( ) )
    {
        add_light_source( position( ), 512.0 ); // the all seeing eye
    }

    return *this;
}

Player & Player::move( )
{
    Mob::move( );
    return *this;
}

Player & Player::die( )
{
#ifdef AXN_DEBUG
    if( god( ) )
        return *this;
#endif
    // if( alive( ) )
    //{
    //     static SoundClip death_cry( "WilhelmScream.wav" );
    //     death_cry.play( );
    // }

    Mob::die( );
    return *this;
}

Player & Player::hurt( double _health )
{
#ifdef AXN_DEBUG
    if( god( ) )
        return *this;
#endif
    Mob::hurt( _health );
    return *this;
}
