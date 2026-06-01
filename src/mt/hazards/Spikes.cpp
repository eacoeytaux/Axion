#include "mt/hazards/Spikes.hpp"

#include "axn/world/Mob.hpp"

var_const( Span<Planc> SPIKE_HEIGHT = { 75.0, 125.0 } );
var_const( Span<dec> SPIKE_BASE_WIDTH_RATIO = { 4.0, 5.0 } );

var_const( Angle SPIKE_MAX_ANGLE_OFFSET = ( Angle::fraction( 24.0 ) ) );

Spike::Spike( ptr<Room> room, Point cref in_base ) : Hazard( room, in_base )
{
    #if defined( AXN_DEBUG )
    draw_debug = true;
    #endif

    persist_render( true );
    needs_render_always( false );

    stationary( true );
    interactive( true );
    solid( true );

    Angle normal = ( RIGHT );

    Planc height = Random::rPlanc( SPIKE_HEIGHT );
    Planc base_width = height / Random::rdec( SPIKE_BASE_WIDTH_RATIO );

    Vector spike = VectorA( normal + Random::rAngle( -SPIKE_MAX_ANGLE_OFFSET, SPIKE_MAX_ANGLE_OFFSET ), height );

    position( in_base + spike );

    draw( WHITE, Polygon( { ORIGIN, -spike + VectorA( PI, half( base_width ) ), -spike + VectorA( A0, half( base_width ) ) } ) );
}

bool Spike::collide( ptr<Object> in_object )
{
    bool reaction = Object::collide( in_object );

    if( cast_as( mob, Mob, in_object ) )
    {
        // TODO if( is_pos( mob->gravity( ) ) && is_neg( mob->velocity( ).dy( ) ) )
        {
            mob->hurt_full( );
            return( true );
        }
    }

    return( reaction );
}
