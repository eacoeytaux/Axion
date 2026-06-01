#include "axn/world/World.hpp"

#include "axn/world/Player.hpp"
#include "axn/world/Terrain.hpp"
#include "axn/graphics/Camera.hpp"
#include "axn/graphics/Lighting.hpp"

var_const( uint START_AGE = ( 999 ) ); // TODO have objects individually validate when they're ready?

#if defined( AXN_DEBUG )
var_const( uint END_AGE = ( 9999999 ) );
#endif

var_const( dec CAMERA_ZOOM_RATIO = ( 0.96875 ) );

#if defined( AXN_DEBUG )
var_const( dec CAMERA_SIDE_BUFFER_RATIO = ( 0.1 ) );
#else
var_const( dec CAMERA_SIDE_BUFFER_RATIO = ( 0.1 ) );
#endif

World::World( )
{
    destroy( ); // from destruction comes creation
}

World::~World( )
{
    destroy( ); // from destruction comes destruction
}

void World::destroy( )
{
    safe_delete( m_rooms );

    m_current_room = nullptr;
    m_next_room = nullptr;

    safe_delete( m_camera );
}

void World::reset( )
{
    dec zoom = !is_null( camera( ) ) ? camera( )->zoom( ) : 1.0;

    destroy( );
    init( );

    camera( )->zoom( zoom );
}

void World::init( )
{
    m_age = 0;

    // todo this should probably be handled by the camera
    uint buffer = min( ( Engine::screen_width( ) * CAMERA_SIDE_BUFFER_RATIO ), ( Engine::screen_height( ) * CAMERA_SIDE_BUFFER_RATIO ) );
    m_camera = new Camera( this, Engine::screen_width( ) - buffer, Engine::screen_height( ) - buffer );

    Random::reset( );

    create( );

    #if !defined( AXN_DEBUG )
    age( START_AGE );
    #endif
}

void World::pause( bool in_paused )
{
    m_paused = in_paused;

    if( paused( ) )
    {
        for_each( player, current_room( )->players( ) )
        {
            player->clear_input( );
        }
    }
}

bool World::paused( ) const
{
    return( m_paused || ( is_null( camera( ) ) || ( camera( )->transitioning( ) || camera( )->transitioned( ) ) ) );
}

void World::render( )
{
    camera( )->clear_subjects( );

    current_room( )->render( );

    camera( )->render( );
}

void World::update( )
{
    if( current_room( ) )
    {
        bool swapping = next_room( ) && ( next_room( ) != current_room( ) );

        if( swapping )
        {
            if( camera( )->transitioned( ) )
            {
                current_room_swap( next_room( ) );

                camera( )->transition( Camera::CIRCLE, camera( )->target( true ) );
            }
            else if( !camera( )->transitioning( ) )
            {
                camera( )->transition( Camera::CIRCLE, camera( )->target( true ) );
            }
        }

        if( !paused( ) )
        {
            current_room( )->update( );

            m_age++;
        }

        if( camera( ) )
        {
            if( current_room( )->players( ).size( ) )
            {
                camera( )->target( current_room( )->player_main( )->position( ) );
            }

            camera( )->update( );
        }
    }

    #if defined( AXN_DEBUG )
    if( age( ) > END_AGE ) { Engine::quit( ); }
    #endif
}

void World::create( ) { }

ptr<Room> World::next_room( ) { return( m_next_room ); }
ptr<Room> World::current_room( ) { return( m_current_room ); }
ptr<Room> World::current_room( ptr<Room> in_room ) { m_next_room = in_room; return( in_room ); }

ptr<Room> World::current_room_swap( ptr<Room> in_room )
{
    m_current_room = in_room;

    camera( )->target( current_room( )->player_main( )->position( ), true );

    return( current_room( in_room ) );
}

ptr<Room> World::add_room( ptr<Room> in_room, bool in_current )
{
    in_room->init( );

    m_rooms.insert( in_room );

    if( in_current || is_null( current_room( ) ) ) { current_room_swap( in_room ); }

    return( in_room );
}

void World::assign_layer_position( ptr<Object> in_object )
{
    in_object->layer_position( queue<uint>( ) );
}

void World::input( list<ptr<Input>> cref in_inputs )
{
    for_each( input, in_inputs )
    {
        if( cast_as( key_input, KeyInput, input ) )
        {
            KeyInput::Key key = key_input->key;
            KeyInput::Dynamic dynamic = key_input->dynamic;

            bool pressed = dynamic == KeyInput::PRESSED;
            bool released = dynamic == KeyInput::RELEASED;
            bool held = dynamic == KeyInput::HELD;
            bool down = pressed || held;

            if( pressed )
            {
                // Log( INFO_LOG, "pressed key ............ ( %c ) [ %i ]", char( key ), int( key ) );
            }

            switch( key )
            {
                no_default;

                b_case( 27 ) : // esc
                {
                    if( pressed )
                    {
                        Engine::quit( );
                        return;
                    }
                }

                b_case( '\\' ) :
                {
                    if( down )
                    {
                        Engine::sync_controllers( );
                    }
                };

                b_case( 'p' ) :
                {
                    if( pressed )
                    {
                        Engine::pause( !Engine::paused( ) );
                    }
                };

                b_case( ';' ) :
                {
                    if( pressed )
                    {
                        if( Settings::shifty( ) ) { Random::seed_milliseconds( ); }

                        reset( );
                    }
                }

                b_case( '.' ) :
                {
                    if( down )
                    {
                        camera( )->zoom( camera( )->zoom( ) / CAMERA_ZOOM_RATIO );
                    }
                }

                b_case( ',' ) :
                {
                    if( down )
                    {
                        camera( )->zoom( camera( )->zoom( ) * CAMERA_ZOOM_RATIO );
                    }
                }

                b_case( '/' ) :
                {
                    if( pressed )
                    {
                        camera( )->zoom( 1.0 );
                    }
                };

                b_case( '=' ) :
                {
                    if( down )
                    {
                        Engine::volume_up( );
                    }
                }

                b_case( '-' ) :
                {
                    if( down )
                    {
                        Engine::volume_down( );
                    }
                }

                b_case( 'm' ) :
                {
                    if( pressed )
                    {
                        Engine::mute( !Engine::muted( ) );
                    }
                }

                b_case( 9 ) : // tab
                {
                    if( pressed )
                    {
                        camera( )->show_hud( !camera( )->show_hud( ) );
                    }
                }

                b_case( -27 ) : a_case( -30 ) : a_case( -31 ) : // shift
                {
                    if( pressed )
                    {
                        Settings::shifty( true );
                    }
                    else
                    {
                        Settings::shifty( false );
                    }
                }

                b_case( -32 ) : // ctrl
                {
                    if( pressed )
                    {
                        Settings::controlling( true );
                    }
                    else
                    {
                        Settings::controlling( false );
                    }
                }

                #if defined( AXN_DEBUG_CONTROLS )

                b_case( '\'' ) :
                {
                    if( pressed )
                    {
                        Engine::step( );
                    }
                };

                b_case( 'c' ) :
                {
                    if( pressed )
                    {
                        static const dec ratio = 0.75; // todo
                        static bool focused = false;

                        camera( )->height( camera( )->height( ) * ( ( focused = !focused ) ? ( ratio ) : inverse( ratio ) ) );
                    }
                };

                #endif

                #if defined( AXN_DEBUG )

                b_case( '`' ) :
                {
                    if( pressed )
                    {
                        Debug::active = !Debug::active;
                    }
                }

                b_case( 'l' ) :
                {
                    if( pressed )
                    {
                        current_room( )->lighting_active( !current_room( )->lighting_active( ) );
                    }
                }

                b_case( 'b' ) :
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::GRAPHICS_GRAYSCALE );
                    }
                }

                b_case( 'z' ) :
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_CAMERA_ZOOM_LOCK );

                        if( Settings::get( Settings::DEBUG_CAMERA_ZOOM_LOCK ) )
                        {
                            camera( )->zoom( camera( )->zoom( ) ); // will reset within limits
                        }
                    }
                }

                b_case( 'o' ) :
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_CAMERA_DISPLAY );
                    }
                }

                b_case( '1' ) :
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_TERRAIN );
                    }
                }

                b_case( '2' ) :
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_PHYSICS );
                    }
                }

                b_case( '3' ) :
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_HEALTH );
                    }
                }

                b_case( '4' ) :
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_DAMAGE );
                    }
                }

                b_case( '5' ) :
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_GRID );
                    }
                }

                b_case( '6' ) :
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_SHOW_VISUAL_BBOX );
                    }
                }

                b_case( '7' ) :
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_SHOW_TERRAIN );
                    }
                }

                b_case( '8' ) :
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_SHOW_FOREGROUND );
                    }
                }

                b_case( '9' ) :
                {
                    if( pressed )
                    {
                        if( Settings::shifty( ) )
                        {
                            if( Settings::get( Settings::DEBUG_SHOW_BACKGROUND ) )
                            {
                                Settings::flip( Settings::DEBUG_SHOW_BACKGROUND_ALL );
                            }
                            else
                            {
                                Settings::flip( Settings::DEBUG_BACKGROUND_COLOR );
                            }
                        }
                        else
                        {
                            Settings::flip( Settings::DEBUG_SHOW_BACKGROUND );
                        }
                    }
                }

                b_case( '0' ) :
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_SHOW_OBJECTS );
                    }
                }

                #endif
            }
        }
        else if( cast_as( mouse_input, MouseInput, input ) )
        {
            Vector movement = mouse_input->movement;

            MouseInput::Dynamic dynamic = mouse_input->dynamic;
            MouseInput::Button button = mouse_input->button;

            if( dynamic == MouseInput::MOVE )
            {
                if( button == MouseInput::NO_BUTTON )
                {
                    camera( )->cursor_world_position( camera( )->screen_to_world( movement ) );
                }
                else if( button == MouseInput::SCROLL_BUTTON )
                {
                    if( is_pos( movement.dy( ) ) )
                    {
                        camera( )->zoom( camera( )->zoom( ) / CAMERA_ZOOM_RATIO );
                    }
                    else if( is_neg( movement.dy( ) ) )
                    {
                        camera( )->zoom( camera( )->zoom( ) * CAMERA_ZOOM_RATIO );
                    }
                }
            }
        }
        else if( cast_as( button_input, ControllerButtonInput, input ) )
        {
            if( button_input->dynamic == ControllerButtonInput::PRESSED )
            {
                switch( button_input->button )
                {
                    no_default;

                    b_case( ControllerButtonInput::START_BUTTON ) :
                    {
                        Engine::pause( !Engine::paused( ) );
                    }

                    #if defined( AXN_DEBUG_CONTROLS )

                    b_case( ControllerButtonInput::START_OPPOSITE_BUTTON ) :
                    {
                        Engine::step( );
                    }

                    #endif
                }
            }
        }
    }

    if( !paused( ) )
    {
        current_room( )->input( in_inputs );
    }
}
