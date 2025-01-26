#include "Thorns.hpp"
#include "World.hpp"
#include "Mob.hpp"

using mtmercy::Thorns;

namespace
{
const Planc DAMAGE = 1.0;
const Planc VINE_THICKNESS = 6.0;
const Planc THORN_LENGTH = 3.0;
const Span<Planc> THORN_SPACING = { THORN_LENGTH * 2.0, THORN_LENGTH * 5.0 };
const Color VINE_COLOR = GREEN;
const Color THORN_COLOR = WHITE;
} // namespace

Thorns::Thorns( Room * room, const Coordinate & _start, const Coordinate & _end ) : Object( room )
{
#ifdef AXN_DEBUG
    m_draw_debug = true;
#endif
    
    persist_render( true );
    needs_render_always( false );
    
    position( midpoint( _start, _end ) );
    space( FixedRectangle( _start - position( ), _end - position( ) ) );
    
    stationary( true );
    interactive( true );
    solid( true );
    
    Vector v = Vector( _start - position( ), _end - position( ) );
    
    Vector v_norm = v;
    v_norm.normalize( );
    
    Vector offset = v;
    offset.magnitude( half( VINE_THICKNESS ) );
    offset.rotate( RIGHT_ANGLE );
    
    Planc length = v.magnitude( );
    
    auto draw_thorns = [ & ]( uint passes = 1 )
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
    
    draw_thorns( );
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
            mob->hurt( Damage( DAMAGE ) );
            return true;
        }
    }

    return false;
}
