#ifndef Engine_hpp
#define Engine_hpp

#include "axn/AXN.hpp"

#include "axn/audio/Audio.hpp"
#include "axn/graphics/Graphics.hpp"

namespace axn
{

namespace reality
{

class World;

}

struct Input : public Serializable
{

    virtual void do_nothing( ) const { }

};

class Engine
{

private:

    Engine( ) { }

public:

    static const uint FPS = 60; // frames per second

    static error run( ptr<World> in_world );
    static error quit( );

    static bool paused( ) { return( paused_eng( ) ); }
    static void pause( bool in_pause ) { pause_eng( in_pause ); }

    #if defined( AXN_DEBUG_CONTROLS )
    static void step( );
    #endif

    static void sync_controllers( ) { sync_controllers_eng( ); }

    // -- graphics --

    static uint screen_width( ) { return( screen_width_eng( ) ); }
    static uint screen_height( ) { return( screen_height_eng( ) ); }

    static FixedRectangle screen_bounds( ) { return( FixedRectangle( screen_width( ), screen_height( ) ) ); }

    static bool show_cursor( ) { return( show_cursor_eng( ) ); }
    static void show_cursor( bool in_show ) { show_cursor_eng( in_show ); }

    static bool anti_alias( ) { return( anti_alias_eng( ) ); }
    static void anti_alias( bool in_aa ) { anti_alias_eng( in_aa ); }

    // -- audio --

    static dec volume( ) { return( volume_eng( ) ); }
    static void volume_up( ) { volume_up_eng( ); }
    static void volume_down( ) { volume_down_eng( ); }
    static bool muted( ) { return( muted_eng( ) ); }
    static void mute( bool in_mute ) { mute_eng( in_mute ); }

    #if defined( AXN_TESTS )
    static bool run_tests_eng( );
    #endif

private:

    static void init_eng( string cref in_app_name );

    static void close_eng( );

    static void update_eng( );
    static void render_eng( );

    static void input_eng( list<ptr<Input>> ref in_inputs );

    static bool paused_eng( );
    static void pause_eng( bool in_pause );

    static void sync_controllers_eng( );

    static uint current_ms_eng( );
    static void wait_eng( uint in_ms );

    // -- graphics --

    static uint screen_width_eng( );
    static uint screen_height_eng( );

    static bool show_cursor_eng( );
    static void show_cursor_eng( bool in_show );

    static bool anti_alias_eng( );
    static void anti_alias_eng( bool in_aa );

    // -- audio --

    static dec volume_eng( );
    static void volume_up_eng( );
    static void volume_down_eng( );
    static bool muted_eng( );
    static void mute_eng( bool in_mute );

};

struct KeyInput : public Input
{

    typedef char Key;
    enum Dynamic
    {
        NO_DYNAMIC = 0,
        PRESSED,
        RELEASED,
        HELD,
        DYNAMICS
    };

    KeyInput( Key in_key, Dynamic in_dynamic ) : key( in_key + ( ( ( in_key >= 'A' ) && ( in_key <= 'Z' ) ) ? ( 'A' - 'a' ) : 0 ) ), dynamic( in_dynamic ) { }

    const Key key;
    const Dynamic dynamic;

};

struct MouseInput : public Input
{

    enum Button
    {
        NO_BUTTON = 0,
        LEFT_BUTTON,
        RIGHT_BUTTON,
        SCROLL_BUTTON,
        BUTTONS
    };

    enum Dynamic
    {
        NO_DYNAMIC = 0,
        PRESSED,
        RELEASED,
        HELD,
        MOVE,
        DYNAMICS
    };

    MouseInput( Button in_button, Dynamic in_dynamic, Vector cref in_vec = V0 ) : button( in_button ), dynamic( in_dynamic ), movement( in_vec ) { }

    const Button button;
    const Dynamic dynamic;
    const Vector movement;

};

struct ControllerInput : public Input
{

};

struct ControllerButtonInput : public ControllerInput
{

    enum Button
    {
        NO_BUTTON = 0,
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
        BUTTONS
    };

    enum Dynamic
    {
        NO_DYNAMIC = 0,
        PRESSED,
        RELEASED,
        HELD,
        MOVE,
        DYNAMICS
    };

    ControllerButtonInput( Button in_button, Dynamic in_dynamic ) : button( in_button ), dynamic( in_dynamic ) { }

    const Button button;
    const Dynamic dynamic;

};

struct ControllerJoystickInput : public ControllerInput
{

    static dec DEAD_ZONE;

    static bool in_dead_zone( dec in_d ) { return( in_d < DEAD_ZONE ); }
    static bool in_dead_zone( Vector cref in_vec ) { return( !in_vec.has_magnitude( ) || in_dead_zone( in_vec.magnitude( ) ) ); }

    enum Joystick
    {
        NO_JOYSTICK = 0,
        RIGHT_JOYSTICK,
        LEFT_JOYSTICK,
        JOYSTICKS
    };

    ControllerJoystickInput( Joystick in_joystick ) : joystick( in_joystick ), dead_zone( true ) { }
    ControllerJoystickInput( Joystick in_joystick, Vector cref in_vec ) : vector( in_vec ), joystick( in_joystick ), dead_zone( false ) { }

    const Joystick joystick;
    const Vector vector;
    const bool dead_zone;

};

} // namespace axn

#endif /* Engine_hpp */
