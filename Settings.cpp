#include "Settings.hpp"

uset<Settings::Option> Settings::settings = uset<Settings::Option>( );

void Settings::init( )
{
    set( GRAPHICS_DETAIL_ALPHA, true );
    
    #if defined ( AXN_DEBUG )
    set( DEBUG_BACKGROUND, true );
    set( DEBUG_CAMERA, false );
    set( DEBUG_PHYSICS, true );
    set( DEBUG_GRID, false );
    set( DEBUG_HEALTH, true );
    #endif
}
