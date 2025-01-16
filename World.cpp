#include "World.hpp"
#include "Camera.hpp"
#include "Player.hpp"
#include "Terrain.hpp"
#include "Lighting.hpp"

namespace
{
cuint START_AGE = 0; // 1024;
cdec CAMERA_ZOOM_RATIO = 0.96875;
#ifdef AXN_DEBUG
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
    destroy( );
    init( );
}

void World::init( )
{
    m_age = 0;

    create( );

    // age world before adding player
    for_range( i, START_AGE )
    {
        update( );
    }
}

void World::create( )
{
    // TODO camera width / height should be independent of screen size
    uint side_buffer = min( Engine::screen_width( ) * CAMERA_SIDE_BUFFER_RATIO, Engine::screen_height( ) * CAMERA_SIDE_BUFFER_RATIO );
    m_camera = new Camera( this, Engine::screen_width( ) - side_buffer, Engine::screen_height( ) - side_buffer );
}

void World::input( const varray<Input *> & _inputs )
{
    for_each( input, _inputs )
    {
        if( KeyInput * key_input = dynamic_cast<KeyInput *>( input ) )
        {
            KeyInput::Key key = key_input->key;
            KeyInput::Dynamic dynamic = key_input->dynamic;

            bool pressed = ( dynamic == KeyInput::PRESSED );
            bool held = ( dynamic == KeyInput::HELD );
            bool down = ( pressed || held );

            if( pressed )
            {
                switch( key )
                {
                    default:
                    {
                        break;
                    }

                    case 27:
                    { // esc key
                        Engine::quit( );
                        return;
                    }

                    case '\\':
                    {
                        Engine::sync_controllers( );
                        break;
                    };

                    case 'p':
                    {
                        Engine::pause( !Engine::paused( ) );
                        break;
                    };

                    case ';':
                    {
                        reset( );
                        break;
                    }

                    case 9:
                    { // tab
                        m_camera->show_hud( !m_camera->show_hud( ) );
                        break;
                    }

                    #ifdef AXN_DEBUG
                    case '\'':
                    {
                        Engine::step( );
                        break;
                    };

                    case 'l':
                    {
                        m_current_room->lighting_active( !m_current_room->lighting_active( ) );
                        break;
                    }

                    case '.':
                    {
                        m_camera->zoom( m_camera->zoom( ) / CAMERA_ZOOM_RATIO );
                        break;
                    }
                    case ',':
                    {
                        m_camera->zoom( m_camera->zoom( ) * CAMERA_ZOOM_RATIO );
                        break;
                    }
                    case '/':
                    {
                        m_camera->zoom( 1.0 );
                        break;
                    };

                    case '=':
                    {
                        Engine::volume_up( );
                        break;
                    }
                    case '-':
                    {
                        Engine::volume_down( );
                        break;
                    }
                    case '0':
                    {
                        Engine::mute( !Engine::muted( ) );
                        break;
                    }

                    case '1':
                    {
                        Mob::draw_health = !Mob::draw_health;
                        break;
                    }

                    case '2':
                    {
                        Object::draw_physics = !Object::draw_physics;
                        break;
                    }

                    case '3':
                    {
                        m_draw_grid = !m_draw_grid;
                        break;
                    }

                    case '4':
                    {
                        m_display_forebackground = !m_display_forebackground;
                        break;
                    }

                    case '8':
                    {
                        m_camera->m_draw_debug = !m_camera->m_draw_debug;
                        break;
                    }

                    case '9':
                    {
                        Engine::anti_alias( !Engine::anti_alias( ) );
                        break;
                    }

                    case '`':
                    {
                        Debug::active = !Debug::active;
                        break;
                    }
                    #endif
                }
            }
        }

        if( MouseInput * mouse_input = dynamic_cast<MouseInput *>( input ) )
        {
            Coordinate position = mouse_input->position;
            MouseInput::Dynamic dynamic = mouse_input->dynamic;
            MouseInput::Button button = mouse_input->button;

            if( dynamic == MouseInput::MOVE )
            {
                if( button == MouseInput::SCROLL_BUTTON )
                {
                    if( is_pos( position.y( ) ) )
                    {
                        m_camera->zoom( m_camera->zoom( ) / CAMERA_ZOOM_RATIO );
                    }
                    else if( is_neg( position.y( ) ) )
                    {
                        m_camera->zoom( m_camera->zoom( ) * CAMERA_ZOOM_RATIO );
                    }
                }
                else
                {
                    m_camera->cursor_world_position( position );
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
                    #ifdef AXN_DEBUG
                    case ControllerButtonInput::START_OPPOSITE_BUTTON:
                    {
                        Engine::step( );
                        break;
                    }
                    #endif
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

uint World::age( ) const
{
    return m_age;
}

Camera * World::camera( )
{
    return m_camera;
}
