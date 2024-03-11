#include "Engine.hpp"

AXN_NAMESPACES

#if defined( __APPLE__ ) // osx
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wquoted-include-in-framework-header"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
// #include <SDL2_mixer/SDL_mixer.h>
// #include <SDL2_mixer/SDL_image.h>
#pragma GCC diagnostic pop
#else
// TODO if linux
// TODO if windows
#endif

namespace axn {

bool PAUSED = false;

uint WINDOW_WIDTH = 1024;
uint WINDOW_HEIGHT = 720;

SDL_Window* WINDOW_OGL = nullptr;

SDL_Window* WINDOW = nullptr;

Slider<ufloat> VOLUME;
ufloat VOLUME_INCREMENT = 0.0625; // must be (0,1), rational, and = 1 / (int)
bool MUTED = true;

inline char* SDL_GetErrorStr( ) {
    const uint SDL_ErrorMsgBufferSize = 256;
    char SDL_ErrorMsgStr[ SDL_ErrorMsgBufferSize ];
    return SDL_GetErrorMsg( SDL_ErrorMsgStr, SDL_ErrorMsgBufferSize );
}

void Engine::init_eng( const string _app_name ) {
    Assert( !(err)SDL_Init( SDL_INIT_EVERYTHING ),
            "SDL2 initialization failed: ", SDL_GetErrorStr( ) );

    Assert( WINDOW_OGL = SDL_CreateWindow( _app_name.c_str( ), SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                                           SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN ),
            "SDL2 rendering initialization failed ", SDL_GetErrorStr( ) );
    Assert( SDL_GL_CreateContext( WINDOW_OGL ),
            "SDL2 context creation failed: ", SDL_GetErrorStr( ) );
    Assert( !ogl::init( ), "OpenGL initialization failed" );

    // Assert( !SDL_JoystickEventState( SDL_ENABLE ), "SDL2 controller
    // initialization failed: ", SDL_GetErrorStr( ) ); Assert(
    // !SDL_JoystickOpen( 0 ), "SDL2 controller initialization failed: ",
    // SDL_GetErrorStr( ) );

    SDL_AudioSpec AUDIO_SPEC_IN, AUDIO_SPEC_OUT;
    SDL_memset( &AUDIO_SPEC_IN, 0, sizeof( AUDIO_SPEC_IN ) );
    SDL_memset( &AUDIO_SPEC_OUT, 0, sizeof( AUDIO_SPEC_OUT ) );

    AUDIO_SPEC_IN.freq = 44100;
    AUDIO_SPEC_IN.format = AUDIO_F32;
    AUDIO_SPEC_IN.channels = 2;
    AUDIO_SPEC_IN.samples = 4096;

    Assert( !(err)SDL_OpenAudio( &AUDIO_SPEC_IN, &AUDIO_SPEC_OUT ),
            "SDL2 audio initialization failed: ", SDL_GetErrorStr( ) );

    show_cursor_eng( true );
}

void Engine::close_eng( ) {
    SDL_CloseAudio( );

    SDL_DestroyWindow( WINDOW_OGL );

    SDL_DestroyWindow( WINDOW );

    // SDL_JoystickClose( 0 );

    SDL_Quit( );
}

void Engine::render_eng( World* world ) {
    ogl::clear( );

    glClearColor( 0.f, 0.f, 0.f, 1.f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity( );

    glEnable( GL_BLEND );
    glBlendEquation( GL_FUNC_ADD );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_ALWAYS );

    for_each( tri, world->active_camera( )->rendering( ) ) {
        ogl::begin_polygons( );

        glColor4f( tri.color1.r( ), tri.color1.g( ), tri.color1.b( ), tri.color1.a( ) );
        glVertex3f( ( floorf( tri.triangle.coordinates( )[ 0 ].x( ) ) / (float)screen_width( ) * 2.f ) - 1.f,
                    ( floorf( tri.triangle.coordinates( )[ 0 ].y( ) ) / (float)screen_height( ) * 2.f ) - 1.f,
                    1.f );
        glColor4f( tri.color2.r( ), tri.color2.g( ), tri.color2.b( ), tri.color2.a( ) );
        glVertex3f( ( floorf( tri.triangle.coordinates( )[ 1 ].x( ) ) / (float)screen_width( ) * 2.f ) - 1.f,
                    ( floorf( tri.triangle.coordinates( )[ 1 ].y( ) ) / (float)screen_height( ) * 2.f ) - 1.f,
                    1.f );
        glColor4f( tri.color3.r( ), tri.color3.g( ), tri.color3.b( ), tri.color3.a( ) );
        glVertex3f( ( floorf( tri.triangle.coordinates( )[ 2 ].x( ) ) / (float)screen_width( ) * 2.f ) - 1.f,
                    ( floorf( tri.triangle.coordinates( )[ 2 ].y( ) ) / (float)screen_height( ) * 2.f ) - 1.f,
                    1.f );

        ogl::end( );
    }

    SDL_GL_SwapWindow( WINDOW_OGL );
}

void Engine::update_eng( World* world ) {}

bool Engine::paused_eng( ) { return PAUSED; }
void Engine::pause_eng( bool p ) { PAUSED = p; }

uint Engine::current_ticks_eng( ) { return SDL_GetTicks( ); }
void Engine::wait_eng( const uint _ticks ) {
    // Logger::log( INFO_LOG, "waiting ms ............. ( %i )", ticks );
    SDL_Delay( _ticks );
}

bool Engine::show_cursor_eng( ) { return ( SDL_ShowCursor( SDL_QUERY ) == SDL_ENABLE ); }
void Engine::show_cursor_eng( bool show ) { SDL_ShowCursor( show ? SDL_ENABLE : SDL_DISABLE ); }

bool Engine::muted_eng( ) { return MUTED; }
void Engine::mute_eng( bool muted ) { MUTED = muted; }
ufloat Engine::volume_eng( ) { return VOLUME.value( ); }

void Engine::volume_up_eng( ) {
    if( VOLUME.value( ) < ( 1.f - VOLUME_INCREMENT ) ) {
        VOLUME.delta( VOLUME_INCREMENT );
        // Logger::log( INFO_LOG, "volume increased ....... ( + )" );
    }
}

void Engine::volume_down_eng( ) {
    if( VOLUME.value( ) ) {
        VOLUME.delta( -VOLUME_INCREMENT );
        // Logger::log( INFO_LOG, "volume decreased ....... ( -)" );
    }
}

uint Engine::screen_width_eng( ) { return WINDOW_WIDTH; }
uint Engine::screen_height_eng( ) { return WINDOW_HEIGHT; }

// inline void mouse_left_click_down( World * world, Camera * camera ) {
//     //Logger::log( INFO_LOG, "mouse down ............. ( left )" );
//     world->player()->fire_arrow( );
// }
//
// inline void mouse_left_click_up( World * world, Camera * camera ) {
//     //Logger::log( INFO_LOG, "mouse up ............... ( left )" );
//     world->player()->release_bow( );
// }
//
// inline void mouse_right_click_down( World * world, Camera * camera ) {
//     //Logger::log( INFO_LOG, "mouse down ............. ( right )" );
//     world->player()->fire_hook( );
// }
//
// inline void mouse_right_click_up( World * world, Camera * camera ) {
//     //Logger::log( INFO_LOG, "mouse up ............... ( right )" );
//     world->player()->release_bow( );
// }
//
// inline void mouse_move( World * world, Camera * camera, const int _x, const
// int _y ) {
//     //Logger::log( INFO_LOG, "mouse move ............. ( %i, %i )", _x, _y );
//     MOUSE_POSITION_SCREEN = Coordinate( _x, _y );
//     Coordinate world_pos = MOUSE_POSITION_SCREEN + camera->center() - Vector(
//     camera->width(), camera->height() ).half(); world_pos.y( -( world_pos.y()
//     - camera->center().y() ) + camera->center().y() ); MOUSE_POSITION_WORLD =
//     Vector( camera->center(), world_pos) / camera->zoom();
//
//     //if ( !PAUSED ) if ( world->player() ) world->player()->aim( Angle(
//     world->player()->position(), MOUSE_POSITION_WORLD ) );
// }
//
// inline void mouse_scroll_up( World * world, Camera * camera ) {
//     //Logger::log( INFO_LOG, "mouse scroll ........... ( up )" );
//     //camera->zoom( camera->zoom() / CAMERA_ZOOM_RATIO );
//     //mouse_move( world, camera, MOUSE_POSITION_SCREEN.x(),
//     MOUSE_POSITION_SCREEN.y() );
// }
//
// inline void mouse_scroll_down( World * world, Camera * camera ) {
//     //Logger::log( INFO_LOG, "mouse scroll ........... ( down )" );
//     //camera->zoom( camera->zoom() * CAMERA_ZOOM_RATIO );
//     //mouse_move( world, camera, MOUSE_POSITION_SCREEN.x(),
//     MOUSE_POSITION_SCREEN.y() );
// }

inline Coordinate world_position_from_event( const SDL_Event & event, const World* world ) {
    return world->active_camera( )->screen_to_world(
        Coordinate( event.motion.x, event.motion.y ) );
}
void Engine::input_eng( World* world, varray<Input*> & inputs ) {
    SDL_Event event;
    while( SDL_PollEvent( &event ) ) {
        switch( event.type ) {
        // keyboard
        case SDL_KEYDOWN: {
            inputs.push_back( new KeyInput( (char)event.key.keysym.sym, KeyInput::PRESSED ) );
            break;
        }
        case SDL_KEYUP: {
            inputs.push_back( new KeyInput( (char)event.key.keysym.sym, KeyInput::RELEASED ) );
            break;
        }

        // mouse
        case SDL_MOUSEMOTION: {
            inputs.push_back( new MouseInput( MouseInput::NO_BUTTON, MouseInput::MOVE,
                                              world_position_from_event( event, world ) ) );
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            switch( event.button.button ) {
            case SDL_BUTTON_LEFT: {
                inputs.push_back( new MouseInput( MouseInput::LEFT_BUTTON, MouseInput::PRESSED,
                                                  world_position_from_event( event, world ) ) );
                break;
            }
            case SDL_BUTTON_RIGHT: {
                inputs.push_back( new MouseInput( MouseInput::RIGHT_BUTTON, MouseInput::PRESSED,
                                                  world_position_from_event( event, world ) ) );
                break;
            }
            }
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            switch( event.button.button ) {
            case SDL_BUTTON_LEFT: {
                inputs.push_back( new MouseInput( MouseInput::LEFT_BUTTON, MouseInput::RELEASED,
                                                  world_position_from_event( event, world ) ) );
                break;
            }
            case SDL_BUTTON_RIGHT: {
                inputs.push_back( new MouseInput( MouseInput::RIGHT_BUTTON,
                                                  MouseInput::RELEASED,
                                                  world_position_from_event( event, world ) ) );
                break;
            }
            }
            break;
        }
        case SDL_MOUSEWHEEL: {
            // const bool SCROLL_FIXED = false;
            // if ( ( SCROLL_FIXED ? -1 : 1 ) * event.wheel.y > 0 )
            // mouse_scroll_up( world, camera ); if ( ( SCROLL_FIXED ? -1 : 1 )
            // * event.wheel.y < 0 ) mouse_scroll_down( world, camera );
            break;
        }
            //            // PS4 dualshock
            //            case SDL_JOYBUTTONDOWN: {
            //                switch ( event.jbutton.button ) {
            //                    case 0: { // X
            //                        //if ( !PAUSED ) world->player()->jump(
            //                        true ); break;
            //                    }
            //                    case 1: { // CIRCLE
            //                        break;
            //                    }
            //                    case 2: { // SQUARE
            //                        break;
            //                    }
            //                    case 3: { // TRIANGLE
            //                        break;
            //                    }
            //                    case 9: { // L1
            //                        //if ( !PAUSED ) world->player()->jump(
            //                        true ); break;
            //                    }
            //                    case 10: { // R1
            //                        //if ( !PAUSED )
            //                        world->player()->fire_arrow( ); break;
            //                    }
            //                    case 11: { // D-UP
            //                        //if ( !PAUSED ) camera->zoom(
            //                        camera->zoom() / CAMERA_ZOOM_RATIO );
            //                        break;
            //                    }
            //                    case 12: { // D-DOWN
            //                        //if ( !PAUSED ) camera->zoom(
            //                        camera->zoom() * CAMERA_ZOOM_RATIO );
            //                        break;
            //                    }
            //                    case 13: { // D-LEFT
            //                        break;
            //                    }
            //                    case 14: { // D-RIGHT
            //                        break;
            //                    }
            //                    case 7: { // L3
            //                        break;
            //                    }
            //                    case 8: { // R3
            //                        break;
            //                    }
            //                    case 6: { // OPTIONS
            //                        break;
            //                    }
            //                    case 4: { // SHARE
            //                        break;
            //                    }
            //                    case 5: { // HOME
            //                        break;
            //                    }
            //                    case 15: { // TOUCH PAD
            //                        break;
            //                    }
            //                }
            //                break;
            //            }
            //            case SDL_JOYBUTTONUP: {
            //                switch ( event.jbutton.button ) {
            //                    case 0: { // X
            //                        //if ( !PAUSED )
            //                        world->player()->jump( false ); break;
            //                    }
            //                    case 1: { // CIRCLE
            //                        break;
            //                    }
            //                    case 2: { // SQUARE
            //                        break;
            //                    }
            //                    case 3: { // TRIANGLE
            //                        break;
            //                    }
            //                    case 9: { // L1
            //                        //if ( !PAUSED ) world->player()->jump(
            //                        false ); break;
            //                    }
            //                    case 10: { // R1
            //                        //if ( !PAUSED )
            //                        world->player()->release_bow( ); break;
            //                    }
            //                    case 11: { // D-UP
            //                        break;
            //                    }
            //                    case 12: { // D-DOWN
            //                        break;
            //                    }
            //                    case 13: { // D-LEFT
            //                        break;
            //                    }
            //                    case 14: { // D-RIGHT
            //                        break;
            //                    }
            //                    case 7: { // L3
            //                        break;
            //                    }
            //                    case 8: { // R3
            //                        break;
            //                    }
            //                    case 6: { // OPTIONS
            //                        PAUSED = !PAUSED;
            //                        break;
            //                    }
            //                    case 4: { // SHARE
            //                        //world->darkness_active(
            //                        world->darkness_active() ); break;
            //                    }
            //                    case 5: { // HOME
            //                        //world->player()->god(
            //                        !world->player()->god(), 666 ); break;
            //                    }
            //                    case 15: { // TOUCH PAD
            //                        break;
            //                    }
            //                }
            //                break;
            //            }
            //            case SDL_JOYAXISMOTION: {
            //                // L2 and R2
            //                if ( ( event.jaxis.axis == 4 ) || (
            //                event.jaxis.axis == 5 ) ) {
            //                    if ( event.jaxis.value > 1200 ) {
            //                        if ( event.jaxis.axis == 4 ) { // L2?
            //                            //if ( !paused() )
            //                            world->player()->melee( );
            //                        } else if ( event.jaxis.axis == 5 ) { //
            //                        R2?
            //                            //if ( !paused() )
            //                            world->player()->fire_hook( );
            //                        }
            //                    } else {
            //
            //                    }
            //                } else {
            //                    // left joystick
            //                    if ( ( event.jaxis.axis == 0 ) || (
            //                    event.jaxis.axis == 1 ) ) {
            //
            //                        if ( event.jaxis.axis == 0 ) {
            //                            if ( event.jaxis.value > 6400 ) {
            //                                //if ( !PAUSED )
            //                                world->player()->moving_right(
            //                                true );
            //                            } else if ( event.jaxis.value < -6400
            //                            ) {
            //                                //if ( !PAUSED )
            //                                world->player()->moving_left( true
            //                                );
            //                            } else {
            //                                //if ( !PAUSED )
            //                                world->player()->moving_right(
            //                                false );
            //                                //if ( !PAUSED )
            //                                world->player()->moving_left(
            //                                false );
            //                            }
            //                        } else if ( event.jaxis.axis == 1 ) {
            //                            if ( event.jaxis.value > 6400 ) {
            //                                //if ( !PAUSED )
            //                                world->player()->looking_down(
            //                                true );
            //                            } else if ( event.jaxis.value < -6400
            //                            ) {
            //                                //if ( !PAUSED )
            //                                world->player()->looking_up( true
            //                                );
            //                            } else {
            //                                //if ( !PAUSED )
            //                                world->player()->looking_down(
            //                                false );
            //                                //if ( !PAUSED )
            //                                world->player()->looking_up( false
            //                                );
            //                            }
            //                        }
            //                    }
            //
            //                    // right joystick
            //                    if ( ( event.jaxis.axis == 2 ) || (
            //                    event.jaxis.axis == 3 ) ) {
            //                        static int x_axis = 0;
            //                        static int y_axis = 0;
            //                        if ( abs( event.jaxis.value ) > 3200 ) {
            //                            if ( event.jaxis.axis == 2 ) x_axis =
            //                            event.jaxis.value; if (
            //                            event.jaxis.axis == 3 ) y_axis =
            //                            -event.jaxis.value;
            //                        }
            //                        //if ( !paused() ) world->player()->aim(
            //                        Angle( (float)x_axis, (float)y_axis ) );
            //                    }
            //                } break;
            //            }

        // exit
        case SDL_WINDOWEVENT: {
            switch( event.window.event ) {
            case SDL_WINDOWEVENT_CLOSE: {
                quit( );
                break;
            }
            }
            break;
        }
        case SDL_QUIT: {
            quit( );
            break;
        }
        }
    }
}

} // namespace axn
