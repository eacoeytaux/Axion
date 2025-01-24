#include "Projectile.hpp"
#include "World.hpp"
#include "Mob.hpp"

using mtmercy::Projectile;

Projectile::Projectile( Room * room, Coordinate cref _position ) : Object( room, _position )
{
    #ifdef AXN_DEBUG
    m_draw_debug = true;
    #endif

    interactive( true );

    solid( true );

    gravity_ratio( 0.5 );
    air_resistance_ratio( 0.001 );

    track_position( 1 );
}

void Projectile::render( ) { Object::render( ); }
void Projectile::update( ) { Object::update( ); }

void Projectile::ground( TerrainEdge * ground )
{
    Object::ground( ground );

    if( ground )
    {
        deactivate( );
    }
}

bool Projectile::collide( Object * object )
{
    Object::collide( object );

    if( object->interactive( ) )
    {
        if( Mob * mob = dynamic_cast<Mob *>( object ) )
        {
            mob->hurt( damage( ) );
        }

        subscribe_to_movement( object );

        deactivate( );

        return true;
    }

    return false;
}

void Projectile::react_to_movement( Object * object, Vector cref _v )
{
    Object::react_to_movement( object, _v );

    position( position( ) + _v );
}

void Projectile::deactivate( )
{
    m_active = false;
    solid( false );
    stationary( true );
    interactive( false );
}
