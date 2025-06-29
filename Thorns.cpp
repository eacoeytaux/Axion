#include "Thorns.hpp"
#include "World.hpp"
#include "Mob.hpp"

using mtmercy::Thorns;

namespace
{

cdec DAMAGE = ( 1.0 );

cPlanc VINE_THICKNESS = ( 6.0 );
cPlanc THORN_LENGTH = ( 3.0 );

const Span<Planc> THORN_SPACING = { THORN_LENGTH * 2.0, THORN_LENGTH * 5.0 };

cColor VINE_COLOR = ( GREEN );
cColor THORN_COLOR = ( WHITE );

} // namespace

Thorns::Thorns( Room * room, const Coordinate & _start, const Coordinate & _end ) : Hazard( room )
{
    #if defined ( AXN_DEBUG )
    draw_debug = true;
    #endif

    persist_render( true );
    needs_render_always( false );

    position( midpoint( _start, _end ) );
    space( FixedRectangle( _start - position( ), _end - position( ) ) );

    stationary( true );
    interactive( true );
    solid( true );

    Vector v = Vector( _start - position( ), _end - position( ) );
    Vector v_norm = v.normalized( );

    Vector offset = v;
    offset.magnitude( half( VINE_THICKNESS ) );
    offset.rotate( RIGHT );

    Planc length = v.magnitude( );

    auto draw_thorns = [ & ] ( uint passes = 1 )
    {
        for_range( i, passes )
        {
            Planc l = 0.0;
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

bool Thorns::collide( Object * object )
{
    Object::collide( object );

    if( object->interactive( ) && !dynamic_cast<Thorns *>( object ) )
    {
        if( Mob * mob = dynamic_cast<Mob *>( object ) )
        {
            mob->damage( Damage( DAMAGE ) );
            return true;
        }
    }

    return false;
}
