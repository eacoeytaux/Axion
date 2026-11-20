#include "axn/world/World.hpp"

#include "axn/world/Player.hpp"
#include "axn/world/Door.hpp"
#include "axn/world/Terrain.hpp"
#include "axn/graphics/Camera.hpp"
#include "axn/graphics/Lighting.hpp"

#if defined( AXN_DEBUG )
var_const( uint START_AGE = ( 99 ) ); // TODO have objects individually validate when they're ready?
var_const( uint END_AGE = ( 999999 ) ); // DEBUG only, closes World is ages this much
#else
var_const( uint START_AGE = ( 999 ) ); // TODO have objects individually validate when they're ready?
#endif

#if defined( AXN_DEBUG )
#endif

var_const( dec CAMERA_ZOOM_RATIO = ( 0.96875 ) );

#if defined( AXN_DEBUG )
var_const( dec CAMERA_SIDE_BUFFER_RATIO = ( 0.1 ) );
#else
var_const( dec CAMERA_SIDE_BUFFER_RATIO = ( 0.1 ) );
#endif

World::~World( )
{
    if( m_load_thread.joinable( ) ) { m_load_thread.join( ); } // C! a load still running would be making rooms as they're destroyed

    destroy( ); // from destruction comes destruction
}

World::World( )
{
    destroy( ); // from destruction comes creation
}

void World::init( )
{
    m_age = 0;

    // todo this should probably be handled by the camera
    uint buffer = min( ( Engine::screen_width( ) * CAMERA_SIDE_BUFFER_RATIO ), ( Engine::screen_height( ) * CAMERA_SIDE_BUFFER_RATIO ) );
    m_camera = new Camera( this, Engine::screen_width( ) - buffer, Engine::screen_height( ) - buffer );

    Random::reset( );

    if( loads_in_background( ) )
    {
        start_loading( [ this ] ( ) { load( ); } );
    }
    else
    {
        load( );
    }
}

void World::render( )
{
    if( camera( ) )
    {
        camera( )->clear_subjects( );

        if( current_room( ) )
        {
            current_room( )->render( );
        }

        camera( )->render( );
    }
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
                ptr<Room> room = next_room( );

                m_queued_sounds.clear( ); // C! nothing from the room being left is heard in the next one
                Engine::stop_audio( ); // C! the sounds and the music

                // C! behind the loading screen, the room catches up to the world's age (only the current room has been updating); the
                // C! rooms swap once it's done, in finish_loading( ), so whatever comes through the door arrives in a room that's caught up
                m_switching_room = true;
                start_loading( [ this, room ] ( ) { if( room->age( ) < age( ) ) { catch_up( room ); } } );
                return;
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
            if( !is_empty( current_room( )->players( ) ) )
            {
                camera( )->target( current_room( )->player_main( )->position( ) );
            }

            // C! the camera's own update( ) is called by Engine::run, every frame, paused or not
        }
    }

    #if defined( AXN_DEBUG )
    if( age( ) > END_AGE ) { Engine::quit( ); }
    #endif
}

void World::pause( bool in_paused )
{
    m_paused = in_paused;

    if( paused( ) && current_room( ) ) // C! no room yet, so no players to stop
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

void World::input( list<ptr<Input>> cref in_inputs )
{
    for_each( input, in_inputs )
    {
        if( cast_as( input, key_input, KeyInput ) )
        {
            KeyInput::Key key = key_input->key;
            KeyInput::Dynamic dynamic = key_input->dynamic;

            bool released = dynamic == KeyInput::RELEASED;
            bool pressed = dynamic == KeyInput::PRESSED;
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
                        if( Settings::shifty( ) ) { Random::rseed( ); }

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

                b_case( 'x' ) :
                {
                    if( pressed )
                    {
                        Engine::screenshot( Engine::ImageFormat( Settings::value( Settings::SCREENSHOT_FORMAT ) ), camera( )->bounds( false ) );

                        camera( )->shutter( ).start( );
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
                        camera( )->cinematic( !camera( )->cinematic( ) ); // C! was shrinking the camera's height, which moved its center
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
                    if( pressed && current_room( ) ) // C! no room yet, so no lighting to toggle
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
                        Settings::flip( Settings::DEBUG_SKELETONS );
                    }
                }

                b_case( '6' ) :
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_AUDIO );
                    }
                }

                b_case( '7' ) :
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_GRID );
                    }
                }

                b_case( '8' ) :
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

                b_case( '9' ) :
                {
                    if( pressed )
                    {
                        if( Settings::shifty( ) )
                        {
                            Settings::flip( Settings::DEBUG_SHOW_FOREGROUND );
                        }
                        else
                        {
                            Settings::flip( Settings::DEBUG_SHOW_TERRAIN );
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
        else if( cast_as( input, mouse_input, MouseInput ) )
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
        else if( cast_as( input, button_input, ControllerButtonInput ) )
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
        if( current_room( ) )
        {
            current_room( )->input( in_inputs );
        }
    }
}

void World::update_loading( )
{
    return_if( !loading( ) );

    m_loading_ticks++;

    return_if( !m_loaded.load( ) );

    finish_loading( );
}

void World::age( ticks in_time, bool in_current_only )
{
    varray<ptr<Room>> rooms;
    if( in_current_only ) { if( current_room( ) ) { rooms.insert_back( current_room( ) ); } }
    else { for_each( room, m_rooms ) { rooms.insert_back( room ); } }

    const varray<HiddenPlayer> c_hidden = hide_players( rooms );

    for_range( i, uint( in_time ) )
    {
        for_each( room, rooms ) { room->update( ); }

        m_age++;
    }

    m_queued_sounds.clear( );
    show_players( c_hidden );
    #if defined( AXN_DEBUG )
    m_heard_sounds.clear( );
    #endif
}

ptr<Room> World::next_room( ) { return( m_next_room ); }
ptr<Room> World::current_room( ) { return( m_current_room ); }
ptr<Room> World::current_room( ptr<Room> in_room ) { return( m_next_room = in_room ); }

ptr<Room> World::add_room( ptr<Room> in_room, bool in_current )
{
    in_room->init( );

    m_rooms.insert( in_room );

    if( in_current || is_null( current_room( ) ) ) { current_room_swap( in_room ); }

    return( in_room );
}

void World::pass( varray<ptr<Object>> cref in_objects, ptr<Door> in_door )
{
    return_if( is_null( in_door ) || is_null( in_door->out( ) ) || is_null( in_door->out( )->room( ) ) ); // C! a door that doesn't lead anywhere passes nothing
    return_if( next_room( ) && ( next_room( ) != current_room( ) ) ); // C! already on the way through a door

    m_passing_objects = in_objects;
    m_passing_door = in_door->out( );

    current_room( m_passing_door->room( ) );
}

void World::play_sound( SoundFX cref in_sound, Point cref in_position )
{
    m_queued_sounds.insert_back( QueuedSound{ in_sound, in_position } );

    #if defined( AXN_DEBUG )
    m_heard_sounds.insert_back( HeardSound{ in_position, in_sound.hearing_umbra( ), in_sound.hearing_penumbra( ), Countdown( max( Engine::sound_length( in_sound ), 1.0 ) ) } ); // C! at least a frame, so one that isn't loaded still shows
    #endif
}

void World::play_sound( SoundFX cref in_sound )
{
    Assert( ( in_sound.global( ) ), "a sound played without a position has to be global( )" );

    m_queued_sounds.insert_back( QueuedSound{ in_sound, ORIGIN } );
}

void World::play_queued_sounds( )
{
    #if defined( AXN_DEBUG )
    for_each( heard, m_heard_sounds ) { heard.playing.tick( ); }
    m_heard_sounds.remove_if( [ ] ( HeardSound cref in_heard ) { return( in_heard.playing.completed( ) ); } );
    #endif

    return_if( m_queued_sounds.empty( ) );

    varray<ptr<Listener>> listeners; // C! gathered only when there's something to hear

    if( current_room( ) )
    {
        for_each_const( object, current_room( )->objects( ) )
        {
            if( cast_as( object, listener, Listener ) ) { listeners.insert_back( listener ); }
        }
    }

    for_each_const( queued, m_queued_sounds )
    {
        declmp loudest_volume = 0.0;
        bool heard = false; // C! by an active listener

        for_each_const( listener, listeners )
        {
            continue_if( !listener->active( ) );
            heard = true;

            const Planc c_distance = listener->listening_position( ).distance_to( queued.position );
            loudest_volume = max( loudest_volume, declmp( dec( listener->volume( ) ) * dec( queued.sound.reach( c_distance ) ) ) );
        }

        if( !heard && queued.sound.global( ) ) { loudest_volume = 1.0; } // C! no one to hear it, as when there's no player

        if( is_pos( loudest_volume ) ) { Engine::speaker( ).play( queued.sound, loudest_volume ); }
    }

    m_queued_sounds.clear( );
}

void World::create( ) { }

void World::destroy( )
{
    safe_delete( m_rooms );

    m_current_room = nullptr;
    m_next_room = nullptr;

    m_passing_objects.clear( ); // C! deleted with their rooms
    m_passing_door = nullptr;

    safe_delete( m_camera );
}

void World::reset( )
{
    dec zoom = !is_null( camera( ) ) ? camera( )->zoom( ) : 1.0;

    destroy( );
    init( );

    camera( )->zoom( zoom );
}

void World::start_loading( function<void( )> cref in_load )
{
    if( m_load_thread.joinable( ) ) { m_load_thread.join( ); } // C! a thread object still holding a finished load can't take a new one

    m_loading = true;
    m_loading_ticks = 0.0;
    m_loaded = !in_load;

    if( in_load )
    {
        m_load_thread = thread( [ this, in_load ] ( ) { in_load( ); m_loaded = true; } );
    }
}

ptr<Room> World::current_room_swap( ptr<Room> in_room )
{
    m_current_room = in_room;

    // C! objects going through a door move now, between the camera transitioning out and back in (outside any room's update, so no room's lists are being looped over)
    if( !is_empty( m_passing_objects ) && !is_null( m_passing_door ) && ( m_passing_door->room( ) == in_room ) )
    {
        uset<ptr<Object>> passing;

        for_each( object, m_passing_objects )
        {
            continue_if( is_null( object ) ); // C! nothing to move

            passing.insert( object );

            if( object->room( ) ) { object->room( )->release( object ); }
        }

        // C! following another object's movement only works within one room, so links to objects left behind are cut
        for_each( object, passing )
        {
            uset<ptr<Object>> subscribers = object->movement_subscribers( ); // C! copies, unsubscribing changes these sets
            uset<ptr<Object>> subscriptions = object->movement_subscriptions( );

            for_each( subscriber, subscribers ) { if( !passing.contains( subscriber ) ) { subscriber->unsubscribe_to_movement( object ); } }
            for_each( subscription, subscriptions ) { if( !passing.contains( subscription ) ) { object->unsubscribe_to_movement( subscription ); } }
        }

        in_room->pass( m_passing_objects, m_passing_door );
    }

    m_passing_objects.clear( );
    m_passing_door = nullptr;

    if( camera( ) && ( current_room( ) && current_room( )->player_main( ) ) )
    {
        camera( )->target( current_room( )->player_main( )->position( ), true );
    }

    return( current_room( in_room ) );
}

varray<World::HiddenPlayer> World::hide_players( varray<ptr<Room>> cref in_rooms )
{
    varray<HiddenPlayer> hidden;

    for_each( room, in_rooms )
    {
        for_each( player, room->players( ) )
        {
            hidden.insert_back( HiddenPlayer{ player, player->invincible_always( ) } );
            player->invisible( true );
            player->invincible_always( true );
        }
    }

    return( hidden );
}

void World::show_players( varray<HiddenPlayer> cref in_hidden )
{
    for_each_const( hidden, in_hidden )
    {
        hidden.player->invisible( false );
        hidden.player->invincible_always( hidden.invincible_always );
    }
}

void World::catch_up( ptr<Room> in_room )
{
    const varray<HiddenPlayer> c_hidden = hide_players( { in_room } );
    in_room->age( ticks( age( ) - in_room->age( ) ) );
    show_players( c_hidden );

    m_queued_sounds.clear( );
    #if defined( AXN_DEBUG )
    m_heard_sounds.clear( );
    #endif
}

void World::load( ) { create( ); age( START_AGE ); }

void World::finish_loading( )
{
    if( m_load_thread.joinable( ) ) { m_load_thread.join( ); }

    m_loading = false;

    if( current_room( ) )
    {
        for_each( player, current_room( )->players( ) )
        {
            player->clear_input( ); // C! the loading screen took no input, so a key let go during it would otherwise stay held
        }
    }

    if( m_switching_room && camera( ) )
    {
        m_switching_room = false;
        current_room_swap( next_room( ) ); // C! the room caught up behind the loading screen, so the objects going through the door move in now
        camera( )->transition( Camera::CIRCLE, camera( )->target( true ) );
    }
}
