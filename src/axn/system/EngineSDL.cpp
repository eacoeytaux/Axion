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
// ------------------- //
#elif defined( OS_LINUX )
// ------------------- //
#include <SDL.h>
#include <SDL_opengl.h>
// #include <SDL_mixer.h>
// #include <SDL_ttf.h>
// -------------------- //
#endif

#define initAssert( func, msg ) bool macro_unique( assert_check ) = func; Assert( macro_unique( assert_check ), msg )

var_static( bool PAUSED = ( false ) );

var_static( bool ANTI_ALIAS = ( true ) );

var_static( SDL_Window * WINDOW = ( nullptr ) );
var_static( uint WINDOW_WIDTH );
var_static( uint WINDOW_HEIGHT );

var_const( dec VOLUME_INCREMENT = ( 0.0625 ) );
var_static( Slider<dec> VOLUME );
var_static( bool MUTED = ( true ) );

var_const( uint MAX_CONTROLLERS = ( 4 ) );
var_static( SDL_Joystick * CONTROLLERS[ MAX_CONTROLLERS ] );
var_static( SDL_Haptic * CONTROLLER_HAPTICS[ MAX_CONTROLLERS ] );

var_const( int MAX_AXIS_VALUE = ( 32767 ) );

chars SDL_GetErrorStr( )
{
    const uint SDL_ErrorMsgBufferSize = 256;
    char SDL_ErrorMsgStr[ SDL_ErrorMsgBufferSize ];

    return( SDL_GetErrorMsg( SDL_ErrorMsgStr, SDL_ErrorMsgBufferSize ) );
}

void Engine::init_eng( string cref in_app_name )
{
    initAssert( !SDL_Init( SDL_INIT_EVERYTHING ),
                "SDL initialization failed: %s", SDL_GetErrorStr( ) );

    initAssert( !SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 ),
                "SDL GL attributes failed: %s", SDL_GetErrorStr( ) );

    initAssert( !SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 8 ),
                "SDL GL attributes failed: %s", SDL_GetErrorStr( ) );

    initAssert( !SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 ),
                "SDL GL attributes failed: %s", SDL_GetErrorStr( ) );

    initAssert( !SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 1 ),
                "SDL GL attributes failed: %s", SDL_GetErrorStr( ) );

    initAssert( WINDOW = SDL_CreateWindow(
        in_app_name.c_str( ), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP ),
        "SDL window initialization failed: %s", SDL_GetErrorStr( ) );

    SDL_DisplayMode display_mode;
    SDL_GetDesktopDisplayMode( 0, &display_mode );
    WINDOW_WIDTH = display_mode.w;
    WINDOW_HEIGHT = display_mode.h;

    #if defined( OS_WINDOWS )
    SDL_SetWindowFullscreen( WINDOW, 0 );
    SDL_SetWindowSize( WINDOW, WINDOW_WIDTH, WINDOW_HEIGHT );
    SDL_SetWindowPosition( WINDOW, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED );
    #endif

    initAssert( SDL_GL_CreateContext( WINDOW ),
                "SDL window context creation failed: %s", SDL_GetErrorStr( ) );

    initAssert( !ogl::init( ), "OpenGL initialization failed" );

    Assert( ( VOLUME_INCREMENT > 0 ) && ( VOLUME_INCREMENT < 1 ), "volume increment must be ( 0, 1 )" );
    Assert( inverse( VOLUME_INCREMENT ) == floor( inverse( VOLUME_INCREMENT ) ), "volume increment be division of 1" );

    SDL_AudioSpec audio_spec_in, audio_spec_out;
    SDL_memset( &audio_spec_in, 0, sizeof( audio_spec_in ) );
    SDL_memset( &audio_spec_out, 0, sizeof( audio_spec_out ) );

    audio_spec_in.format = AUDIO_F32;
    audio_spec_in.samples = 4096;
    audio_spec_in.freq = 44100;
    audio_spec_in.channels = 2;

    // initAssert( !SDL_OpenAudio( &audio_spec_in, &audio_spec_out ),
    //             "SDL audio initialization failed: %s", SDL_GetErrorStr( ) );

    initAssert( SDL_JoystickEventState( SDL_ENABLE ),
                "SDL controller initialization failed: %s", SDL_GetErrorStr( ) );

    for_range( i, MAX_CONTROLLERS )
    {
        if( ( CONTROLLERS[ i ] = SDL_JoystickOpen( i ) ) )
        {
            if( SDL_JoystickIsHaptic( CONTROLLERS[ i ] ) )
            {
                if( ( CONTROLLER_HAPTICS[ i ] = SDL_HapticOpenFromJoystick( CONTROLLERS[ i ] ) ) )
                {
                    initAssert( SDL_HapticRumbleInit( CONTROLLER_HAPTICS[ i ] ),
                                "SDL controller haptic initialization failed: %s", SDL_GetErrorStr( ) );
                }
            }
        }
    }

    show_cursor_eng( false );
}

void Engine::close_eng( )
{
    for_range( i, MAX_CONTROLLERS )
    {
        SDL_JoystickClose( CONTROLLERS[ i ] );
        SDL_HapticClose( CONTROLLER_HAPTICS[ i ] );
    }

    SDL_DestroyWindow( WINDOW );
    SDL_CloseAudio( );
    SDL_Quit( );
}

void Engine::render_eng( )
{
    SDL_GL_SwapWindow( WINDOW );

    ogl::clear( );
}

bool Engine::paused_eng( ) { return( PAUSED ); }
void Engine::pause_eng( bool in_pause ) { PAUSED = in_pause; }

void Engine::sync_controllers_eng( )
{
    for_range( i, MAX_CONTROLLERS )
    {
        SDL_JoystickClose( CONTROLLERS[ i ] );
        SDL_HapticClose( CONTROLLER_HAPTICS[ i ] );

        if( ( CONTROLLERS[ i ] = SDL_JoystickOpen( i ) ) )
        {
            if( SDL_JoystickIsHaptic( CONTROLLERS[ i ] ) )
            {
                if( ( CONTROLLER_HAPTICS[ i ] = SDL_HapticOpenFromJoystick( CONTROLLERS[ i ] ) ) )
                {
                    SDL_HapticRumbleInit( CONTROLLER_HAPTICS[ i ] );
                }
            }
        }
    }
}

uint Engine::current_ms_eng( ) { return( SDL_GetTicks( ) ); }
void Engine::wait_eng( uint in_ms ) { if( is_pos( in_ms ) ) { SDL_Delay( in_ms - 1 ); } }

bool Engine::show_cursor_eng( ) { return( SDL_ShowCursor( SDL_QUERY ) == SDL_ENABLE ); }
void Engine::show_cursor_eng( bool in_show ) { SDL_ShowCursor( in_show ? SDL_ENABLE : SDL_DISABLE ); }

bool Engine::anti_alias_eng( ) { return( ANTI_ALIAS ); }
void Engine::anti_alias_eng( bool in_show )
{
    ANTI_ALIAS = in_show;

    if( ANTI_ALIAS )
    {
        ogl::enable_anti_alias( );
    }
    else
    {
        ogl::disable_anti_alias( );
    }
}

bool Engine::muted_eng( ) { return( MUTED ); }
void Engine::mute_eng( bool in_muted ) { MUTED = in_muted; }

dec Engine::volume_eng( ) { return( VOLUME.value( ) ); }

void Engine::volume_up_eng( )
{
    if( VOLUME.value( ) < one_minus( VOLUME_INCREMENT ) )
    {
        // Log( INFO_LOG, "volume increased ....... ( + )" );
        VOLUME.delta( VOLUME_INCREMENT );
    }
}

void Engine::volume_down_eng( )
{
    if( VOLUME.value( ) )
    {
        // Log( INFO_LOG, "volume decreased ....... ( - )" );
        VOLUME.delta( -VOLUME_INCREMENT );
    }
}

uint Engine::screen_width_eng( ) { return( WINDOW_WIDTH ); }
uint Engine::screen_height_eng( ) { return( WINDOW_HEIGHT ); }

void Engine::input_eng( list<ptr<Input>> ref in_inputs )
{
    // SDL_HapticRumblePlay( CONTROLLER_HAPTICS[ 0 ], 0.75, 500 );

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
            // -- keyboard --

            b_case( SDL_KEYDOWN ) :
            {
                in_inputs.insert_back( new KeyInput( char( event.key.keysym.sym ), KeyInput::PRESSED ) );
            }

            b_case( SDL_KEYUP ) :
            {
                in_inputs.insert_back( new KeyInput( char( event.key.keysym.sym ), KeyInput::RELEASED ) );
            }

            // -- mouse --

            b_case( SDL_MOUSEMOTION ) :
            {
                in_inputs.insert_back( new MouseInput( MouseInput::NO_BUTTON, MouseInput::MOVE, Vector( event.motion.x, event.motion.y ) ) );
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

            b_case( SDL_MOUSEWHEEL ) :
            {
                in_inputs.insert_back( new MouseInput( MouseInput::SCROLL_BUTTON, MouseInput::MOVE, Vector( event.wheel.x, event.wheel.y ) ) );
            }

            // -- PS4 dualshock --

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

                    if( ( event.jaxis.axis == 0 ) || ( event.jaxis.axis == 1 ) )
                    { // left joystick
                        if( event.jaxis.axis == 0 )
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
            { // exit
                switch( event.window.event )
                {
                    b_case( SDL_WINDOWEVENT_CLOSE ) :
                    {
                        quit( );
                    }
                }
            }

            b_case( SDL_QUIT ) :
            { // exit
                quit( );
            }
        }
    }

    if( ( joystick_x_axis_left != joystick_x_axis_left_new ) ||
        ( joystick_y_axis_left != joystick_y_axis_left_new ) )
    {
        Vector v_old( joystick_x_axis_left, joystick_y_axis_left );

        joystick_x_axis_left = joystick_x_axis_left_new;
        joystick_y_axis_left = joystick_y_axis_left_new;

        Vector v_new( joystick_x_axis_left, joystick_y_axis_left );

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
        Vector v_old( joystick_x_axis_right, joystick_y_axis_right );

        joystick_x_axis_right = joystick_x_axis_right_new;
        joystick_y_axis_right = joystick_y_axis_right_new;

        Vector v_new( joystick_x_axis_right, joystick_y_axis_right );

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
