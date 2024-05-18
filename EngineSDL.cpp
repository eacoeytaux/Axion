#include "Engine.hpp"
#include "World.hpp"

#include "OGL.hpp"
#include "OS.hpp"
#if defined( OS_WINDOWS )
// ---------------- //
#include <SDL.h>
#include <SDL_opengl.h>
// #include <SDL_mixer.h>
// #include <SDL_image.h>
// ---------------- //
#elif defined( OS_APPLE )
// ---------------- //
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wquoted-include-in-framework-header"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// #include <SDL2_mixer/SDL_mixer.h>
// #include <SDL2_mixer/SDL_image.h>

#pragma GCC diagnostic pop
// ---------------- //
#elif defined( OS_LINUX )
// ---------------- //
// todo
// ---------------- //
#endif

namespace axn
{

bool PAUSED = false;

SDL_Window * WINDOW = nullptr;
uint WINDOW_WIDTH;
uint WINDOW_HEIGHT;
bool ANTI_ALIAS = true;

bool MUTED = true;
dec VOLUME_INCREMENT = 0.0625;
Slider<dec> VOLUME;

const uint MAX_CONTROLLERS = 4;
SDL_Joystick * CONTROLLERS[ MAX_CONTROLLERS ];

char * SDL_GetErrorStr( )
{
    const uint SDL_ErrorMsgBufferSize = 256;
    char SDL_ErrorMsgStr[ SDL_ErrorMsgBufferSize ];
    return SDL_GetErrorMsg( SDL_ErrorMsgStr, SDL_ErrorMsgBufferSize );
}

void Engine::init_eng( const string _app_name )
{
    bool assert_check = false;

    Assert( ( VOLUME_INCREMENT > 0 ) && ( VOLUME_INCREMENT < 1 ), "volume increment must be ( 0, 1 )" );
    Assert( inverse( VOLUME_INCREMENT ) == floor( inverse( VOLUME_INCREMENT ) ), "volume increment be division of 1" );

    assert_check = !SDL_Init( SDL_INIT_EVERYTHING );
    Assert( assert_check, "SDL initialization failed: ", SDL_GetErrorStr( ) );

    assert_check = !SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
    Assert( assert_check, "SDL GL Attributes failed: ", SDL_GetErrorStr( ) );

    assert_check = !SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 8 );
    Assert( assert_check, "SDL GL Attributes failed: ", SDL_GetErrorStr( ) );

    assert_check = !SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
    Assert( assert_check, "SDL GL Attributes failed: ", SDL_GetErrorStr( ) );

    assert_check = WINDOW = SDL_CreateWindow(
        _app_name.c_str( ),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP );
    Assert( assert_check, "SDL window initialization failed ", SDL_GetErrorStr( ) );

    SDL_DisplayMode display_mode;
    SDL_GetDesktopDisplayMode( 0, &display_mode );
    WINDOW_WIDTH = display_mode.w;
    WINDOW_HEIGHT = display_mode.h;

    assert_check = SDL_GL_CreateContext( WINDOW );
    Assert( assert_check, "SDL window context creation failed: ", SDL_GetErrorStr( ) );

    assert_check = !ogl::init( );
    Assert( assert_check, "OpenGL initialization failed" );

    SDL_AudioSpec audio_spec_in, audio_spec_out;
    SDL_memset( &audio_spec_in, 0, sizeof( audio_spec_in ) );
    SDL_memset( &audio_spec_out, 0, sizeof( audio_spec_out ) );

    audio_spec_in.freq = 44100;
    audio_spec_in.format = AUDIO_F32;
    audio_spec_in.channels = 2;
    audio_spec_in.samples = 4096;

    assert_check = !SDL_OpenAudio( &audio_spec_in, &audio_spec_out );
    Assert( assert_check, "SDL audio initialization failed: ", SDL_GetErrorStr( ) );

    assert_check = SDL_JoystickEventState( SDL_ENABLE );
    Assert( assert_check, "SDL controller initialization failed: ", SDL_GetErrorStr( ) );
    for_range( MAX_CONTROLLERS ) CONTROLLERS[ i ] = SDL_JoystickOpen( i );

    show_cursor_eng( false );
}

void Engine::close_eng( )
{
    for_range( MAX_CONTROLLERS ) SDL_JoystickClose( CONTROLLERS[ i ] );
    SDL_DestroyWindow( WINDOW );
    SDL_CloseAudio( );
    SDL_Quit( );
}

void Engine::render_eng( )
{
    SDL_GL_SwapWindow( WINDOW );
}

bool Engine::paused_eng( ) { return PAUSED; }
void Engine::pause_eng( bool p ) { PAUSED = p; }

void Engine::sync_controllers_eng( )
{
    for_range( MAX_CONTROLLERS )
    {
        SDL_JoystickClose( CONTROLLERS[ i ] );
        CONTROLLERS[ i ] = SDL_JoystickOpen( i );
    }
}

uint Engine::current_ticks_eng( ) { return SDL_GetTicks( ); }
void Engine::wait_eng( const uint _ticks )
{
    if( _ticks )
    {
        // Log( INFO_LOG, "waiting ms ............. ( %i )", ticks );
        SDL_Delay( _ticks );
    }
}

bool Engine::show_cursor_eng( ) { return ( SDL_ShowCursor( SDL_QUERY ) == SDL_ENABLE ); }
void Engine::show_cursor_eng( bool show ) { SDL_ShowCursor( show ? SDL_ENABLE : SDL_DISABLE ); }

bool Engine::anti_alias_eng( ) { return ANTI_ALIAS; }
void Engine::anti_alias_eng( bool show )
{
    ANTI_ALIAS = show;
    if( ANTI_ALIAS )
    {
        ogl::enable_anti_alias( );
    }
    else
    {
        ogl::disable_anti_alias( );
    }
}

bool Engine::muted_eng( ) { return MUTED; }
void Engine::mute_eng( bool muted ) { MUTED = muted; }

dec Engine::volume_eng( ) { return VOLUME.value( ); }

void Engine::volume_up_eng( )
{
    if( VOLUME.value( ) < ( 1.0 - VOLUME_INCREMENT ) )
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

uint Engine::screen_width_eng( ) { return WINDOW_WIDTH; }
uint Engine::screen_height_eng( ) { return WINDOW_HEIGHT; }

Coordinate world_position_from_event( const SDL_Event & event, World * world ) { return world->active_camera( )->screen_to_world( Coordinate( event.motion.x, event.motion.y ) ); }

void Engine::input_eng( varray<Input *> & inputs, World * world )
{
    static int joystick_x_axis_right = 0;
    static int joystick_y_axis_right = 0;

    static int joystick_x_axis_right_new = 0;
    static int joystick_y_axis_right_new = 0;

    SDL_Event event;
    while( SDL_PollEvent( &event ) )
    {
        switch( event.type )
        {
            // keyboard
            case SDL_KEYDOWN :
            {
                inputs.insert_back( new KeyInput( (char)event.key.keysym.sym, KeyInput::PRESSED ) );
                break;
            }
            case SDL_KEYUP :
            {
                inputs.insert_back( new KeyInput( (char)event.key.keysym.sym, KeyInput::RELEASED ) );
                break;
            }

            // mouse
            case SDL_MOUSEMOTION :
            {
                inputs.insert_back( new MouseInput( MouseInput::NO_BUTTON, MouseInput::MOVE, world_position_from_event( event, world ) ) );
                break;
            }
            case SDL_MOUSEBUTTONDOWN :
            {
                switch( event.button.button )
                {
                    case SDL_BUTTON_LEFT :
                    {
                        inputs.insert_back( new MouseInput( MouseInput::LEFT_BUTTON, MouseInput::PRESSED, world_position_from_event( event, world ) ) );
                        break;
                    }
                    case SDL_BUTTON_RIGHT :
                    {
                        inputs.insert_back( new MouseInput( MouseInput::RIGHT_BUTTON, MouseInput::PRESSED, world_position_from_event( event, world ) ) );
                        break;
                    }
                }
                break;
            }
            case SDL_MOUSEBUTTONUP :
            {
                switch( event.button.button )
                {
                    case SDL_BUTTON_LEFT :
                    {
                        inputs.insert_back( new MouseInput( MouseInput::LEFT_BUTTON, MouseInput::RELEASED, world_position_from_event( event, world ) ) );
                        break;
                    }
                    case SDL_BUTTON_RIGHT :
                    {
                        inputs.insert_back( new MouseInput( MouseInput::RIGHT_BUTTON, MouseInput::RELEASED, world_position_from_event( event, world ) ) );
                        break;
                    }
                }
                break;
            }
            case SDL_MOUSEWHEEL :
            {
                inputs.insert_back( new MouseInput( MouseInput::SCROLL_BUTTON, MouseInput::MOVE, Coordinate( event.wheel.x, event.wheel.y ) ) );
                break;
            }

            // PS4 dualshock
            case SDL_JOYBUTTONUP :
            case SDL_JOYBUTTONDOWN :
            {
                ControllerButtonInput::DYNAMIC dynamic = ( event.type == SDL_JOYBUTTONUP ) ? ControllerButtonInput::RELEASED : ControllerButtonInput::PRESSED;

                switch( event.jbutton.button )
                {
                    case 0 :
                    { // X
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::RIGHT_DOWN_BUTTON, dynamic ) );
                        break;
                    }
                    case 1 :
                    { // CIRCLE
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::RIGHT_LEFT_BUTTON, dynamic ) );
                        break;
                    }
                    case 2 :
                    { // SQUARE
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::RIGHT_RIGHT_BUTTON, dynamic ) );
                        break;
                    }
                    case 3 :
                    { // TRIANGLE
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::RIGHT_UP_BUTTON, dynamic ) );
                        break;
                    }
                    case 7 :
                    { // L3
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::L3_BUTTON, dynamic ) );
                        break;
                    }
                    case 8 :
                    { // R3
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::R3_BUTTON, dynamic ) );
                        break;
                    }
                    case 9 :
                    { // L1
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::L1_BUTTON, dynamic ) );
                        break;
                    }
                    case 10 :
                    { // R1
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::R1_BUTTON, dynamic ) );
                        break;
                    }
                    case 11 :
                    { // D-UP
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::LEFT_UP_BUTTON, dynamic ) );
                        break;
                    }
                    case 12 :
                    { // D-DOWN
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::LEFT_DOWN_BUTTON, dynamic ) );
                        break;
                    }
                    case 13 :
                    { // D-LEFT
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::LEFT_LEFT_BUTTON, dynamic ) );
                        break;
                    }
                    case 14 :
                    { // D-RIGHT
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::LEFT_RIGHT_BUTTON, dynamic ) );
                        break;
                    }
                    case 5 :
                    { // HOME
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::HOME_BUTTON, dynamic ) );
                        break;
                    }
                    case 6 :
                    { // OPTIONS
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::START_BUTTON, dynamic ) );
                        break;
                    }
                    case 4 :
                    { // SHARE
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::START_OPPOSITE_BUTTON, dynamic ) );
                        break;
                    }
                    case 15 :
                    { // TOUCH PAD
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::TOUCHPAD_BUTTON, dynamic ) );
                        break;
                    }
                }
                break;
            }
            case SDL_JOYAXISMOTION :
            {
                // L2 and R2
                if( ( event.jaxis.axis == 4 ) || ( event.jaxis.axis == 5 ) )
                {
                    ControllerButtonInput::DYNAMIC dynamic = ( ( event.jaxis.value > 1200 ) ? ControllerButtonInput::PRESSED : ControllerButtonInput::RELEASED );
                    if( event.jaxis.axis == 4 )
                    { // L2
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::L2_BUTTON, dynamic ) );
                    }
                    else if( event.jaxis.axis == 5 )
                    { // R2
                        inputs.insert_back( new ControllerButtonInput( ControllerButtonInput::R2_BUTTON, dynamic ) );
                    }
                }
                else
                {
                    // left joystick
                    if( ( event.jaxis.axis == 0 ) || ( event.jaxis.axis == 1 ) )
                    {
                        if( event.jaxis.axis == 0 )
                        {
                            if( event.jaxis.value > 6400 )
                            {
                                inputs.insert_back( new ControllerJoystickInput( ControllerJoystickInput::LEFT_JOYSTICK, X_HAT ) );
                                break;
                            }
                            else if( event.jaxis.value < -6400 )
                            {
                                inputs.insert_back( new ControllerJoystickInput( ControllerJoystickInput::LEFT_JOYSTICK, -X_HAT ) );
                                break;
                            }
                            else
                            {
                                inputs.insert_back( new ControllerJoystickInput( ControllerJoystickInput::LEFT_JOYSTICK, ZERO_VECTOR ) );
                                break;
                            }
                        }
                        else if( event.jaxis.axis == 1 )
                        {
                            if( event.jaxis.value > 6400 )
                            {
                                inputs.insert_back( new ControllerJoystickInput( ControllerJoystickInput::LEFT_JOYSTICK, -Y_HAT ) );
                                break;
                            }
                            else if( event.jaxis.value < -6400 )
                            {
                                inputs.insert_back( new ControllerJoystickInput( ControllerJoystickInput::LEFT_JOYSTICK, Y_HAT ) );
                                break;
                            }
                            else
                            {
                                // inputs.insert_back( new ControllerJoystickInput( ControllerJoystickInput::LEFT_JOYSTICK, ZERO_VECTOR ) );
                                break;
                            }
                        }
                    }

                    // right joystick
                    if( ( event.jaxis.axis == 2 ) || ( event.jaxis.axis == 3 ) )
                    {
                        if( abs( event.jaxis.value ) > 3200 )
                        {
                            if( event.jaxis.axis == 2 )
                            {
                                joystick_x_axis_right_new = event.jaxis.value;
                            }
                            else if( event.jaxis.axis == 3 )
                            {
                                joystick_y_axis_right_new = -event.jaxis.value;
                            }
                        }
                    }
                }
                break;
            }

            // exit
            case SDL_WINDOWEVENT :
            {
                switch( event.window.event )
                {
                    case SDL_WINDOWEVENT_CLOSE :
                    {
                        quit( );
                        break;
                    }
                }
                break;
            }
            case SDL_QUIT :
            {
                quit( );
                break;
            }
        }
    }

    if( ( joystick_x_axis_right != joystick_x_axis_right_new ) ||
        ( joystick_y_axis_right != joystick_y_axis_right_new ) )
    {
        joystick_x_axis_right = joystick_x_axis_right_new;
        joystick_y_axis_right = joystick_y_axis_right_new;

        inputs.insert_back( new ControllerJoystickInput( ControllerJoystickInput::RIGHT_JOYSTICK, VectorA( Angle( atan2( (dec)joystick_y_axis_right, (dec)joystick_x_axis_right ) ) ) ) );
    }
}

} // namespace axn
