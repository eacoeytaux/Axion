#ifndef Engine_hpp
#define Engine_hpp

#include "axn.hpp"

#include "Graphics.hpp"
#include "Audio.hpp"
#include "Input.hpp"
#include "World.hpp"

namespace axn {

namespace reality {
class World;
}

class Engine {
private:
    Engine( ) {}

public:
    static const uint FPS = 30; // frames per second

    static err run( World*, const string app_name = "" );
    static err quit( );

    static bool paused( ) { return paused_eng( ); }
    static void pause( bool p ) { pause_eng( p ); }

    // graphics
    static uint screen_width( ) { return screen_width_eng( ); }
    static uint screen_height( ) { return screen_height_eng( ); }
    static void screen_resize( uint width, uint height ) { screen_resize_eng( width, height ); }

    static bool show_cursor( ) { return show_cursor_eng( ); }
    static void show_cursor( bool show ) { show_cursor_eng( show ); }

    // audio
    static ufloat volume( ) { return volume_eng( ); }
    static void volume_up( ) { volume_up_eng( ); }
    static void volume_down( ) { volume_down_eng( ); }
    static bool muted( ) { return muted_eng( ); }
    static void mute( bool m ) { mute_eng( m ); }

private:
    static void init_eng( string app_name );
    static void close_eng( );

    static void input_eng( World*, varray<Input*> & );
    static void update_eng( World* );
    static void render_eng( World* );

    static bool paused_eng( );
    static void pause_eng( bool );

    static uint current_ticks_eng( );
    static void wait_eng( uint ticks );

    // graphics
    static uint screen_width_eng( );
    static uint screen_height_eng( );
    static void screen_resize_eng( uint width, uint height );

    static bool show_cursor_eng( );
    static void show_cursor_eng( bool );

    // audio
    static ufloat volume_eng( );
    static void volume_up_eng( );
    static void volume_down_eng( );
    static bool muted_eng( );
    static void mute_eng( bool );

#ifdef AXN_TEST
    static bool run_tests_eng( );
#endif
};

} // namespace axn

#endif /* Engine_hpp */
