#include "Spikes.hpp"

#include "Mob.hpp"

using mtmercy::Spike;

Spike::Spike( Room * room, Coordinate cref _base ) : Hazard( room, _base )
{
    #if defined ( AXN_DEBUG )
    draw_debug = true;
    #endif

    persist_render( true );
    needs_render_always( false );

    stationary( true );
    interactive( true );
    solid( true );

    Vector spike( 0.0, 100.0 );

    position( _base + spike );

    draw( WHITE, Polygon( { ORIGIN, -spike + VectorA( PI, 15.0 ), -spike + VectorA( A0, 15.0 ) } ) );
}

bool Spike::collide( Object * object )
{
    bool reaction = Object::collide( object );

    if( Mob * mob = dynamic_cast<Mob *>( object ) )
    {
        // TODO if( is_pos( mob->gravity( ) ) && is_neg( mob->velocity( ).dy( ) ) )
        {
            //mob->hurt_full( );
            //mob->stationary( true );
            return true;
        }
    }

    return reaction;
}
