#include "axn/world/Projectile.hpp"

#include "axn/world/World.hpp"
#include "axn/world/Mob.hpp"
#include "axn/world/Player.hpp"

var_const( dec DEFAULT_GRAVITY_SCALE = ( 0.5 ) );
var_const( dec DEFAULT_AIR_RESISTANCE = ( 0.001 ) );

Projectile::Projectile( ptr<Room> in_room, Damage cref in_damage, Point cref in_position, Vector cref in_velocity ) : Object( in_room, in_position, in_velocity )
{
    #if defined( AXN_DEBUG )
    draw_debug = true;
    #endif

    interactive( true );

    solid( true );

    gravity_scale( DEFAULT_GRAVITY_SCALE );
    air_resistance( DEFAULT_AIR_RESISTANCE );

    m_damage = in_damage;

    // track_position( 1 );
}

void Projectile::render( ) { Object::render( ); }
void Projectile::update( ) { Object::update( ); }

void Projectile::activate( bool in_active )
{
    m_active = in_active;

    if( m_active )
    {
        solid( true );
        interactive( true );
    }
    else
    {
        solid( false );
        interactive( false );
    }
}

void Projectile::react_to_movement( ptr<Object> in_object, Vector cref in_movement )
{
    Object::react_to_movement( in_object, in_movement );

    move( in_movement ); // TODO this should be standard in Object
}

bool Projectile::collide( ptr<Object> in_object )
{
    Object::collide( in_object );

    if( in_object->solid( ) )
    {
        if( cast_as( in_object, mob, Mob ) )
        {
            if( !m_player_only || cast( mob, Player ) )
            {
                mob->damage( damage( ) );
            }
        }

        in_object->add_force( force( ) );

        subscribe_to_movement( in_object );
        stationary( true );

        activate( false );

        return( true );
    }

    return( false );
}

void Projectile::ground( ptr<Terrain::Node> in_ground, Terrain::Bumper cref in_bumper )
{
    Object::ground( in_ground, in_bumper );

    if( in_ground )
    {
        stationary( true );

        activate( false );
    }
}
