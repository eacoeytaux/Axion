#include "Player.hpp"

#include "World.hpp"
#include "IcePick.hpp"
#include "Arrow.hpp"
#include "Hook.hpp"
#include "Fire.hpp"

AXN_NAMESPACES

Player::Player( World* world, const Coordinate & _position ) : Mob( world, _position, 100 ) {}

const Player & Player::render( ) const {
    Mob::render( );

    if( god( ) ) {
        static_setup( Drawing, god_drawing ) {
            const Color glow_color = Color( WHITE ).a( 0.125 );
            const planc glow_radius = height( );
            god_drawing.add( glow_color, Shape::circle( glow_radius * 2.f ) );
            god_drawing.add( glow_color, Shape::circle( glow_radius * 1.7f ) );
            god_drawing.add( glow_color, Shape::circle( glow_radius * 1.3f ) );
        }

        draw( god_drawing );
    }

    return *this;
}

Player & Player::update( ) {
    Mob::update( );

    // for_each (object, world()->objects_in_range(hit_box())) {
    //     if (dynamic_pointer_cast<Fire>(object)) hurt(1);
    // }

    clear_light_sources( );
    add_light_source( position( ), 100.f );

    if( god( ) )
        add_light_source( position( ), pow<planc>( 2.f, 16 ) ); // the all seeing eye

    return *this;
}

Player & Player::move( ) {
    Mob::move( );
    return *this;
}

Player & Player::die( ) {
#ifdef AXN_DEBUG
    if( god( ) )
        return *this;
#endif
    // static SoundClip death_cry("WilhelmScream.wav");
    // if (alive()) death_cry.play();
    Mob::die( );
    return *this;
}

Player & Player::hurt( uint _health ) {
#ifdef AXN_DEBUG
    if( god( ) )
        return *this;
#endif
    Mob::hurt( _health );
    return *this;
}
