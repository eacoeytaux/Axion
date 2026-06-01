#include "mt/hazards/Thorns.hpp"

#include "axn/world/Mob.hpp"

var_const( dec DAMAGE = ( 1.0 ) );

var_const( Planc VINE_THICKNESS = ( 6.0 ) );
var_const( Planc THORN_LENGTH = ( 3.0 ) );

var_const( Span<Planc> THORN_SPACING = { THORN_LENGTH * 2.0, THORN_LENGTH * 5.0 } );

var_const( Color VINE_COLOR = ( GREEN ) );
var_const( Color THORN_COLOR = ( WHITE ) );

Thorns::Thorns( ptr<Room> in_room, Point cref in_start, Point cref in_end ) : Hazard( in_room )
{
    #if defined( AXN_DEBUG )
    draw_debug = true;
    #endif

    persist_render( true );
    needs_render_always( false );

    position( midpoint( in_start, in_end ) );
    space( FixedRectangle( in_start - position( ), in_end - position( ) ) );

    stationary( true );
    interactive( true );
    solid( true );

    Vector v = Vector( in_start - position( ), in_end - position( ) );
    Vector v_norm = v.normalized( );

    Vector offset = v;
    offset.magnitude( half( VINE_THICKNESS ) );
    offset.rotate_right( true );

    Planc length = v.magnitude( );

    lambdaf( draw_thorns )( uint in_passes = 1 )
    {
        for_range( i, in_passes )
        {
            Planc l = P0;

            while( l < length )
            {
                draw( THORN_COLOR, Polygon::equilateral( 4, THORN_LENGTH, ( v_norm * l ) + ( Random::negated( offset * Random::rPlanc( 1.0 ) ) ) ) );

                l += Random::rPlanc( THORN_SPACING );
            }
        }
    };

    draw_thorns( 1 );
    draw( VINE_COLOR, Line( v ), VINE_THICKNESS );
    draw_thorns( 2 );
}

bool Thorns::collide( ptr<Object> in_object )
{
    Object::collide( in_object );

    if( in_object->interactive( ) && !cast( Thorns, in_object ) )
    {
        if( cast_as( mob, Mob, in_object ) )
        {
            mob->damage( Damage( DAMAGE ) );
            return( true );
        }
    }

    return( false );
}
