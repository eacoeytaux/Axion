#include "axn/system/Engine.hpp"

#include "axn/system/OS.hpp"
#include "axn/system/OGL.hpp"

// ------------------- //
#if defined( OS_WINDOWS )
// ------------------- //
#include <SDL.h>
#include <SDL_opengl.h>
// #include <SDL_mixer.h>
// #include <SDL_ttf.h>
#include <direct.h>
#include <sys/stat.h>
// ------------------- //
#elif defined( OS_APPLE )
// ------------------- //
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wquoted-include-in-framework-header"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
// #include <SDL2_mixer/SDL_mixer.h>
// #include <SDL2_ttf/SDL_ttf.h>
#pragma GCC diagnostic pop
#include <sys/stat.h>
// ------------------- //
#elif defined( OS_LINUX )
// ------------------- //
#include <SDL.h>
#include <SDL_opengl.h>
// #include <SDL_mixer.h>
// #include <SDL_ttf.h>
#include <sys/stat.h>
// -------------------- //
#endif

// C! bmp and png writing for screenshots (lib/stb, public domain), its warnings are not ours
#define STB_IMAGE_WRITE_IMPLEMENTATION
// ------------------- //
#if defined( OS_WINDOWS )
// ------------------- //
#pragma warning( push, 0 )
#include <stb_image_write.h>
#pragma warning( pop )
// ------------------- //
#elif defined( OS_APPLE )
// ------------------- //
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <stb_image_write.h>
#pragma clang diagnostic pop
// ------------------- //
#elif defined( OS_LINUX )
// ------------------- //
#include <stb_image_write.h>
// -------------------- //
#endif

#define initAssert( func, ... ) bool macro_unique( assert_check ) = func; Assert( ( macro_unique( assert_check ) ), __VA_ARGS__ ); macro_end

var_static( bool b_paused = ( false ) );

var_static( bool b_anti_alias = ( true ) );

var_static( SDL_Window * window = ( nullptr ) );
var_static( uint window_width );
var_static( uint window_height );

section( AUDIO )
{
    var_const( bool START_MUTED = ( false ) ); // C! whether the game starts muted (m mutes and unmutes)
    var_const( declmp START_VOLUME = ( 0.5 ) ); // C! the volume the game starts at (= and - turn it up and down)
    var_const( dec VOLUME_INCREMENT = ( 0.0625 ) );

    var_const( bool START_SFX_MUTED = ( START_MUTED ) ); // C! whether the sound effects start muted
    var_const( declmp START_SFX_VOLUME = ( START_VOLUME ) ); // C! the sound effects' volume the game starts at, as a part of the global one
    var_const( dec SFX_VOLUME_INCREMENT = ( VOLUME_INCREMENT ) );

    var_const( bool START_MUSIC_MUTED = ( START_MUTED ) ); // C! whether the music starts muted
    var_const( declmp START_MUSIC_VOLUME = ( START_VOLUME ) ); // C! the music's volume the game starts at, as a part of the global one
    var_const( dec MUSIC_VOLUME_INCREMENT = ( VOLUME_INCREMENT ) );

    var_const( uint SOUND_BUFFER_SAMPLES = ( 1024 ) ); // C! samples per channel the device asks for at once: fewer starts sounds sooner, too few crackles
    var_const( uint SOUND_FREQUENCY = ( 44100 ) ); // C!

    var_const( uint SFX_CHANNELS = ( 16 ) ); // C! the most sounds playing at once: one more replaces whichever has played longest
    var_const( uint MUSIC_CHANNELS = ( 1 ) );

    var_const( dec MAX_PITCH = ( 3.0 ) ); // C! the most a sound can be pitched up or down, in octaves

    var_static( bool b_muted = ( START_MUTED ) );
    var_static( declmp volume = ( START_VOLUME ) );

    var_static( bool b_sfx_muted = ( START_SFX_MUTED ) );
    var_static( declmp sfx_volume = ( START_SFX_VOLUME ) );

    var_static( bool b_music_muted = ( START_MUSIC_MUTED ) );
    var_static( declmp music_volume = ( START_MUSIC_VOLUME ) );

    var_static( SDL_AudioDeviceID audio_device = ( 0 ) ); // C! 0 when there's no sound
    var_static( varray<Voice> music_voices ); // C! the music playing, on its own (MUSIC_CHANNELS at most), only touched with the device locked like the sounds

    var_static( varray<Voice> voices ); // C! the sounds playing, only touched with the device locked (the callback runs on SDL's audio thread)
}

section( CONTROLLERS )
{
    var_const( uint MAX_CONTROLLERS = ( 4 ) );
    var_const( int MAX_AXIS_VALUE = ( 32767 ) );

    var_static( SDL_Joystick * controllers[ MAX_CONTROLLERS ] );
    var_static( SDL_Haptic * controller_haptics[ MAX_CONTROLLERS ] );
}

var_static( atomic<uint> screenshots_saving = ( 0 ) ); // C! screenshots still being written on their own threads

chars SDL_GetErrorStr( )
{
    const uint SDL_ErrorMsgBufferSize = 256;
    char SDL_ErrorMsgStr[ SDL_ErrorMsgBufferSize ];

    return( SDL_GetErrorMsg( SDL_ErrorMsgStr, SDL_ErrorMsgBufferSize ) );
}

// C! fills SDL's buffer with every voice mixed together, at the engine's volume (SDL locks the device while it runs)
void mix_voices( void * in_data, Uint8 * in_stream, int in_length )
{
    float * out = reinterpret_cast< float * >( in_stream );
    const uint c_count = uint( in_length ) / uint( sizeof( float ) );

    for_range( i, c_count ) { out[ i ] = float( 0.0 ); }

    // C! the global volume, and the sound effects' and music's each times it (silent when muted)
    const dec c_global = ( b_muted ? 0.0 : dec( ::volume ) );
    const dec c_sfx = ( b_sfx_muted ? 0.0 : dec( sfx_volume ) ) * c_global;
    const dec c_music = ( b_music_muted ? 0.0 : dec( music_volume ) ) * c_global;

    // C! mixes in_voices into the buffer at in_volume times each one's own, moving each on (a looping one back to its start when it ends),
    // C! and keeps only those still playing
    lambdaf( mix )( varray<Voice> ref in_voices, dec in_volume )
    {
        varray<Voice> playing;

        for_each( voice, in_voices )
        {
            varray<dec> cref samples = *voice.samples( );
            const dec c_volume = voice.volume( ) * in_volume;
            const uint c_frames = samples.size( ) / 2;
            bool ended = false;

            for_range( i, c_count / 2 )
            {
                if( ( uint( voice.at( ) ) + 1 ) >= c_frames )
                {
                    if( !voice.sound( ).loop( ) || ( c_frames < 2 ) ) { ended = true; break; }
                    voice.at( voice.at( ) - dec( c_frames - 1 ) ); // C! round again, keeping how far between frames it was
                }

                // C! between two frames when it's pitched, so it's blended from both (linear interpolation)
                const uint c_frame = uint( voice.at( ) );
                const dec c_blend = voice.at( ) - dec( c_frame );
                for_range( channel, 2 )
                {
                    const dec c_sample = ( samples[ ( c_frame * 2 ) + channel ] * one_minus( c_blend ) ) + ( samples[ ( ( c_frame + 1 ) * 2 ) + channel ] * c_blend );
                    out[ ( i * 2 ) + channel ] += float( c_sample * c_volume );
                }

                voice.at( voice.at( ) + voice.step( ) );
            }

            if( !ended && ( voice.sound( ).loop( ) || ( ( uint( voice.at( ) ) + 1 ) < c_frames ) ) ) { playing.insert_back( voice ); }
        }

        in_voices = playing;
    };

    mix( voices, c_sfx );
    mix( music_voices, c_music );

    for_range( i, c_count ) { out[ i ] = float( minmax( dec( out[ i ] ), -1.0, 1.0 ) ); } // C! loud sounds together clip rather than wrap around
}

// C! frames of the sound moved on for each frame played: 2 is an octave up, 0.5 an octave down. Pitch changes the speed too for now: shifting it alone needs time-stretching
dec playback_step( SoundFX cref in_sound ) { return( in_sound.speed( ) * pow( 2.0, minmax( in_sound.pitch( ), -MAX_PITCH, MAX_PITCH ) ) ); }

void Engine::init_eng( string cref in_app_name )
{
    initAssert( ( !SDL_Init( SDL_INIT_EVERYTHING ) ),
                "SDL initialization failed: %s", SDL_GetErrorStr( ) );

    initAssert( ( !SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 ) ),
                "SDL GL attributes failed: %s", SDL_GetErrorStr( ) );

    initAssert( ( !SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 8 ) ),
                "SDL GL attributes failed: %s", SDL_GetErrorStr( ) );

    initAssert( ( !SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 ) ),
                "SDL GL attributes failed: %s", SDL_GetErrorStr( ) );

    initAssert( ( !SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 1 ) ),
                "SDL GL attributes failed: %s", SDL_GetErrorStr( ) );

    initAssert( ( !SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 ) ), // C! enough steps for a fading drawing to give each of its polygons its own depth
                "SDL GL attributes failed: %s", SDL_GetErrorStr( ) );

    initAssert( ( window = SDL_CreateWindow( charify( in_app_name ),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP ) ),
                "SDL window initialization failed: %s", SDL_GetErrorStr( ) );

    SDL_DisplayMode display_mode;
    SDL_GetDesktopDisplayMode( 0, &display_mode );
    window_width = display_mode.w;
    window_height = display_mode.h;

    #if defined( OS_WINDOWS )
    SDL_SetWindowFullscreen( window, 0 );
    SDL_SetWindowSize( window, window_width, window_height );
    SDL_SetWindowPosition( window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED );
    #endif

    initAssert( ( SDL_GL_CreateContext( window ) ),
                "SDL window context creation failed: %s", SDL_GetErrorStr( ) );

    initAssert( ( !ogl::init( ) ), "OpenGL initialization failed" );

    for_each_const( increment, varray<dec>( { VOLUME_INCREMENT, SFX_VOLUME_INCREMENT, MUSIC_VOLUME_INCREMENT } ) )
    {
        Assert( ( is_pos( increment ) && ( increment < 1 ) ), "volume increment must be ( 0, 1 )" );
        Assert( ( inverse( increment ) == floor( inverse( increment ) ) ), "volume increment must divide 1" );
    }

    SDL_AudioSpec audio_spec_in, audio_spec_out;
    SDL_memset( &audio_spec_in, 0, sizeof( audio_spec_in ) );
    SDL_memset( &audio_spec_out, 0, sizeof( audio_spec_out ) );

    audio_spec_in.format = AUDIO_F32;
    audio_spec_in.samples = Uint16( SOUND_BUFFER_SAMPLES );
    audio_spec_in.freq = SOUND_FREQUENCY;
    audio_spec_in.channels = 2;
    audio_spec_in.callback = mix_voices;

    ::volume = START_VOLUME;

    audio_device = SDL_OpenAudioDevice( nullptr, 0, &audio_spec_in, &audio_spec_out, 0 ); // C! exactly this format, so loaded sounds can be mixed straight in

    if( is_zero( audio_device ) )
    {
        Log( WARNING_LOG, "no sound ............... ( %s )", SDL_GetErrorStr( ) ); // C! the game runs on without it
    }
    else
    {
        SDL_PauseAudioDevice( audio_device, 0 );
    }

    initAssert( ( SDL_JoystickEventState( SDL_ENABLE ) ),
                "SDL controller initialization failed: %s", SDL_GetErrorStr( ) );

    for_range( i, MAX_CONTROLLERS )
    {
        if( ( controllers[ i ] = SDL_JoystickOpen( i ) ) )
        {
            if( SDL_JoystickIsHaptic( controllers[ i ] ) )
            {
                if( ( controller_haptics[ i ] = SDL_HapticOpenFromJoystick( controllers[ i ] ) ) )
                {
                    initAssert( ( SDL_HapticRumbleInit( controller_haptics[ i ] ) ),
                                "SDL controller haptic initialization failed: %s", SDL_GetErrorStr( ) );
                }
            }
        }
    }

    show_cursor_eng( false );
}

void Engine::close_eng( )
{
    while( screenshots_saving.load( ) ) { SDL_Delay( 1 ); } // C! let screenshots still being written finish, so none are left cut off

    for_range( i, MAX_CONTROLLERS )
    {
        SDL_JoystickClose( controllers[ i ] );
        SDL_HapticClose( controller_haptics[ i ] );
    }

    SDL_DestroyWindow( window );
    if( !is_zero( audio_device ) ) { SDL_CloseAudioDevice( audio_device ); audio_device = 0; }
    SDL_Quit( );
}

void Engine::render_eng( ) { SDL_GL_SwapWindow( window ); ogl::clear( ); }

void Engine::input_eng( list<ptr<Input>> ref in_inputs )
{
    // SDL_HapticRumblePlay( controller_haptics[ 0 ], 0.75, 500 );

    static dec joystick_x_axis_left = 0;
    static dec joystick_y_axis_left = 0;

    static dec joystick_x_axis_left_new = 0;
    static dec joystick_y_axis_left_new = 0;

    static dec joystick_x_axis_right = 0;
    static dec joystick_y_axis_right = 0;

    static dec joystick_x_axis_right_new = 0;
    static dec joystick_y_axis_right_new = 0;

    static SDL_Event event;

    while( SDL_PollEvent( &event ) )
    {
        switch( event.type )
        {
            // -- keyboard -- //

            b_case( SDL_KEYUP ) :
            {
                in_inputs.insert_back( new KeyInput( char( event.key.keysym.sym ), KeyInput::RELEASED ) );
            }

            b_case( SDL_KEYDOWN ) :
            {
                in_inputs.insert_back( new KeyInput( char( event.key.keysym.sym ), KeyInput::PRESSED ) );
            }

            // -- mouse -- //

            b_case( SDL_MOUSEMOTION ) :
            {
                in_inputs.insert_back( new MouseInput( MouseInput::NO_BUTTON, MouseInput::MOVE, Vector( event.motion.x, event.motion.y ) ) );
            }

            b_case( SDL_MOUSEBUTTONUP ) :
            {
                switch( event.button.button )
                {
                    b_case( SDL_BUTTON_LEFT ) :
                    {
                        in_inputs.insert_back( new MouseInput( MouseInput::LEFT_BUTTON, MouseInput::RELEASED ) );
                    }

                    b_case( SDL_BUTTON_RIGHT ) :
                    {
                        in_inputs.insert_back( new MouseInput( MouseInput::RIGHT_BUTTON, MouseInput::RELEASED ) );
                    }
                }
            }

            b_case( SDL_MOUSEBUTTONDOWN ) :
            {
                switch( event.button.button )
                {
                    b_case( SDL_BUTTON_LEFT ) :
                    {
                        in_inputs.insert_back( new MouseInput( MouseInput::LEFT_BUTTON, MouseInput::PRESSED ) );
                    }

                    b_case( SDL_BUTTON_RIGHT ) :
                    {
                        in_inputs.insert_back( new MouseInput( MouseInput::RIGHT_BUTTON, MouseInput::PRESSED ) );
                    }
                }
            }

            b_case( SDL_MOUSEWHEEL ) :
            {
                in_inputs.insert_back( new MouseInput( MouseInput::SCROLL_BUTTON, MouseInput::MOVE, Vector( event.wheel.x, event.wheel.y ) ) );
            }

            // -- PS4 dualshock -- //

            b_case( SDL_JOYBUTTONUP ) : a_case( SDL_JOYBUTTONDOWN ) :
            {
                ControllerButtonInput::Dynamic dynamic = ( event.type == SDL_JOYBUTTONUP ) ? ControllerButtonInput::RELEASED : ControllerButtonInput::PRESSED;

                switch( event.jbutton.button )
                {
                    b_case( 0 ) :
                    { // X
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::RIGHT_DOWN_BUTTON, dynamic ) );
                    }

                    b_case( 1 ) :
                    { // CIRCLE
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::RIGHT_LEFT_BUTTON, dynamic ) );
                    }

                    b_case( 2 ) :
                    { // SQUARE
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::RIGHT_RIGHT_BUTTON, dynamic ) );
                    }

                    b_case( 3 ) :
                    { // TRIANGLE
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::RIGHT_UP_BUTTON, dynamic ) );
                    }

                    b_case( 7 ) :
                    { // L3
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::L3_BUTTON, dynamic ) );
                    }

                    b_case( 8 ) :
                    { // R3
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::R3_BUTTON, dynamic ) );
                    }

                    b_case( 9 ) :
                    { // L1
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::L1_BUTTON, dynamic ) );
                    }

                    b_case( 10 ) :
                    { // R1
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::R1_BUTTON, dynamic ) );
                    }

                    b_case( 11 ) :
                    { // D-UP
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::LEFT_UP_BUTTON, dynamic ) );
                    }

                    b_case( 12 ) :
                    { // D-DOWN
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::LEFT_DOWN_BUTTON, dynamic ) );
                    }

                    b_case( 13 ) :
                    { // D-LEFT
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::LEFT_LEFT_BUTTON, dynamic ) );
                    }

                    b_case( 14 ) :
                    { // D-RIGHT
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::LEFT_RIGHT_BUTTON, dynamic ) );
                    }

                    b_case( 5 ) :
                    { // HOME
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::HOME_BUTTON, dynamic ) );
                    }

                    b_case( 6 ) :
                    { // OPTIONS
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::START_BUTTON, dynamic ) );
                    }

                    b_case( 4 ) :
                    { // SHARE
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::START_OPPOSITE_BUTTON, dynamic ) );
                    }

                    b_case( 15 ) :
                    { // TOUCH PAD
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::TOUCHPAD_BUTTON, dynamic ) );
                    }
                }
                break;
            }

            b_case( SDL_JOYAXISMOTION ) :
            {
                if( ( event.jaxis.axis == 4 ) || ( event.jaxis.axis == 5 ) )
                { // L2 and R2
                    ControllerButtonInput::Dynamic dynamic = ( ( event.jaxis.value > 1200 ) ? ControllerButtonInput::PRESSED : ControllerButtonInput::RELEASED );

                    if( event.jaxis.axis == 4 )
                    { // L2
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::L2_BUTTON, dynamic ) );
                    }
                    else if( event.jaxis.axis == 5 )
                    { // R2
                        in_inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::R2_BUTTON, dynamic ) );
                    }
                }
                else
                { // joysticks
                    dec axis_value = dec( event.jaxis.value ) / dec( MAX_AXIS_VALUE );

                    if( is_zero( event.jaxis.axis ) || ( event.jaxis.axis == 1 ) )
                    { // left joystick
                        if( is_zero( event.jaxis.axis ) )
                        {
                            joystick_x_axis_left_new = +axis_value;
                        }
                        else if( event.jaxis.axis == 1 )
                        {
                            joystick_y_axis_left_new = -axis_value;
                        }
                    }

                    if( ( event.jaxis.axis == 2 ) || ( event.jaxis.axis == 3 ) )
                    { // right joystick
                        if( event.jaxis.axis == 2 )
                        {
                            joystick_x_axis_right_new = +axis_value;
                        }
                        else if( event.jaxis.axis == 3 )
                        {
                            joystick_y_axis_right_new = -axis_value;
                        }
                    }
                }
            }

            b_case( SDL_WINDOWEVENT ) :
            {
                switch( event.window.event )
                {
                    b_case( SDL_WINDOWEVENT_CLOSE ) :
                    {
                        quit( );
                    }
                }
            }

            b_case( SDL_QUIT ) :
            {
                quit( );
            }
        }
    }

    if( ( joystick_x_axis_left != joystick_x_axis_left_new ) ||
        ( joystick_y_axis_left != joystick_y_axis_left_new ) )
    {
        Vector v_old( joystick_x_axis_left,
                      joystick_y_axis_left );

        Vector v_new( joystick_x_axis_left,
                      joystick_y_axis_left );

        joystick_x_axis_left = joystick_x_axis_left_new;
        joystick_y_axis_left = joystick_y_axis_left_new;

        if( !ControllerJoystickInput::in_dead_zone( v_new ) )
        {
            in_inputs.insert_back( new ControllerJoystickInput( ControllerJoystickInput::LEFT_JOYSTICK, v_new ) );
        }
        else if( !ControllerJoystickInput::in_dead_zone( v_old ) )
        {
            in_inputs.insert_back( new ControllerJoystickInput( ControllerJoystickInput::LEFT_JOYSTICK ) );
        }
    }

    if( ( joystick_x_axis_right != joystick_x_axis_right_new ) ||
        ( joystick_y_axis_right != joystick_y_axis_right_new ) )
    {
        Vector v_old( joystick_x_axis_right,
                      joystick_y_axis_right );

        Vector v_new( joystick_x_axis_right_new,
                      joystick_y_axis_right_new );

        joystick_x_axis_right = joystick_x_axis_right_new;
        joystick_y_axis_right = joystick_y_axis_right_new;

        if( !ControllerJoystickInput::in_dead_zone( v_new ) )
        {
            in_inputs.insert_back( new ControllerJoystickInput( ControllerJoystickInput::RIGHT_JOYSTICK, v_new ) );
        }
        else if( !ControllerJoystickInput::in_dead_zone( v_old ) )
        {
            in_inputs.insert_back( new ControllerJoystickInput( ControllerJoystickInput::RIGHT_JOYSTICK ) );
        }
    }
}

bool Engine::paused_eng( ) { return( b_paused ); }
void Engine::pause_eng( bool in_pause ) { b_paused = in_pause; }

void Engine::sync_controllers_eng( )
{
    for_range( i, MAX_CONTROLLERS )
    {
        SDL_JoystickClose( controllers[ i ] );
        SDL_HapticClose( controller_haptics[ i ] );

        if( ( controllers[ i ] = SDL_JoystickOpen( i ) ) )
        {
            if( SDL_JoystickIsHaptic( controllers[ i ] ) )
            {
                if( ( controller_haptics[ i ] = SDL_HapticOpenFromJoystick( controllers[ i ] ) ) )
                {
                    SDL_HapticRumbleInit( controller_haptics[ i ] );
                }
            }
        }
    }
}

uint Engine::current_ms_eng( ) { return( SDL_GetTicks( ) ); }

void Engine::wait_eng( uint in_ms ) { if( is_pos( in_ms ) ) { SDL_Delay( in_ms - 1 ); } }

uint Engine::screen_width_eng( ) { return( window_width ); }
uint Engine::screen_height_eng( ) { return( window_height ); }

bool Engine::show_cursor_eng( ) { return( SDL_ShowCursor( SDL_QUERY ) == SDL_ENABLE ); }
void Engine::show_cursor_eng( bool in_show ) { SDL_ShowCursor( in_show ? SDL_ENABLE : SDL_DISABLE ); }

bool Engine::anti_alias_eng( ) { return( b_anti_alias ); }
void Engine::anti_alias_eng( bool in_show ) { if( b_anti_alias = in_show ) { ogl::enable_anti_alias( ); } else { ogl::disable_anti_alias( ); } }

void Engine::screenshot_eng( ImageFormat in_format, FixedRectangle cref in_bounds )
{
    int width = 0;
    int height = 0;

    SDL_GL_GetDrawableSize( window, &width, &height ); // C! the drawable can be bigger than the window on high-DPI screens

    return_if( !is_pos( width ) || !is_pos( height ) );

    int x = 0;
    int y = 0;

    if( is_pos( in_bounds.area( ) ) )
    {
        // C! the bounds are in screen units from the middle, y up like OpenGL, and the drawable can have more pixels than that
        const dec c_scale_x = dec( width ) / dec( screen_width_eng( ) );
        const dec c_scale_y = dec( height ) / dec( screen_height_eng( ) );

        const int c_x_end = min( width, int( round( half( dec( width ) ) + ( dec( in_bounds.upper_bound_x( ) ) * c_scale_x ) ) ) );
        const int c_y_end = min( height, int( round( half( dec( height ) ) + ( dec( in_bounds.upper_bound_y( ) ) * c_scale_y ) ) ) );

        x = max( 0, int( round( half( dec( width ) ) + ( dec( in_bounds.lower_bound_x( ) ) * c_scale_x ) ) ) );
        y = max( 0, int( round( half( dec( height ) ) + ( dec( in_bounds.lower_bound_y( ) ) * c_scale_y ) ) ) );

        width = c_x_end - x;
        height = c_y_end - y;

        return_if( !is_pos( width ) || !is_pos( height ) );
    }

    varray<unsigned char> pixels = ogl::read_pixels( uint( x ), uint( y ), uint( width ), uint( height ) );

    return_if( pixels.empty( ) );

    string extension;

    switch( in_format )
    {
        default:
        {
            Log( WARNING_LOG, "screenshot format not supported" );
            return;
        }

        b_case( BMP ) :
        {
            extension = ".bmp";
        }

        b_case( PNG ) :
        {
            extension = ".png";
        }
    }

    string filename = run_directory( "screenshots" ) + "screenshot-" + to_string( milliseconds( ) ) + extension; // C! the folder is made now, so deleting it while the game runs is fine

    screenshots_saving++;

    stbi_flip_vertically_on_write( 1 ); // OpenGL rows start at the bottom, image rows at the top

    // use thread to prevent freeze
    thread( [ pixels = move( pixels ), filename, width, height, in_format ] ( )
    {
        bool saved = false;

        switch( in_format )
        {
            default:
            {
                saved = false;
            }

            b_case( BMP ) :
            {
                saved = !is_zero( stbi_write_bmp( charify( filename ), width, height, 3, &pixels[ 0 ] ) );
            }

            b_case( PNG ) :
            {
                saved = !is_zero( stbi_write_png( charify( filename ), width, height, 3, &pixels[ 0 ], width * 3 ) );
            }
        }

        if( saved )
        {
            Log( INFO_LOG, "screenshot saved ....... ( %s )", charify( filename ) );
        }
        else
        {
            Log( WARNING_LOG, "screenshot failed ...... ( %s )", charify( filename ) );
        }

        screenshots_saving--;

    } ).detach( );
}

void Engine::make_directory_eng( string cref in_directory )
{
    #if defined( OS_WINDOWS )
    _mkdir( charify( in_directory ) ); // C! fails harmlessly when the folder is already there
    #elif defined( OS_APPLE )
    mkdir( charify( in_directory ), 0755 );
    #elif defined( OS_LINUX )
    mkdir( charify( in_directory ), 0755 );
    #endif
}

bool Engine::directory_exists_eng( string cref in_directory )
{
    #if defined( OS_WINDOWS )
    struct _stat info = { };
    return( is_zero( _stat( charify( in_directory ), &info ) ) && !is_zero( info.st_mode & _S_IFDIR ) );
    #else
    struct stat info = { };
    return( is_zero( stat( charify( in_directory ), &info ) ) && S_ISDIR( info.st_mode ) );
    #endif
}

string Engine::executable_directory_eng( )
{
    char * path = SDL_GetBasePath( );
    return_if( !path, nostring );

    string directory( path );
    SDL_free( path );

    for_each( c, directory ) { if( c == '\\' ) { c = '/'; } } // C! one kind of slash, so the "../" added after it fits
    return( directory );
}

declmp Engine::volume_eng( ) { return( ::volume ); }
void Engine::volume_eng( declmp in_volume ) { SDL_LockAudioDevice( audio_device ); ::volume = in_volume; SDL_UnlockAudioDevice( audio_device ); } // C! locked, since the audio thread reads it
void Engine::volume_up_eng( ) { SDL_LockAudioDevice( audio_device ); ::volume = dec( ::volume ) + VOLUME_INCREMENT; SDL_UnlockAudioDevice( audio_device ); } // C! up to 1 (declmp)
void Engine::volume_down_eng( ) { SDL_LockAudioDevice( audio_device ); ::volume = dec( ::volume ) - VOLUME_INCREMENT; SDL_UnlockAudioDevice( audio_device ); } // C! down to 0 (declmp)
bool Engine::muted_eng( ) { return( b_muted ); }
void Engine::mute_eng( bool in_mute ) { SDL_LockAudioDevice( audio_device ); b_muted = in_mute; SDL_UnlockAudioDevice( audio_device ); }

declmp Engine::sfx_volume_eng( ) { return( ::sfx_volume ); }
void Engine::sfx_volume_eng( declmp in_volume ) { SDL_LockAudioDevice( audio_device ); ::sfx_volume = in_volume; SDL_UnlockAudioDevice( audio_device ); } // C! locked, since the audio thread reads it
void Engine::sfx_volume_up_eng( ) { SDL_LockAudioDevice( audio_device ); ::sfx_volume = dec( ::sfx_volume ) + SFX_VOLUME_INCREMENT; SDL_UnlockAudioDevice( audio_device ); } // C! up to 1 (declmp)
void Engine::sfx_volume_down_eng( ) { SDL_LockAudioDevice( audio_device ); ::sfx_volume = dec( ::sfx_volume ) - SFX_VOLUME_INCREMENT; SDL_UnlockAudioDevice( audio_device ); } // C! down to 0 (declmp)
bool Engine::sfx_muted_eng( ) { return( b_sfx_muted ); }
void Engine::sfx_mute_eng( bool in_mute ) { SDL_LockAudioDevice( audio_device ); b_sfx_muted = in_mute; SDL_UnlockAudioDevice( audio_device ); }

declmp Engine::music_volume_eng( ) { return( ::music_volume ); }
void Engine::music_volume_eng( declmp in_volume ) { SDL_LockAudioDevice( audio_device ); ::music_volume = in_volume; SDL_UnlockAudioDevice( audio_device ); } // C! locked, since the audio thread reads it
void Engine::music_volume_up_eng( ) { SDL_LockAudioDevice( audio_device ); ::music_volume = dec( ::music_volume ) + MUSIC_VOLUME_INCREMENT; SDL_UnlockAudioDevice( audio_device ); } // C! up to 1 (declmp)
void Engine::music_volume_down_eng( ) { SDL_LockAudioDevice( audio_device ); ::music_volume = dec( ::music_volume ) - MUSIC_VOLUME_INCREMENT; SDL_UnlockAudioDevice( audio_device ); } // C! down to 0 (declmp)
bool Engine::music_muted_eng( ) { return( b_music_muted ); }
void Engine::music_mute_eng( bool in_mute ) { SDL_LockAudioDevice( audio_device ); b_music_muted = in_mute; SDL_UnlockAudioDevice( audio_device ); }

varray<dec> Engine::load_sound_eng( string cref in_filename )
{
    varray<dec> samples;

    SDL_AudioSpec spec;
    Uint8 * buffer = nullptr;
    Uint32 length = 0;

    if( !SDL_LoadWAV( charify( resource( in_filename ) ), &spec, &buffer, &length ) )
    {
        Log( WARNING_LOG, "sound not loaded ....... ( %s: %s )", charify( in_filename ), SDL_GetError( ) );
        return( samples );
    }

    SDL_AudioCVT convert;
    if( !is_neg( SDL_BuildAudioCVT( &convert, spec.format, spec.channels, spec.freq, AUDIO_F32, 2, SOUND_FREQUENCY ) ) )
    {
        varray<Uint8> converted( uint( length ) * uint( max( convert.len_mult, 1 ) ) );
        SDL_memcpy( &converted[ 0 ], buffer, length );
        convert.len = int( length );
        convert.buf = &converted[ 0 ];

        if( !SDL_ConvertAudio( &convert ) )
        {
            const float * floats = reinterpret_cast< const float * >( convert.buf );
            const uint c_count = uint( convert.len_cvt ) / uint( sizeof( float ) );

            samples.resize( c_count );
            for_range( i, c_count ) { samples[ i ] = dec( floats[ i ] ); }
        }
    }

    SDL_FreeWAV( buffer );

    if( samples.empty( ) ) { Log( WARNING_LOG, "sound not converted .... ( %s )", charify( in_filename ) ); }

    return( samples );
}

void Engine::play_sound_eng( SoundFX cref in_sound, declmp in_volume )
{
    return_if( !in_sound.loaded( ) );

    Voice voice( in_sound, in_volume );
    voice.step( playback_step( in_sound ) );

    return_if( is_zero( audio_device ) || !is_pos( voice.volume( ) ) );
    return_if( !is_pos( in_sound.speed( ) ) ); // C! a sound that doesn't move on would hold a channel forever

    SDL_LockAudioDevice( audio_device );

    if( voices.size( ) < SFX_CHANNELS )
    {
        voices.insert_back( voice );
    }
    else if( !voices.empty( ) )
    {
        uint longest = 0;
        for_range( i, voices.size( ) ) { if( voices[ i ].at( ) > voices[ longest ].at( ) ) { longest = i; } }
        voices[ longest ] = voice; // C! every channel is busy: the sound that's played longest makes way
    }

    SDL_UnlockAudioDevice( audio_device );
}

ticks Engine::sound_length_eng( SoundFX cref in_sound )
{
    const dec c_step = playback_step( in_sound );
    return_if( !in_sound.loaded( ) || !is_pos( c_step ), 0.0 );
    return( ( dec( half( in_sound.samples( )->size( ) ) ) / ( dec( SOUND_FREQUENCY ) * c_step ) ) * dec( FPS ) ); // C! a left and a right sample for each frame of the sound
}

declmp Engine::loudest_sound_eng( )
{
    Max<declmp> loudest( 0.0 ); // C! 0 when nothing is playing

    if( audio_device )
    {
        SDL_LockAudioDevice( audio_device );
        for_each_const( voice, voices ) { loudest.update( voice.heard_volume( ) ); }
        SDL_UnlockAudioDevice( audio_device );
    }

    return( loudest );
}

void Engine::stop_sounds_eng( )
{
    return_if( is_zero( audio_device ) );

    SDL_LockAudioDevice( audio_device );
    voices.clear( );
    SDL_UnlockAudioDevice( audio_device );
}

void Engine::play_music_eng( Music cref in_music )
{
    return_if( is_zero( audio_device ) );

    SDL_LockAudioDevice( audio_device );
    music_voices.clear( ); // C! in place of what was playing; music that couldn't be loaded just stops it
    if( in_music.loaded( ) ) { music_voices.insert_back( Voice( in_music ) ); }
    while( music_voices.size( ) > MUSIC_CHANNELS ) { music_voices.remove_front( ); }
    SDL_UnlockAudioDevice( audio_device );
}

void Engine::stop_music_eng( )
{
    return_if( is_zero( audio_device ) );

    SDL_LockAudioDevice( audio_device );
    music_voices.clear( );
    SDL_UnlockAudioDevice( audio_device );
}
