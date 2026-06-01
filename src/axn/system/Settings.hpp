#ifndef Settings_hpp
#define Settings_hpp

#include "axn/system/Utility.hpp"

namespace axn
{

class Settings
{

public:

    enum option
    {
        NO_OPTION = 0,

        GRAPHICS_DETAIL_ALPHA,
        GRAPHICS_DETAIL_LOWPOLY,
        GRAPHICS_DETAIL_PARTICLES,

        GRAPHICS_GRAYSCALE,

        #if defined( AXN_DEBUG )
        DEBUG_BACKGROUND_COLOR,
        DEBUG_SHOW_BACKGROUND_ALL,
        DEBUG_SHOW_BACKGROUND,
        DEBUG_SHOW_FOREGROUND,
        DEBUG_SHOW_TERRAIN,
        DEBUG_SHOW_OBJECTS,
        DEBUG_SHOW_VISUAL_BBOX,
        DEBUG_CAMERA_ZOOM_LOCK,
        DEBUG_CAMERA_DISPLAY,
        DEBUG_HEALTH,
        DEBUG_DAMAGE,
        DEBUG_PHYSICS,
        DEBUG_TERRAIN,
        DEBUG_GRID,
        #endif
    };

    static void init( );

    static bool get( option in_opt ) { Assert( !is_zero( in_opt ) ); return( settings.contains( in_opt ) ); }
    static bool set( option in_opt, bool in_set ) { Assert( !is_zero( in_opt ) ); if( in_set ) { settings.insert( in_opt ); } else { settings.remove( in_opt ); } return( in_set ); }

    static bool flip( option in_opt ) { Assert( !is_zero( in_opt ) ); return( set( in_opt, !get( in_opt ) ) ); }

    static bool shifty( ) { return( b_shifty ); }
    static bool shifty( bool in_set ) { b_shifty = in_set; return( in_set ); }

    static bool controlling( ) { return( b_controlling ); }
    static bool controlling( bool in_set ) { b_controlling = in_set; return( in_set ); }

private:

    static uset<option> settings;

    static bool b_shifty; // if shift key is held
    static bool b_controlling; // if ctrl key is held

};

} // namespace axn

#endif /* Settings_hpp */
