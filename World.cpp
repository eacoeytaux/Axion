#include "World.hpp"

#include "Camera.hpp"
#include "Player.hpp"
#include "Terrain.hpp"
#include "Lighting.hpp"
#include "Background.hpp"
#include "Foreground.hpp"

namespace
{
const uint GRID_BLOCK_SIZE = 105;
const uint START_AGE = 0; // 1024;
const dec CAMERA_ZOOM_RATIO = 0.96875;
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

    bounds( FixedRectangle( 1500.0, 1500.0 ) );

    create( );

    for_range( i, START_AGE ) update( ); // age world before adding player

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

void World::render_bounds( Camera * camera )
{
    const Color BOUNDS_COLOR = RED;
    const Planc BOUNDS_THICKNESS = 5.0;

    static uint PULSE = 0;
    const uint PULSE_SPAN = 30;
    const uint PULSE_THICKNESS = BOUNDS_THICKNESS;
    const dec PULSE_ALPHA_START = 0.8;

    static uint PULSE_COLOR = 0;
    const uint PULSE_COLOR_SPAN = 71;

    dec percent_pulse = (dec)( PULSE = ( PULSE + 1 ) % PULSE_SPAN ) / (dec)( PULSE_SPAN );
    dec percent_color = (dec)( PULSE_COLOR = ( PULSE_COLOR + 1 ) % PULSE_COLOR_SPAN ) / (dec)( PULSE_COLOR_SPAN );

    static ColorSlider rainbow[] = {
        ColorSlider( RED, YELLOW ),
        ColorSlider( YELLOW, GREEN ),
        ColorSlider( GREEN, CYAN ),
        ColorSlider( CYAN, BLUE ),
        ColorSlider( BLUE, MAGENTA ),
        ColorSlider( MAGENTA, RED ) };

    Color color;
    for_range( i, 6 )
    {
        if( percent_color < ( (dec)( i + 1 ) / 6.0 ) )
        {
            color = rainbow[ i ].color_at( ( percent_color - ( (dec)i / 6.0 ) ) * 6.0 );
            break;
        }
    }

    Drawing bounds_drawing;
    bounds_drawing.draw( color.a( ( 1.0 - percent_pulse ) * PULSE_ALPHA_START ), bounds( ), BOUNDS_THICKNESS + ( percent_pulse * PULSE_THICKNESS * 2 ), true );
    bounds_drawing.draw( color.a( 1.0 ), bounds( ), BOUNDS_THICKNESS, true );
    
    camera->capture( Visible( bounds_drawing ) );
}

#ifdef AXN_DEBUG
void World::render_object_grid( Camera * camera, const bool _fill_blocks, std::function<bool(const Grid::Block& block)> fill_block)
{
    const dec GRID_THICKNESS = 1.0;
    const Color GRID_COLOR = WHITE.a( 0.25 );
    const Color FILLED_BLOCK_COLOR = GREEN.a( 0.25 );

    Grid& grid = object_grid( );
    Coordinate top = bounds( ).top( );
    Coordinate bottom = bounds( ).bottom( );

    Drawing grid_drawing;

    if (_fill_blocks)
    {
        for_range(x, grid.x_range().range())
        {
            for_range(y, grid.x_range().range())
            {
                if (fill_block(grid.block(x, y)))
                {
                    grid_drawing.draw(FILLED_BLOCK_COLOR,
                        Polygon({ Coordinate(bottom.x() + (GRID_BLOCK_SIZE * x), bottom.y() + (GRID_BLOCK_SIZE * y)),
                                  Coordinate(min(bottom.x() + (GRID_BLOCK_SIZE * (x + 1)), top.x()), bottom.y() + (GRID_BLOCK_SIZE * y)),
                                  Coordinate(min(bottom.x() + (GRID_BLOCK_SIZE * (x + 1)), top.x()), min(bottom.y() + (GRID_BLOCK_SIZE * (y + 1)), top.y( ))),
                                  Coordinate(bottom.x() + (GRID_BLOCK_SIZE * x), min(bottom.y() + (GRID_BLOCK_SIZE * (y + 1) ), top.y( ) ) ) } ) );
                }
            }
        }
    }

    for_range( x, grid.x_range( ).range( ) - 1 )
    {
        grid_drawing.draw( GRID_COLOR, Line( Coordinate( bottom.x( ) + ( GRID_BLOCK_SIZE * ( x + 1 ) ), bottom.y( ) ), Coordinate( bottom.x( ) + ( GRID_BLOCK_SIZE * ( x + 1 ) ), top.y( ) ) ), GRID_THICKNESS, true );
    }

    for_range( y, grid.y_range( ).range( ) - 1 )
    {
        grid_drawing.draw( GRID_COLOR, Line( Coordinate( bottom.x( ), bottom.y( ) + ( GRID_BLOCK_SIZE * ( y + 1 ) ) ), Coordinate( top.x( ), bottom.y( ) + ( GRID_BLOCK_SIZE * ( y + 1 ) ) ) ), GRID_THICKNESS, true );
    }
    
    grid_drawing.draw( GRID_COLOR, bounds( ), GRID_THICKNESS, true );

    camera->capture( Visible( grid_drawing ) );
}

void World::render_camera_fps( Camera * camera, const bool _show_crosshairs )
{
    const Camera * _camera = active_camera( );
    const Planc _width = _camera->width( );
    const Planc _height = _camera->height( );
    const dec _zoom = _camera->zoom( );

    const Angle DELTA = TAU / (dec)Engine::FPS;
    const Planc LINE_THICKNESS = 1.0;
    const Planc TARGET_RADIUS = 2.0;
    const Planc FPS_RADIUS = 32.0;
    const Color MAIN_COLOR = WHITE;
    const Color TARGET_COLOR = RED;
    const dec COLOR_OPACITY = 1.0;

    static Angle delta;
    delta -= DELTA;

    Vector target_offset = _camera->target( ) - _camera->center( );

    Polygon target_outer = Circle( ( TARGET_RADIUS + LINE_THICKNESS ) / _zoom, target_offset );
    Polygon target_inner = Circle( ( TARGET_RADIUS ) / _zoom, target_offset );
    Polygon target_cover = Circle( ( TARGET_RADIUS ) / _zoom );

    Line fps_line = Line( ORIGIN, Coordinate( 0, FPS_RADIUS / _zoom ) ).rotate( delta );
    Polygon fps_circle = Circle( FPS_RADIUS / _zoom );
    Polygon fps_dot = Circle( TARGET_RADIUS / _zoom );

    Drawing overlay_drawing;

    if(_show_crosshairs )
    {
        overlay_drawing.draw( MAIN_COLOR.a( COLOR_OPACITY ),
                              Line( Coordinate( -half( _width ) / _zoom, 0.0 ),
                                    Coordinate( half( _width ) / _zoom, 0.0 ) ),
                              LINE_THICKNESS, true );
        overlay_drawing.draw( MAIN_COLOR.a( COLOR_OPACITY ),
                              Line( Coordinate( 0.0, -half( _height ) / _zoom ),
                                    Coordinate( 0.0, half( _height ) / _zoom ) ),
                              LINE_THICKNESS, true );
    }
    
    overlay_drawing.draw(MAIN_COLOR, target_outer, FILLED);
    overlay_drawing.draw(TARGET_COLOR, target_inner, FILLED);
    overlay_drawing.draw(MAIN_COLOR, target_cover, FILLED);

    overlay_drawing.draw( MAIN_COLOR, fps_dot, FILLED );
    overlay_drawing.draw( MAIN_COLOR.a( COLOR_OPACITY ), fps_circle, LINE_THICKNESS, true );
    overlay_drawing.draw( MAIN_COLOR.a( COLOR_OPACITY ), fps_line, LINE_THICKNESS, true );

    overlay_drawing.move( _camera->center( ) );

    camera->capture( Visible( overlay_drawing ) );
}
#endif

World & World::render( )
{
    Camera * camera = m_active_camera;
    camera->clear( );

#ifdef AXN_DEBUG
    if( Debug::active )
    {
        // when debugging don't show full darkness
        m_lighting->darkness_intensity( min<dec>( m_lighting->darkness_intensity( ), 0.25 ) );
    }
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
    {
        camera->lighting( m_lighting );
    }
    else
    {
        camera->clear_lighting( );
    }

    render_bounds( camera );

#ifdef AXN_DEBUG
    if( Debug::active )
    {
        // render_grid( camera );
        render_object_grid( camera, true, []( const Grid::Block & block) { return block.m_objects.size( ); } );

        for_each( object, m_objects )
        {
            if( object->draw_debug )
            {
                camera->capture( Visible( object->debug_overlay( ).move( object->position( ) ) ) );
            }
        }

        render_camera_fps( camera );
    }
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
                case 27 :
                { // esc key
                    Engine::quit( );
                    return *this;
                }
                case ';' :
                {
                    if( pressed )
                        reset( );
                    break;
                }
                case 'p' :
                {
                    if( pressed )
                        Engine::pause( !Engine::paused( ) );
                    break;
                };
                case 'c' :
                {
                    if( pressed )
                        Engine::sync_controllers( );
                    break;
                };
#ifdef AXN_DEBUG
                case '\'' :
                {
                    if( pressed )
                        Engine::step( );
                    break;
                };
#endif
                case 'm' :
                {
                    if( pressed )
                        Engine::mute( !Engine::muted( ) );
                    break;
                }
                case 'z' :
                {
                    if( pressed )
                        m_active_camera->zoom( 1.0 );
                    break;
                };
                case 'o' :
                {
                    if( pressed )
                        Engine::anti_alias( !Engine::anti_alias( ) );
                    break;
                }
                case 'l' :
                {
                    if( pressed )
                        m_lighting_active = !m_lighting_active;
                    break;
                }
                case '=' :
                {
                    Engine::volume_up( );
                    break;
                }
                case '-' :
                {
                    Engine::volume_down( );
                    break;
                }
                case '.' :
                {
                    if( down )
                        m_active_camera->zoom( m_active_camera->zoom( ) / CAMERA_ZOOM_RATIO );
                    break;
                }
                case ',' :
                {
                    if( down )
                        m_active_camera->zoom( m_active_camera->zoom( ) * CAMERA_ZOOM_RATIO );
                    break;
                }
                case '\\' :
                {
                    if( pressed )
                        Engine::show_cursor( !Engine::show_cursor( ) );
                    break;
                }
#ifdef AXN_DEBUG
                case '`' :
                {
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

    update_objects( m_objects );

    m_objects.remove_if( [ & ]( Object * object )
                         {
        if( object->deleted( ) )
        {
            remove_object( object );
            return true;
        }
        else
        {
            return false;
        } } );

    if( m_players.size( ) )
    {
        m_active_camera->update( m_players[ 0 ]->position( ) );
    }

    return *this;
}

World & World::update_object( Object * object )
{
    object_grid( ).mark_absent( object );

    object->update_object( );

    object_grid( ).mark_present( object );

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
    // objects.reserve( m_solid_objects.size( ) );
    for_each( object, m_solid_objects )
    {
        if( in_range<Planc>( object->position( ).x( ), _lower_x - half( object->width( ) ), _upper_x + half( object->width( ) ), true ) )
        {
            objects.insert_back( object );
        }
    }
    return objects;
}

World & World::add_object( Object * object )
{
    m_object_queue.push( object );
    return *this;
}

World & World::remove_object( Object * object )
{
    object_grid( ).mark_absent( object );
    return *this;
}

World & World::add_objects_from_queue( )
{
    while( m_object_queue.size( ) )
    {
        Object * object = m_object_queue.front( );
        m_objects.insert_back( object );

        if( object->solid( ) )
        {
            m_solid_objects.insert_back( object );
        }

        object_grid( ).mark_present( object );

        m_object_queue.pop( );
    }
    return *this;
}

World & World::clear_objects( )
{
    for_each( object, m_objects ) safe_delete( object );

    m_objects.clear( );
    m_solid_objects.clear( );

    while( m_object_queue.size( ) )
    {
        delete( m_object_queue.front( ) );
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

const FixedRectangle & World::bounds( ) const
{
    return m_bounds;
}

World & World::bounds( const FixedRectangle & _bounds )
{
    m_object_grid.init( m_bounds = _bounds );
    return *this;
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

World::Grid & World::Grid::init( const FixedRectangle & _bounds )
{
    m_offset = _bounds.bottom( );

    m_grid_x_size = ceil( _bounds.width( ) / (dec)GRID_BLOCK_SIZE );
    m_grid_y_size = ceil( _bounds.height( ) / (dec)GRID_BLOCK_SIZE );

    m_grid.resize( m_grid_x_size, varray<Block>( m_grid_y_size ) );

    for_range( x, m_grid_x_size )
    {
        for_range( y, m_grid_y_size )
        {
            block( x, y ).init( x, y );
        }
    }

    return *this;
}

World::Grid::Block& World::Grid::block( const uint _x, const uint _y )
{
    if( valid_x( _x ) && valid_y( _y ) )
    {
        return m_grid[ _x ][ _y ];
    }
    else
    {
        return m_out_of_bounds_block;
    }
}

const set<Object *> & World::Grid::objects( const uint _x, const uint _y ) const
{
    return m_grid[ _x ][ _y ].m_objects;
}

uint World::Grid::x( const Planc & _x ) const
{
    Planc x_translated = _x - m_offset.x();

    if (x_translated >= 0)
    {
        return floor(x_translated / GRID_BLOCK_SIZE);
    }
    else
    {
        return 0;
    }
}

uint World::Grid::y( const Planc & _y ) const
{
    Planc y_translated = _y - m_offset.y( );

    if ( y_translated >= 0 )
    {
        return floor(y_translated / GRID_BLOCK_SIZE);
    }
    else
    {
        return 0;
    }
}

Span<uint> World::Grid::x_range( const FixedRectangle & _r ) const
{
    return Span<uint>( x( _r.bottom( ).x( ) ), x( _r.top( ).x( ) ) );
}

Span<uint> World::Grid::y_range( const FixedRectangle & _r ) const
{
    return Span<uint>( y( _r.bottom( ).y( ) ), y( _r.top( ).y( ) ) );
}

World::Grid & World::Grid::mark( const bool _present, Object * object )
{
    if (object->z() == 1.0)
    {
        Span<uint> grid_x_range = x_range(object->hit_box());
        Span<uint> grid_y_range = y_range(object->hit_box());

        for_range(x, grid_x_range.range() + 1)
        {
            for_range(y, grid_y_range.range() + 1)
            {
                Block & b = block(x + grid_x_range.min(), y + grid_y_range.min());
                if (_present)
                {
                    b.m_objects.insert(object);
                }
                else
                {
                    b.m_objects.erase(object);
                }
            }
        }
    }

    return *this;
}
