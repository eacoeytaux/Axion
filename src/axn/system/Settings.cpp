#include "axn/system/Settings.hpp"

#include "axn/system/Engine.hpp"

// -- static defines -- //

uint Identifiable::total_ids = 0; // must be 0
uint Identifiable::total_types = 0; // must be 0
bool Stringable::labeling = true;

#if defined( AXN_DEBUG )
bool Debug::bbreakpoint = true;
bool Debug::active = false;
bool Assert::active = true;
#endif

// -- Settings -- //

carray<uint, Settings::OPTION_COUNT> Settings::settings = carray<uint, Settings::OPTION_COUNT>( );

bool Settings::b_shifty = false;
bool Settings::b_controlling = false;

void Settings::init( )
{
    set( GRAPHICS_DETAIL_ALPHA, true );
    set( GRAPHICS_DETAIL_LOWPOLY, false );
    set( GRAPHICS_DETAIL_PARTICLES, true );

    set( GRAPHICS_GRAYSCALE, false );

    #if defined( AXN_DEBUG )
    set( DEBUG_BACKGROUND_COLOR, false );
    set( DEBUG_SHOW_BACKGROUND_ALL, true );
    set( DEBUG_SHOW_BACKGROUND, true );
    set( DEBUG_SHOW_FOREGROUND, true );
    set( DEBUG_SHOW_TERRAIN, true );
    set( DEBUG_SHOW_OBJECTS, true );
    set( DEBUG_SHOW_VISUAL_BBOX, false );
    set( DEBUG_CAMERA_ZOOM_LOCK, true );
    set( DEBUG_CAMERA_DISPLAY, false );
    set( DEBUG_AUDIO, true );
    set( DEBUG_PHYSICS, true );
    set( DEBUG_TERRAIN, true );
    set( DEBUG_GRID, false );
    set( DEBUG_HEALTH, true );
    set( DEBUG_DAMAGE, true );
    set( DEBUG_SKELETONS, true );
    #endif

    value( SCREENSHOT_FORMAT, Engine::BMP );
}
