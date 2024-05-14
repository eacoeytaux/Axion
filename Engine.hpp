#ifndef Engine_hpp
#define Engine_hpp

#include "AXN.hpp"

#include "Audio.hpp"
#include "Graphics.hpp"

namespace axn
{

namespace reality
{

class World;

}

struct Input
{
    virtual void do_nothing( ) { }
};

class Engine
{
private:
    Engine( ) { }

public:
    static const uint FPS = 30; // frames per second

    static error run( World * world, const string app_name = "" );
    static error quit( );

    static bool paused( ) { return paused_eng( ); }
    static void pause( bool p ) { pause_eng( p ); }
#ifdef AXN_DEBUG
    static void step( );
#endif

    static void sync_controllers( ) { sync_controllers_eng( ); }

    // graphics
    static uint screen_width( ) { return screen_width_eng( ); }
    static uint screen_height( ) { return screen_height_eng( ); }
    static void screen_resize( uint width, uint height ) { screen_resize_eng( width, height ); }

    static bool show_cursor( ) { return show_cursor_eng( ); }
    static void show_cursor( bool show ) { show_cursor_eng( show ); }

    static bool anti_alias( ) { return anti_alias_eng( ); }
    static void anti_alias( bool aa ) { anti_alias_eng( aa ); }

    // audio
    static double volume( ) { return volume_eng( ); }
    static void volume_up( ) { volume_up_eng( ); }
    static void volume_down( ) { volume_down_eng( ); }
    static bool muted( ) { return muted_eng( ); }
    static void mute( bool m ) { mute_eng( m ); }

private:
    static void init_eng( string app_name );
    static void close_eng( );

    static void input_eng( varray<Input *> &, World * );
    static void update_eng( );
    static void render_eng( );

    static bool paused_eng( );
    static void pause_eng( bool );

    static void sync_controllers_eng( );

    static uint current_ticks_eng( );
    static void wait_eng( uint ticks );

    // graphics
    static uint screen_width_eng( );
    static uint screen_height_eng( );
    static void screen_resize_eng( uint width, uint height );

    static bool show_cursor_eng( );
    static void show_cursor_eng( bool );

    static bool anti_alias_eng( );
    static void anti_alias_eng( bool );

    // audio
    static double volume_eng( );
    static void volume_up_eng( );
    static void volume_down_eng( );
    static bool muted_eng( );
    static void mute_eng( bool );
};

struct KeyInput : public Input
{
    typedef char KEY;
    enum DYNAMIC
    {
        NO_DYNAMIC,
        PRESSED,
        HELD,
        RELEASED,
    };

    KeyInput( KEY k, DYNAMIC d ) : key( k + ( ( ( k >= 'A' ) && ( k <= 'Z' ) ) ? ( 'A' - 'a' ) : 0 ) ), dynamic( d ) { }

    const KEY key;
    const DYNAMIC dynamic;
};

struct MouseInput : public Input
{
    enum BUTTON
    {
        NO_BUTTON,
        LEFT_BUTTON,
        RIGHT_BUTTON,
        SCROLL_BUTTON,
    };

    enum DYNAMIC
    {
        NO_DYNAMIC,
        PRESSED,
        HELD,
        RELEASED,
        MOVE,
    };

    MouseInput( BUTTON b, DYNAMIC d, const Coordinate & p ) : button( b ), dynamic( d ), position( p ) { }

    const BUTTON button;
    const DYNAMIC dynamic;
    const Coordinate position;
};

struct ControllerInput : public Input
{
};

struct ControllerButtonInput : public ControllerInput
{
    enum BUTTON
    {
        NO_BUTTON,
        LEFT_UP_BUTTON,
        LEFT_DOWN_BUTTON,
        LEFT_LEFT_BUTTON,
        LEFT_RIGHT_BUTTON,
        RIGHT_UP_BUTTON,
        RIGHT_DOWN_BUTTON,
        RIGHT_LEFT_BUTTON,
        RIGHT_RIGHT_BUTTON,
        L1_BUTTON,
        L2_BUTTON,
        L3_BUTTON,
        R1_BUTTON,
        R2_BUTTON,
        R3_BUTTON,
        HOME_BUTTON,
        START_BUTTON,
        START_OPPOSITE_BUTTON,
        TOUCHPAD_BUTTON,
    };

    enum DYNAMIC
    {
        NO_DYNAMIC,
        PRESSED,
        HELD,
        RELEASED,
        MOVE,
    };

    ControllerButtonInput( BUTTON b, DYNAMIC d ) : button( b ), dynamic( d ) { }

    const BUTTON button;
    const DYNAMIC dynamic;
};

struct ControllerJoystickInput : public ControllerInput
{
    enum JOYSTICK
    {
        NO_JOYSTICK,
        RIGHT_JOYSTICK,
        LEFT_JOYSTICK,
    };

    enum DIRECTION
    {
        NO_DIRECTION,
        UP,
        DOWN,
        LEFT,
        RIGHT,
    };

    ControllerJoystickInput( JOYSTICK j, const Vector & v ) : vector( v ),
                                                              joystick( j ),
                                                              direction( v.destination( ).rotate( -half( RIGHT_ANGLE ) ).in_quadrant( Q1 ) ? UP : v.destination( ).rotate( -half( RIGHT_ANGLE ) ).in_quadrant( Q3 ) ? DOWN
                                                                                                                                              : v.destination( ).rotate( -half( RIGHT_ANGLE ) ).in_quadrant( Q2 )   ? LEFT
                                                                                                                                              : v.destination( ).rotate( -half( RIGHT_ANGLE ) ).in_quadrant( Q4 )   ? RIGHT
                                                                                                                                                                                                                    : NO_DIRECTION ) { }

    const Vector vector;
    const JOYSTICK joystick;
    const DIRECTION direction;
};

} // namespace axn

#endif /* Engine_hpp */
