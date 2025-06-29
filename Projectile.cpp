#include "Projectile.hpp"
#include "World.hpp"
#include "Mob.hpp"

using axn::Projectile;

Projectile::Projectile( Room * room, Coordinate cref _position ) : Object( room, _position )
{
    #if defined ( AXN_DEBUG )
    draw_debug = true;
    #endif

    interactive( true );

    solid( true );

    // todo
    gravity_scale( 0.5 );
    air_resistance( 0.001 );

    // track_position( 1 );
}

void Projectile::render( ) { Object::render( ); }
void Projectile::update( ) { Object::update( ); }

void Projectile::ground( Terrain::Node * ground, Terrain::Bumper cref _bumper )
{
    Object::ground( ground, _bumper );

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
            mob->damage( damage( ) );
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

    move( _v );
}

void Projectile::deactivate( )
{
    m_active = false;
    solid( false );
    stationary( true );
    interactive( false );
}
