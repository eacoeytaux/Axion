#include "axn/world/Door.hpp"

var_const( Color FILL_COLOR = ( BLACK ) );

#if defined( AXN_DEBUG )
var_const( Color DEBUG_LOCKED_COLOR = ( RED ) );
var_const( Color DEBUG_UNLOCKED_COLOR = ( GREEN ) );
var_const( dec DEBUG_COLOR_ALPHA = ( 0.2 ) );
#endif

Door::Door( ptr<Room> in_room, Point cref in_position, Planc cref in_width, Planc cref in_height, bool in_locked ) : Object( in_room, in_position ), m_room( in_room ), m_position( in_position ), m_locked( in_locked )
{
    #if defined( AXN_DEBUG )
    draw_debug = true;
    #endif

    space( Polygon::rectangle( in_width, in_height, VectorY( half( in_height ) ) ) );

    stationary( true );
    interactive( true );
}

void Door::render( )
{
    Object::render( );

    draw( FILL_COLOR, space( ) );
    draw( door_drawing( ) );
}

#if defined( AXN_DEBUG )
Drawing Door::debug_overlay( ) const
{
    Drawing debug_overlay;

    if( Settings::get( Settings::DEBUG_PHYSICS ) )
    {
        debug_overlay.draw( ( locked( ) ? DEBUG_LOCKED_COLOR : DEBUG_UNLOCKED_COLOR ).a( DEBUG_COLOR_ALPHA ), space( ) );
    }

    return( debug_overlay.draw( Object::debug_overlay( ) ) );
}
#endif
