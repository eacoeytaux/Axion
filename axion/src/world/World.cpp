#include "World.hpp"

#include "Player.hpp"

#include "Sky.hpp"
#include "Fire.hpp"
#include "Tree.hpp"
#include "Bird.hpp"
#include "Fox.hpp"
#include "Waterfall.hpp"

AXN_NAMESPACES
using axn::reality::World;

const uint START_AGE = 512;
const ufloat CAMERA_ZOOM_RATIO = 0.96875;

World::World( ) {
    m_objects = list<Object*>( );
    m_object_queue = queue<Object*>( );
    destroy( ); // from destruction comes creation
}

World::~World( ) { destroy( ); }

World & World::create( ) {
    m_lighting = new Lighting( );

    m_wind = Vector( (planc)-1.5f, (planc)0.f );

    m_background = new Sky( this );
    add_object( m_background );

    m_terrain = new Terrain( this );

    for_each( terrain_edge, m_terrain->edges( ) ) {
        int tree_count = Random::rng_int( 0, 1 );
        for_range( tree_count ) {
            Coordinate root = terrain_edge->vertex1( )->position( ) +
                              ( Vector( terrain_edge->vertex1( )->position( ),
                                        terrain_edge->vertex2( )->position( ) ) *
                                Random::rng_float( 1 ) );
            add_object( new Tree( this, root ) );
        }
    }
    
    add_object( m_terrain );

    add_object( new Fire( this, Coordinate( 200.f, 100.f ) ) );
    add_object( new Waterfall( this, Coordinate( 0.f, 0.f ), METER * 4.f, METER * 8.f, 0.95f ) );

    // add_object(new Bird(this, Coordinate(-100.f, 100.f)));
    add_object( new Fox( this, Coordinate( 200.f, 100.f ) ) );

    return *this;
}

World & World::destroy( ) {
    clear_objects( );
    delete m_lighting;
    m_lighting = nullptr;
    delete m_terrain;
    m_terrain = nullptr;
    delete m_background;
    m_background = nullptr;
    delete m_foreground;
    m_foreground = nullptr;
    return *this;
}

World & World::init( ) {
    m_age = 0;
    create( );
    
    for_range( START_AGE ) update( ); // age world before adding player
    
    add_player( Coordinate( 0.f, 300.f ));
    m_active_camera = &m_player_cameras[ 0 ];
    m_active_camera->target( m_players[ 0 ]->position( ), true );
    m_active_camera->zoom( 1.f );
    
    return *this;
}

World & World::reset( ) {
    destroy( );
    init( );
    return *this;
}

#ifdef AXN_DEBUG
const Angle DELTA = -PI_2 / Engine::FPS;
const planc LINE_THICKNESS = 1.f;
const planc TARGET_RADIUS = 2.f;
const planc FPS_RADIUS = 32.f;
const Color COLOR_MAIN = WHITE;
const Color COLOR_INNER = RED;
const float COLOR_OPACITY = 0.5f;

Drawing camera_drawing( const Camera & _camera ) {
    Drawing grid;

    grid.add( COLOR_MAIN.a( COLOR_OPACITY ),
              Line( Coordinate( -_camera.width( ).half( ), 0.f ),
                    Coordinate( _camera.width( ).half( ), 0.f ) ),
              true );
    grid.add( COLOR_MAIN.a( COLOR_OPACITY ),
              Line( Coordinate( 0.f, -_camera.height( ).half( ) ),
                    Coordinate( 0.f, _camera.height( ).half( ) ) ),
              true );

    Vector target_offset = _camera.target( ) - _camera.center( );

    Shape target_outer = Shape::circle( TARGET_RADIUS + LINE_THICKNESS, target_offset );
    Shape target_inner = Shape::circle( TARGET_RADIUS, target_offset );

    grid.add( COLOR_MAIN, target_outer, FILLED, true );
    grid.add( COLOR_INNER, target_inner, FILLED, true );

    grid.move( _camera.center( ) );

    return grid;
}

Drawing fps_drawing( const Camera & _camera ) {
    Drawing fps;

    static Shape fps_dot = Shape::circle( TARGET_RADIUS );
    static Line fps_line( ORIGIN, Coordinate( 0, FPS_RADIUS ) );
    static Shape fps_circle = Shape::circle( FPS_RADIUS );
    fps_line.rotate( DELTA );

    fps.add( COLOR_MAIN, fps_dot, FILLED, true );
    fps.add( COLOR_MAIN.a( COLOR_OPACITY ), fps_circle, LINE_THICKNESS, true );
    fps.add( COLOR_MAIN.a( COLOR_OPACITY ), fps_line, LINE_THICKNESS, true );

    fps.move( _camera.center( ) );

    return fps;
}
#endif

World & World::render( ) {
    Camera* camera = m_active_camera;
    camera->clear( );

    //    #ifdef AXN_DEBUG
    //    // when debugging don't show full darkness
    //    if (Debug::active)
    //    m_lighting->darkness_intensity(min<float>(m_lighting->darkness_intensity(),
    //    0.25)); #endif

    m_lighting->clear_light_sources( );

    for_each( object, m_objects ) {
        for_each( light, object->lights( ) ) { m_lighting->add_light_source( light ); }
        if( object->needs_render( ) ) {
            object->render( );
        }
        if( object->visible( ) ) {
            camera->capture( object );
        }
    }

    camera->render( );

    m_lighting->render( camera ); // TODO change to camera->add_lighting(m_lighting);
    camera->capture( m_lighting );

    camera->render( );

#ifdef AXN_DEBUG
    Drawing debug_overlay;
    if( Debug::active ) {
        for_each( object, m_objects ) if( object->draw_debug )
            debug_overlay.add( object->debug_overlay( ) );

        debug_overlay.add( camera_drawing( *camera ) );
        debug_overlay.add( fps_drawing( *camera ) );
    }

    Visible debug_visible( debug_overlay );
    if( Debug::active )
        camera->capture( &debug_visible );
#endif

    camera->render( );

    return *this;
}

bool object_sort( const Object* const & obj1, const Object* const & obj2 ) {
    if( obj1->z( ) != obj2->z( ) )
        return ( obj1->z( ) < obj2->z( ) );
    if( obj1->layer_position( ) != obj2->layer_position( ) )
        return ( obj1->layer_position( ) < obj2->layer_position( ) );
    if( obj1->age( ) != obj2->age( ) )
        return ( obj1->age( ) > obj2->age( ) );
    // if (obj1->position().x() != obj2->position().x()) return
    // (obj1->position().x() < obj2->position().x()); if (obj1->position().y()
    // != obj2->position().y()) return (obj1->position().y() <
    // obj2->position().y());
    return false;
}

World & World::input( const varray<Input*> & _inputs ) {
    for_each( input, _inputs ) {
        if( KeyInput* key_input = dynamic_cast<KeyInput*>( input ) ) {
            KeyInput::KEY key = key_input->key;
            KeyInput::DYNAMIC dynamic = key_input->dynamic;

            bool pressed = ( dynamic == KeyInput::PRESSED );
            bool held = ( dynamic == KeyInput::HELD );
            bool down = ( pressed || held );

            switch( key ) {
            case 27: { // esc key
                Engine::quit( );
                return *this;
            }
#ifdef AXN_DEBUG
            case '`': {
                if( pressed )
                    Debug::active = !Debug::active;
                break;
            }
            case ';': {
                if( pressed )
                    reset( );
                break;
            }
            case 'p': {
                if( pressed )
                    Engine::pause( !Engine::paused( ) );
                break;
            };
            case 'm': {
                if( pressed )
                    Engine::mute( !Engine::muted( ) );
                break;
            }
            case '=': {
                Engine::volume_up( );
                break;
            }
            case '-': {
                Engine::volume_down( );
                break;
            }
            case '\'': {
                // darkness_active(!(darkness_active()));
                break;
            }
            case '.': {
                if( down )
                    m_active_camera->zoom( m_active_camera->zoom( ) / CAMERA_ZOOM_RATIO );
                break;
            }
            case ',': {
                if( down )
                    m_active_camera->zoom( m_active_camera->zoom( ) * CAMERA_ZOOM_RATIO );
                break;
            }
            case '\\': {
                if( pressed )
                    Engine::show_cursor( !Engine::show_cursor( ) );
                break;
            }
#endif
            }
        }

        if( MouseInput* mouse_input = dynamic_cast<MouseInput*>( input ) ) {
            Coordinate position = mouse_input->position;
            MouseInput::DYNAMIC dynamic = mouse_input->dynamic;
            // MouseInput::BUTTON button = mouse_input->button;

            if( dynamic == MouseInput::MOVE ) {
                m_active_camera->m_cursor_world_position = position;
            }
        }
    }

    if( !Engine::paused( ) )
        for_each( input, _inputs ) for_each( player, m_players ) player->input( input );

    return *this;
}

World & World::update( ) {
    // update all objects
    add_objects_from_queue( );
    m_objects.sort( object_sort );
    for_each( object, m_objects ) {
        object->pre_update( );
        object->update( );
        object->post_update( );
    }
    m_objects.remove_if( []( Object* object ) { return object->deleted( ); } );

    if( m_players.size( ) ) {
        Coordinate camera_target = m_players[ 0 ]->position( );
        camera_target += Vector( 0.f, m_active_camera->height( ) / 4.f ); // TODO 4 is arb
        m_active_camera->update( camera_target );
    }

    ++m_age;
    return *this;
}

const list<Object*> & World::objects( ) const { return m_objects; }

// const varray<shared_ptr<const Object>> & World::objects() const {
//     return experimental::propagate_const<shared_ptr<const Object>>();
//     return m_objects;
//     return varray<shared_ptr<const Object>>();
// }

list<Object*> World::objects_in_range( const planc _lower_x, const planc _upper_x ) {
    list<Object*> objects;
    // objects.reserve(m_objects.size());
    for_each( object, m_objects ) {
        if( in_range<float>( object->position( ).x( ), _lower_x, _upper_x ) ) {
            objects.push_back( object );
        }
    }
    return objects;
}

World & World::add_object( Object* _object ) {
    m_object_queue.push( _object );
    return *this;
}

World & World::add_objects_from_queue( ) {
    // m_objects.reserve(m_objects.size() + m_object_queue.size());
    // for_each (object, m_object_queue) m_objects.push_back(object);
    while( m_object_queue.size( ) ) {
        m_objects.push_back( m_object_queue.front( ) );
        m_object_queue.pop( );
    }
    return *this;
}

World & World::clear_objects( ) {
    for_each( object, m_objects ) { delete( object ); }
    m_objects.clear( );
    while( m_object_queue.size( ) ) {
        delete m_object_queue.front( );
        m_object_queue.pop( );
    }

    m_players.clear( );

    m_terrain = nullptr;
    m_background = nullptr;
    m_foreground = nullptr;

    return *this;
}

uint World::age( ) const { return m_age; }

World & World::add_player( const Coordinate & _position ) {
    Player* new_player = create_player( _position );
    m_players.push_back( new_player );

    Camera player_camera =
        Camera( new_player->position( ), Engine::screen_width( ), Engine::screen_height( ) );
    m_player_cameras.push_back( player_camera );

    add_object( new_player );

    return *this;
}

const Player* World::player( const uint _player_number ) {
    if( m_players.valid_index( _player_number ) )
        return m_players[ _player_number ];
    else
        return nullptr;
}

const Camera* World::active_camera( ) const { return m_active_camera; }

Camera* player_camera( uint player_number = 0 );
varray<Camera*> all_player_cameras( );

// shared_ptr<const Player> World::player( const uint _player_number) const {
//     if (m_players.valid_index(_player_number)) return make_shared<const
//     Player>(m_players[_player_number]); else return nullptr;
// }

const Terrain* World::terrain( ) const { return m_terrain; }

Vector World::wind( ) const { return m_wind; }
