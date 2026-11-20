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

    // C! an input read back from its serialized( ) text, null when the text isn't one:
    // C! "KEY <code> <DYNAMIC>", "MOUSE <BUTTON> <DYNAMIC> <x> <y>", "BUTTON <BUTTON> <DYNAMIC>", "JOYSTICK <JOYSTICK> <x> <y>", or "JOYSTICK <JOYSTICK> DEAD"
    static ptr<Input> deserialized( string cref in_text );

};

class Engine
{

private:

    Engine( ) { }

public:

    static const uint FPS; // frames per second

    static Error run( ptr<World> in_world );

    static void quit( );

    static bool paused( ) { return( paused_eng( ) ); }
    static void pause( bool in_pause ) { pause_eng( in_pause ); }

    #if defined( AXN_DEBUG_CONTROLS )
    static void step( );
    #endif

    static void sync_controllers( ) { sync_controllers_eng( ); }

    // -- graphics -- //

    static FixedRectangle screen_bounds( ) { return( FixedRectangle( screen_width( ), screen_height( ) ) ); }

    static uint screen_width( ) { return( screen_width_eng( ) ); }
    static uint screen_height( ) { return( screen_height_eng( ) ); }

    static bool show_cursor( ) { return( show_cursor_eng( ) ); }
    static void show_cursor( bool in_show ) { show_cursor_eng( in_show ); }

    static bool anti_alias( ) { return( anti_alias_eng( ) ); }
    static void anti_alias( bool in_aa ) { anti_alias_eng( in_aa ); }

    enum ImageFormat
    {
        NO_IMAGE_FORMAT = 0,
        BMP,
        PNG,
    };

    static void screenshot( ImageFormat in_format, FixedRectangle cref in_bounds = FixedRectangle( ) ); // C! saved in run_directory( "screenshots" ) after the next render

    // -- files -- //

    // C! tmp/run-<ms>/, where ms is when this run started (milliseconds since epoch), or the folder in_folder inside it.
    // C! Each folder is made the first time it's asked for, so a run only gets the folders it uses
    static string run_directory( string cref in_folder = nostring );

    // C! where the resource files are: res/, found in the working directory (VS and Xcode both run from the project folder), or else in
    // C! the nearest folder above the executable that has one. resource( "sounds/boop.wav" ) is that folder + "sounds/boop.wav"
    static string resource_directory( );
    static string resource( string cref in_filename ) { return( resource_directory( ) + in_filename ); }

    // -- audio -- //

    // C! the global volume and mute, and the sound effects' (sfx) and music's own. The sfx and music volumes are each multiplied by the
    // C! global one, and muting the global volume silences both; up and down move a volume one step (its *_INCREMENT in EngineSDL.cpp)
    static declmp volume( ) { return( volume_eng( ) ); }
    static void volume( declmp in_volume ) { volume_eng( in_volume ); }
    static void volume_up( ) { volume_up_eng( ); }
    static void volume_down( ) { volume_down_eng( ); }
    static bool muted( ) { return( muted_eng( ) ); }
    static void mute( bool in_mute ) { mute_eng( in_mute ); }

    static declmp sfx_volume( ) { return( sfx_volume_eng( ) ); }
    static void sfx_volume( declmp in_volume ) { sfx_volume_eng( in_volume ); }
    static void sfx_volume_up( ) { sfx_volume_up_eng( ); }
    static void sfx_volume_down( ) { sfx_volume_down_eng( ); }
    static bool sfx_muted( ) { return( sfx_muted_eng( ) ); }
    static void sfx_mute( bool in_mute ) { sfx_mute_eng( in_mute ); }

    static declmp music_volume( ) { return( music_volume_eng( ) ); }
    static void music_volume( declmp in_volume ) { music_volume_eng( in_volume ); }
    static void music_volume_up( ) { music_volume_up_eng( ); }
    static void music_volume_down( ) { music_volume_down_eng( ); }
    static bool music_muted( ) { return( music_muted_eng( ) ); }
    static void music_mute( bool in_mute ) { music_mute_eng( in_mute ); }

    static Speaker ref speaker( ); // todo make this more robust

    // C! at the sound's own volume times in_heard_volume (how loudly the Listener that hears it best hears it) times the engine's, at its speed
    // C! and pitch; one that isn't loaded( ) only sets off sound_error( )
    static void play_sound( SoundFX cref in_sound, declmp in_heard_volume = 1.0 );
    static varray<dec> load_sound( string cref in_filename ) { return( load_sound_eng( in_filename ) ); } // C! in_filename's samples (in res/), ready to play: empty when it can't be loaded

    // C! the loudest sound playing right now, at its own volume through its speaker (not the engine's volume or mute), 0 when none is
    static void stop_audio( bool in_sounds_only = false ) { stop_sounds_eng( ); if( !in_sounds_only ) { stop_music_eng( ); } } // C! every sound playing cut off, and the music too unless in_sounds_only

    static void play_music( Music cref in_music ) { play_music_eng( in_music ); } // C! in place of any music playing, at its own volume times the engine's
    static void stop_music( ) { stop_music_eng( ); }

    static declmp loudest_sound( ); // C! the loudest heard volume of the sounds playing (see play_sound( )), whatever their own volume

    static ticks sound_length( SoundFX cref in_sound ) { return( sound_length_eng( in_sound ) ); } // C! how many frames of the game it plays for, at its speed and pitch (0 when it isn't loaded( ))

    static bool sound_error( ); // C! a sound that isn't loaded( ) was played in the last SOUND_ERROR_TICKS
    static void tick_sound_error( ); // C! once a pass of the main loop

    // -- tests -- //

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

    // -- graphics -- //

    static uint screen_width_eng( );
    static uint screen_height_eng( );

    static bool show_cursor_eng( );
    static void show_cursor_eng( bool in_show );

    static bool anti_alias_eng( );
    static void anti_alias_eng( bool in_aa );

    static void screenshot_eng( ImageFormat in_format, FixedRectangle cref in_bounds ); // C! reads the screen and saves it, between the render and the swap

    // -- files -- //

    static void make_directory_eng( string cref in_directory ); // C! does nothing when it's already there
    static bool directory_exists_eng( string cref in_directory );
    static string executable_directory_eng( ); // C! the folder the executable is in, ending in a /, or nostring when it can't be found

    // -- audio -- //

    static declmp volume_eng( );
    static void volume_eng( declmp in_volume );
    static void volume_up_eng( );
    static void volume_down_eng( );
    static bool muted_eng( );
    static void mute_eng( bool in_mute );

    static declmp sfx_volume_eng( );
    static void sfx_volume_eng( declmp in_volume );
    static void sfx_volume_up_eng( );
    static void sfx_volume_down_eng( );
    static bool sfx_muted_eng( );
    static void sfx_mute_eng( bool in_mute );

    static declmp music_volume_eng( );
    static void music_volume_eng( declmp in_volume );
    static void music_volume_up_eng( );
    static void music_volume_down_eng( );
    static bool music_muted_eng( );
    static void music_mute_eng( bool in_mute );

    static varray<dec> load_sound_eng( string cref in_filename );
    static void play_sound_eng( SoundFX cref in_sound, declmp in_volume );

    static ticks sound_length_eng( SoundFX cref in_sound );

    static declmp loudest_sound_eng( );

    static void stop_sounds_eng( );

    static void play_music_eng( Music cref in_music );
    static void stop_music_eng( );

};

struct KeyInput : public Input
{

    typedef char Key;
    enum Dynamic
    {
        NO_DYNAMIC = 0,
        RELEASED,
        PRESSED,
        HELD,
    };

    KeyInput( Key in_key, Dynamic in_dynamic ) : key( in_key + ( ( ( in_key >= 'A' ) && ( in_key <= 'Z' ) ) ? ( 'A' - 'a' ) : 0 ) ), dynamic( in_dynamic ) { }

    virtual string serialized( ) const override;

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
    };

    enum Dynamic
    {
        NO_DYNAMIC = 0,
        RELEASED,
        PRESSED,
        HELD,
        MOVE,
    };

    MouseInput( Button in_button, Dynamic in_dynamic, Vector cref in_vec = V0 ) : button( in_button ), dynamic( in_dynamic ), movement( in_vec ) { }

    virtual string serialized( ) const override;

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
    };

    enum Dynamic
    {
        NO_DYNAMIC = 0,
        RELEASED,
        PRESSED,
        HELD,
        MOVE,
    };

    ControllerButtonInput( Button in_button, Dynamic in_dynamic ) : button( in_button ), dynamic( in_dynamic ) { }

    virtual string serialized( ) const override;

    const Button button;
    const Dynamic dynamic;

};

struct ControllerJoystickInput : public ControllerInput
{

    static const dec DEAD_ZONE;

    static bool in_dead_zone( dec in_d ) { return( in_d < DEAD_ZONE ); }
    static bool in_dead_zone( Vector cref in_vec ) { return( !in_vec.has_magnitude( ) || in_dead_zone( in_vec.magnitude( ) ) ); }

    enum Joystick
    {
        NO_JOYSTICK = 0,
        RIGHT_JOYSTICK,
        LEFT_JOYSTICK,
    };

    ControllerJoystickInput( Joystick in_joystick ) : joystick( in_joystick ), dead_zone( true ) { }
    ControllerJoystickInput( Joystick in_joystick, Vector cref in_vec ) : vector( in_vec ), joystick( in_joystick ), dead_zone( false ) { }

    virtual string serialized( ) const override;

    const Joystick joystick;
    const Vector vector;
    const bool dead_zone;

};

} // namespace axn

#endif /* Engine_hpp */
