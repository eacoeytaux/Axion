#ifndef Settings_hpp
#define Settings_hpp

#include "axn/system/Utility.hpp"

namespace axn
{

class Settings final
{

public:

    enum Option
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
        DEBUG_AUDIO,
        DEBUG_PHYSICS,
        DEBUG_TERRAIN,
        DEBUG_GRID,
        DEBUG_HEALTH,
        DEBUG_DAMAGE,
        DEBUG_SKELETONS,
        #endif

        SCREENSHOT_FORMAT,

        OPTION_COUNT, // not an option, just a counter
    };

    static void init( );

    static uint value( Option in_opt ) { Assert( !is_zero( in_opt ) ); return( settings[ in_opt ] ); }
    static uint value( Option in_opt, uint in_value ) { Assert( !is_zero( in_opt ) ); return( settings[ in_opt ] = in_value ); }

    static bool get( Option in_opt ) { return( !is_zero( value( in_opt ) ) ); }
    static bool set( Option in_opt, bool in_set ) { value( in_opt, in_set ? 1 : 0 ); return( in_set ); }

    static bool flip( Option in_opt ) { Assert( !is_zero( in_opt ) ); return( set( in_opt, is_zero( settings[ in_opt ] ) ) ); }

    static bool shifty( ) { return( b_shifty ); }
    static bool shifty( bool in_set ) { return( b_shifty = in_set ); }

    static bool controlling( ) { return( b_controlling ); }
    static bool controlling( bool in_set ) { return( b_controlling = in_set ); }

private:

    static carray<uint, OPTION_COUNT> settings;

    static bool b_shifty; // if shift key is held
    static bool b_controlling; // if ctrl key is held

};

} // namespace axn

#endif /* Settings_hpp */
