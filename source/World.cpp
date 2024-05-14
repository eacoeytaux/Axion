#include "World.hpp"

#include "Camera.hpp"
#include "Player.hpp"
#include "Terrain.hpp"
#include "Lighting.hpp"
#include "Background.hpp"
#include "Foreground.hpp"

using axn::reality::World;

namespace
{
const uint START_AGE = 0; // 1024;
const double CAMERA_ZOOM_RATIO = 0.96875;
} // namespace

World::World( )
{
    m_objects = list<Object *>( );
    m_solid_objects = list<Object *>( );
    m_object_queue = queue<Object *>( );
    destroy( ); // from destruction comes creation
}

World::~World( ) { destroy( ); }

World & World::create( )
{
    m_lighting = new Lighting( );
    add_object( m_terrain = generate_terrain( ) );
    return *this;
}

World & World::destroy( )
{
    clear_objects( );
    safe_delete( m_lighting );
    safe_delete( m_terrain );
    safe_delete( m_background );
    safe_delete( m_foreground );
    return *this;
}

World & World::init( )
{
    m_age = 0;
    create( );

    for_range( START_AGE ) update( ); // age world before adding player

    add_player( Coordinate( 0.0, 300.0 ) );
    m_active_camera = &m_player_cameras[ 0 ];
    m_active_camera->target( m_players[ 0 ]->position( ), true );
    m_active_camera->m_cursor_world_position = COORDINATE_INFINITY_NEGATIVE;

    return *this;
}

World & World::reset( )
{
    destroy( );
    init( );
    return *this;
}

#ifdef AXN_DEBUG
const Angle DELTA = -( TAU / (double)Engine::FPS );
const Planc LINE_THICKNESS = 1.0;
const Planc TARGET_RADIUS = 2.0;
const Planc FPS_RADIUS = 32.0;
const Color COLOR_MAIN = WHITE;
const Color COLOR_INNER = RED;
const double COLOR_OPACITY = 0.5;

Drawing camera_drawing( const Camera & _camera )
{
    Drawing grid;

    grid.draw( COLOR_MAIN.a( COLOR_OPACITY ), Line( Coordinate( -_camera.width( ).half( ) / _camera.zoom( ), 0.0 ), Coordinate( _camera.width( ).half( ) / _camera.zoom( ), 0.0 ) ), true );
    grid.draw( COLOR_MAIN.a( COLOR_OPACITY ), Line( Coordinate( 0.0, -_camera.height( ).half( ) / _camera.zoom( ) ), Coordinate( 0.0, _camera.height( ).half( ) / _camera.zoom( ) ) ), true );

    Vector target_offset = _camera.target( ) - _camera.center( );

    Shape target_outer = Shape::circle( ( TARGET_RADIUS + LINE_THICKNESS ) / _camera.zoom( ), target_offset );
    Shape target_inner = Shape::circle( ( TARGET_RADIUS ) / _camera.zoom( ), target_offset );

    grid.draw( COLOR_MAIN, target_outer, FILLED, true );
    grid.draw( COLOR_INNER, target_inner, FILLED, true );

    grid.move( _camera.center( ) );

    return grid;
}

Drawing fps_drawing( const Camera & _camera )
{
    Drawing fps;

    Shape fps_circle = Shape::circle( FPS_RADIUS / _camera.zoom( ) );
    Shape fps_dot = Shape::circle( TARGET_RADIUS / _camera.zoom( ) );
    Line fps_line( ORIGIN, Coordinate( 0, FPS_RADIUS / _camera.zoom( ) ) );

    static Angle delta;
    delta += DELTA;

    fps_line.rotate( delta );

    fps.draw( COLOR_MAIN, fps_dot, FILLED, true );
    fps.draw( COLOR_MAIN.a( COLOR_OPACITY ), fps_circle, LINE_THICKNESS, true );
    fps.draw( COLOR_MAIN.a( COLOR_OPACITY ), fps_line, LINE_THICKNESS, true );

    fps.move( _camera.center( ) );

    return fps;
}
#endif

World & World::render( )
{
    Camera * camera = m_active_camera;
    camera->clear( );

#ifdef AXN_DEBUG
    // when debugging don't show full darkness
    if( Debug::active )
        m_lighting->darkness_intensity( min<double>( m_lighting->darkness_intensity( ), 0.25 ) );
#endif

    m_lighting->clear_light_sources( );

    for_each( object, m_objects )
    {
        for_each( light, object->lights( ) ) { m_lighting->add_light_source( light ); }
        if( object->needs_render( ) )
        {
            object->render_object( );
        }
        if( object->visible( ) )
        {
            camera->capture( object );
        }
    }

    if( m_lighting_active && m_lighting )
        camera->lighting( m_lighting );
    else
        camera->clear_lighting( );

#ifdef AXN_DEBUG
    Drawing debug_overlay;
    if( Debug::active )
    {
        for_each( object, m_objects ) if( object->draw_debug )
            debug_overlay.draw( object->debug_overlay( ) );

        debug_overlay.draw( camera_drawing( *camera ) );
        debug_overlay.draw( fps_drawing( *camera ) );
    }

    Visible debug_visible( debug_overlay );
    if( Debug::active )
        camera->capture( &debug_visible );
#endif

    camera->render( );

    return *this;
}

World & World::input( const varray<Input *> & _inputs )
{
    for_each( input, _inputs )
    {
        if( KeyInput * key_input = dynamic_cast<KeyInput *>( input ) )
        {
            KeyInput::KEY key = key_input->key;
            KeyInput::DYNAMIC dynamic = key_input->dynamic;

            bool pressed = ( dynamic == KeyInput::PRESSED );
            bool held = ( dynamic == KeyInput::HELD );
            bool down = ( pressed || held );

            switch( key )
            {
            case 27 : { // esc key
                Engine::quit( );
                return *this;
            }
            case ';' : {
                if( pressed )
                    reset( );
                break;
            }
            case 'p' : {
                if( pressed )
                    Engine::pause( !Engine::paused( ) );
                break;
            };
            case 'c' : {
                if( pressed )
                    Engine::sync_controllers( );
                break;
            };
#ifdef AXN_DEBUG
            case '\'' : {
                if( pressed )
                    Engine::step( );
                break;
            };
#endif
            case 'm' : {
                if( pressed )
                    Engine::mute( !Engine::muted( ) );
                break;
            }
            case 'z' : {
                if( pressed )
                    m_active_camera->zoom( 1.0 );
                break;
            };
            case 'o' : {
                if( pressed )
                    Engine::anti_alias( !Engine::anti_alias( ) );
                break;
            }
            case 'l' : {
                if( pressed )
                    m_lighting_active = !m_lighting_active;
                break;
            }
            case '=' : {
                Engine::volume_up( );
                break;
            }
            case '-' : {
                Engine::volume_down( );
                break;
            }
            case '.' : {
                if( down )
                    m_active_camera->zoom( m_active_camera->zoom( ) / CAMERA_ZOOM_RATIO );
                break;
            }
            case ',' : {
                if( down )
                    m_active_camera->zoom( m_active_camera->zoom( ) * CAMERA_ZOOM_RATIO );
                break;
            }
            case '\\' : {
                if( pressed )
                    Engine::show_cursor( !Engine::show_cursor( ) );
                break;
            }
#ifdef AXN_DEBUG
            case '`' : {
                if( pressed )
                    Debug::active = !Debug::active;
                break;
            }
#endif
            }
        }

        if( MouseInput * mouse_input = dynamic_cast<MouseInput *>( input ) )
        {
            Coordinate position = mouse_input->position;
            MouseInput::DYNAMIC dynamic = mouse_input->dynamic;
            MouseInput::BUTTON button = mouse_input->button;

            if( dynamic == MouseInput::MOVE )
            {
                if( button == MouseInput::SCROLL_BUTTON )
                {
                    if( position.y( ) > 0.0 )
                        m_active_camera->zoom( m_active_camera->zoom( ) / CAMERA_ZOOM_RATIO );
                    else if( position.y( ) < 0.0 )
                        m_active_camera->zoom( m_active_camera->zoom( ) * CAMERA_ZOOM_RATIO );
                }
                else
                {
                    m_active_camera->m_cursor_world_position = position;
                }
            }
        }
    }

    if( !Engine::paused( ) )
    {
        for_each( input, _inputs )
        {
            for_each( player, m_players )
            {
                player->input( input );
            }
        }
    }

    return *this;
}

World & World::pause( bool paused )
{
    if( paused )
    {
        for_each( player, m_players )
        {
            player->clear_input( );
        }
    }
    return *this;
}

World & World::update( )
{
    ++m_age;

    // update all objects
    static auto object_sort = []( const Object * const & obj1, const Object * const & obj2 )
    {
        if( obj1->z( ) != obj2->z( ) )
            return ( obj1->z( ) < obj2->z( ) );
        if( obj1->layer_position( ) != obj2->layer_position( ) )
            return ( obj1->layer_position( ) < obj2->layer_position( ) );
        if( obj1->age( ) != obj2->age( ) )
            return ( obj1->age( ) > obj2->age( ) );
        // if( obj1->position( ).x( ) != obj2->position( ).x( ) )
        //      return ( obj1->position( ).x( ) < obj2->position( ).x( ) );
        // if( obj1->position( ).y( ) != obj2->position( ).y( ) )
        //     return ( obj1->position( ).y( ) < obj2->position( ).y( ) );
        return false;
    };

    add_objects_from_queue( );
    m_objects.sort( object_sort );
    m_solid_objects.sort( object_sort );
    for_each( object, m_objects )
    {
        object->update_object( );
    }
    m_objects.remove_if( []( Object * object )
                         { return object->deleted( ); } );

    if( m_players.size( ) )
    {
        m_active_camera->update( m_players[ 0 ]->position( ) );
    }

    return *this;
}

const list<Object *> & World::objects( ) const
{
    return m_objects;
}

list<Object *> World::objects_in_range( const Planc & _lower_x, const Planc & _upper_x )
{
    list<Object *> objects;
    // objects.reserve(m_objects.size());
    for_each( object, m_objects )
    {
        if( in_range<Planc>( object->position( ).x( ), _lower_x, _upper_x, true ) )
        {
            objects.insert_back( object );
        }
    }
    return objects;
}

list<Object *> World::solid_objects_in_range( const Planc & _lower_x, const Planc & _upper_x )
{
    list<Object *> objects;
    // objects.reserve(m_solid_objects.size());
    for_each( object, m_solid_objects )
    {
        if( in_range<Planc>( object->position( ).x( ), _lower_x - half( object->width( ) ), _upper_x + half( object->width( ) ), true ) )
        {
            objects.insert_back( object );
        }
    }
    return objects;
}

World & World::add_object( Object * _object )
{
    m_object_queue.push( _object );
    return *this;
}

World & World::add_objects_from_queue( )
{
    while( m_object_queue.size( ) )
    {
        Object * object = m_object_queue.front( );
        m_objects.insert_back( object );
        if( object->solid( ) )
            m_solid_objects.insert_back( object );

        m_object_queue.pop( );
    }
    return *this;
}

World & World::clear_objects( )
{
    for_each( object, m_objects ) safe_delete( object );

    m_solid_objects.clear( );
    m_objects.clear( );

    while( m_object_queue.size( ) )
    {
        safe_delete( m_object_queue.front( ) );
        m_object_queue.pop( );
    }

    m_players.clear( );

    m_lighting = nullptr;
    m_terrain = nullptr;
    m_background = nullptr;
    m_foreground = nullptr;

    return *this;
}

uint World::age( ) const
{
    return m_age;
}

World & World::add_player( const Coordinate & _position )
{
    Player * new_player = create_player( _position );
    m_players.insert_back( new_player );

    Camera player_camera = Camera( new_player->position( ), Engine::screen_width( ), Engine::screen_height( ) );
    m_player_cameras.insert_back( player_camera );

    add_object( new_player );

    return *this;
}

const Player * World::player( const uint _player_number )
{
    if( m_players.valid_index( _player_number ) )
        return m_players[ _player_number ];
    else
        return nullptr;
}

const Camera * World::active_camera( ) const
{
    return m_active_camera;
}

Camera * player_camera( uint player_number = 0 );
varray<Camera *> all_player_cameras( );

World & World::background( Background * background )
{
    add_object( m_background = background );
    return *this;
}

World & World::wind( const Vector & _wind )
{
    m_wind = _wind;
    return *this;
}

const Lighting * World::lighting( ) const
{
    return m_lighting;
}

bool World::lighting_active( ) const
{
    return m_lighting_active;
}

World & World::lighting_active( bool active )
{
    m_lighting_active = active;
    return *this;
}

const Terrain * World::terrain( ) const
{
    return m_terrain;
}

Vector World::wind( ) const
{
    return m_wind;
}
