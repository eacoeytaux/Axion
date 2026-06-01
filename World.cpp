#include "World.hpp"
#include "Camera.hpp"
#include "Player.hpp"
#include "Terrain.hpp"
#include "Lighting.hpp"

namespace
{

cuint START_AGE = 0; // 1024;
cdec CAMERA_ZOOM_RATIO = 0.96875;
#if defined ( AXN_DEBUG )
cdec CAMERA_SIDE_BUFFER_RATIO = 0.1;
#else
cdec CAMERA_SIDE_BUFFER_RATIO = 0.0;
#endif

} // namespace

World::World( )
{
    destroy( ); // from destruction comes creation
}

World::~World( )
{
    destroy( );
}

void World::destroy( )
{
    safe_delete( m_camera );
}

void World::reset( )
{
    dec zoom = ( !is_null( m_camera ) ? m_camera->zoom( ) : 1.0 );

    destroy( );
    init( );

    m_camera->zoom( zoom );
}

void World::init( )
{
    m_age = 0;

    uint buffer = min( ( Engine::screen_width( ) * CAMERA_SIDE_BUFFER_RATIO ), ( Engine::screen_height( ) * CAMERA_SIDE_BUFFER_RATIO ) );
    m_camera = new Camera( this, Engine::screen_width( ) - buffer, Engine::screen_height( ) - buffer );

    create( );

    // age world before adding player
    for_range( i, START_AGE )
    {
        update( );
    }
}

void World::create( )
{

}

void World::input( const list<Input *> & _inputs )
{
    for_each( input, _inputs )
    {
        if( KeyInput * key_input = dynamic_cast<KeyInput *>( input ) )
        {
            KeyInput::Key key = key_input->key;
            KeyInput::Dynamic dynamic = key_input->dynamic;

            bool pressed = ( dynamic == KeyInput::PRESSED );
            bool released = ( dynamic == KeyInput::RELEASED );
            bool held = ( dynamic == KeyInput::HELD );
            bool down = ( pressed || held );

            if( pressed )
            {
                // Log( INFO_LOG, "pressed key ............ ( %c ) [ %i ]", key, (int)key );
            }
            else if( released )
            {
                // Log( INFO_LOG, "released key ........... ( %c ) [ %i ]", key, (int)key );
            }

            switch( key )
            {
                default:
                {
                    break;
                }

                case 27:
                { // esc key
                    if( pressed )
                    {
                        Engine::quit( );
                        return;
                    }

                    break;
                }

                case '\\':
                {
                    if( down )
                    {
                        Engine::sync_controllers( );
                    }

                    break;
                };

                case 'p':
                {
                    if( pressed )
                    {
                        Engine::pause( !Engine::paused( ) );
                    }

                    break;
                };

                case ';':
                {
                    if( pressed )
                    {
                        reset( );
                    }

                    break;
                }

                case 9:
                { // tab
                    if( pressed )
                    {
                        m_camera->show_hud( !m_camera->show_hud( ) );
                    }

                    break;
                }

                #if defined ( AXN_DEBUG )
                case '`':
                {
                    if( pressed )
                    {
                        Debug::active = !Debug::active;
                    }

                    break;
                }

                case -27: // rshift
                case -31: // lshift
                {
                    if( pressed )
                    {
                        Debug::shifty = true;
                    }
                    else if( released )
                    {
                        Debug::shifty = false;
                    }

                    break;
                }

                case '\'':
                {
                    if( down )
                    {
                        Engine::step( );
                    }

                    break;
                };

                case '.':
                {
                    if( down )
                    {
                        m_camera->zoom( m_camera->zoom( ) / CAMERA_ZOOM_RATIO );
                    }

                    break;
                }
                case ',':
                {
                    if( down )
                    {
                        m_camera->zoom( m_camera->zoom( ) * CAMERA_ZOOM_RATIO );
                    }

                    break;
                }
                case '/':
                {
                    if( pressed )
                    {
                        m_camera->zoom( 1.0 );
                    }

                    break;
                };

                case '=':
                {
                    if( down )
                    {
                        Engine::volume_up( );
                    }

                    break;
                }
                case '-':
                {
                    if( down )
                    {
                        Engine::volume_down( );
                    }

                    break;
                }
                case '0':
                {
                    if( pressed )
                    {
                        Engine::mute( !Engine::muted( ) );
                    }

                    break;
                }

                case 'l':
                {
                    if( pressed )
                    {
                        m_current_room->lighting_active( !m_current_room->lighting_active( ) );
                    }

                    break;
                }

                case 'o':
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_CAMERA );
                    }

                    break;
                }

                case '1':
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_PHYSICS_TERRAIN );
                    }

                    break;
                }

                case '2':
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_PHYSICS );
                    }

                    break;
                }

                case '3':
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_HEALTH );
                    }

                    break;
                }

                case '4':
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_GRID );
                    }

                    break;
                }

                case '8':
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_SHOW_TERRAIN );
                    }

                    break;
                }

                case '9':
                {
                    if( pressed )
                    {
                        Settings::flip( Settings::DEBUG_SHOW_BACKGROUND );
                    }

                    break;
                }

                #endif
            }
        }

        if( MouseInput * mouse_input = dynamic_cast<MouseInput *>( input ) )
        {
            Vector movement = mouse_input->movement;
            MouseInput::Dynamic dynamic = mouse_input->dynamic;
            MouseInput::Button button = mouse_input->button;

            if( dynamic == MouseInput::MOVE )
            {
                if( button == MouseInput::NO_BUTTON )
                {
                    m_camera->cursor_world_position( m_camera->screen_to_world( movement ) );
                }
                else if( button == MouseInput::SCROLL_BUTTON )
                {
                    if( is_pos( movement.dy( ) ) )
                    {
                        m_camera->zoom( m_camera->zoom( ) / CAMERA_ZOOM_RATIO );
                    }
                    else if( is_neg( movement.dy( ) ) )
                    {
                        m_camera->zoom( m_camera->zoom( ) * CAMERA_ZOOM_RATIO );
                    }
                }
            }
        }

        if( ControllerButtonInput * button_input = dynamic_cast<ControllerButtonInput *>( input ) )
        {
            if( button_input->dynamic == ControllerButtonInput::PRESSED )
            {
                switch( button_input->button )
                {
                    case ControllerButtonInput::START_BUTTON:
                    {
                        Engine::pause( !Engine::paused( ) );
                        break;
                    }
                    #if defined ( AXN_DEBUG )
                    case ControllerButtonInput::START_OPPOSITE_BUTTON:
                    {
                        Engine::step( );
                        break;
                    }
                    #endif
                    default:
                    {
                        break;
                    }
                }
            }
        }

        if( ControllerJoystickInput * joystick_input = dynamic_cast<ControllerJoystickInput *>( input ) )
        {
            if( joystick_input->joystick == ControllerJoystickInput::LEFT_JOYSTICK )
            {

            }

            if( joystick_input->joystick == ControllerJoystickInput::RIGHT_JOYSTICK )
            {

            }
        }
    }

    current_room( )->input( _inputs );
}

void World::pause( bool paused )
{
    if( paused )
    {
        for_each( player, current_room( )->players( ) )
        {
            player->clear_input( );
        }
    }
}

void World::render( )
{
    Camera * camera = m_camera;
    camera->clear_subjects( );

    current_room( )->render( );

    camera->render( );
}

void World::update( )
{
    ++m_age;

    if( current_room( ) )
    {
        current_room( )->update( );
    }

    if( m_camera )
    {
        if( current_room( )->players( ).size( ) )
        {
            m_camera->target( current_room( )->player_main( )->position( ) );
        }

        m_camera->update( );
    }
}

void World::assign_layer_position( Object * object )
{
    object->layer_position( queue<uint>( ) );
}
