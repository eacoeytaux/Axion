#include "World.hpp"
#include "Camera.hpp"
#include "Player.hpp"
#include "Terrain.hpp"
#include "Lighting.hpp"

namespace
{
const uint GRID_BLOCK_SIZE = 256;
const uint START_AGE = 0; // 1024;
const dec CAMERA_ZOOM_RATIO = 0.96875;
} // namespace

World::World( )
{
    m_objects = varray<Object *>( );
    m_object_queue = queue<Object *>( );
    m_foreground_objects = varray<Object *>( );
    m_background_objects = varray<Object *>( );
    destroy( ); // from destruction comes creation
}

World::~World( )
{
    destroy( );
}

void World::create( const FixedRectangle & _bounds )
{
    bounds( _bounds );

    m_terrain = generate_terrain( );
    assign_layer_position( m_terrain );

    m_lighting = new Lighting( );

    // TODO camera width / height should be independent of screen size
    m_camera = new Camera( this, Engine::screen_width( ), Engine::screen_height( ) );
}

void World::destroy( )
{
    clear_objects( );

    safe_delete( m_camera );
    safe_delete( m_lighting );
    safe_delete( m_terrain );
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

    add_player( Coordinate( 0.0, 300.0 ) );
}

void World::reset( )
{
    destroy( );
    init( );
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

            switch( key )
            {
                case 27 :
                { // esc key
                    Engine::quit( );
                    return;
                }
                case ';' :
                {
                    if( pressed )
                    {
                        reset( );
                    }
                    break;
                }
                case 'p' :
                {
                    if( pressed )
                    {
                        Engine::pause( !Engine::paused( ) );
                    }
                    break;
                };
                case 'c' :
                {
                    if( pressed )
                    {
                        Engine::sync_controllers( );
                    }
                    break;
                };
#ifdef AXN_DEBUG
                case '\'' :
                {
                    if( pressed )
                    {
                        Engine::step( );
                    }
                    break;
                };
#endif
                case 'm' :
                {
                    if( pressed )
                    {
                        Engine::mute( !Engine::muted( ) );
                    }
                    break;
                }
                case 'z' :
                {
                    if( pressed )
                    {
                        m_camera->zoom( 1.0 );
                    }
                    break;
                };
                case 'o' :
                {
                    if( pressed )
                    {
                        Engine::anti_alias( !Engine::anti_alias( ) );
                    }
                    break;
                }
                case 'l' :
                {
                    if( pressed )
                    {
                        m_lighting_active = !m_lighting_active;
                    }
                    break;
                }
                case 'h' :
                {
                    if( pressed )
                    {
                        m_camera->show_hud( !m_camera->show_hud( ) );
                    }
                    break;
                }
#ifdef AXN_DEBUG
                case 'b' :
                {
                    if( pressed )
                    {
                        m_display_forebackground = !m_display_forebackground;
                    }
                    break;
                }
#endif
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
                    {
                        m_camera->zoom( m_camera->zoom( ) / CAMERA_ZOOM_RATIO );
                    }
                    break;
                }
                case ',' :
                {
                    if( down )
                    {
                        m_camera->zoom( m_camera->zoom( ) * CAMERA_ZOOM_RATIO );
                    }
                    break;
                }
                case '\\' :
                {
                    if( pressed )
                    {
                        Engine::show_cursor( !Engine::show_cursor( ) );
                    }
                    break;
                }
#ifdef AXN_DEBUG
                case '`' :
                {
                    if( pressed )
                    {
                        Debug::active = !Debug::active;
                    }
                    break;
                }
#endif
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
                    if( is_positive( position.y( ) ) )
                    {
                        m_camera->zoom( m_camera->zoom( ) / CAMERA_ZOOM_RATIO );
                    }
                    else if( is_negative( position.y( ) ) )
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
}

void World::pause( bool paused )
{
    if( paused )
    {
        for_each( player, m_players )
        {
            player->clear_input( );
        }
    }
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

    static ColorSlider rainbow[ 6 ] = {
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
    bounds_drawing.draw( color, bounds( ), BOUNDS_THICKNESS, true, true );
    bounds_drawing.draw( color.a( ( 1.0 - percent_pulse ) * PULSE_ALPHA_START ), bounds( ), BOUNDS_THICKNESS + ( percent_pulse * PULSE_THICKNESS * 2 ), true, true );

    camera->capture( new Visible( bounds_drawing ), true );
}

#ifdef AXN_DEBUG
void World::render_object_grid( Camera * camera, const bool _fill_blocks, function<bool( const Grid::Block & block )> fill_block )
{
    const dec GRID_THICKNESS = 1.0;
    const Color GRID_COLOR = WHITE.a( 0.25 );
    const Color FILLED_BLOCK_COLOR = GREEN.a( 0.25 );

    Grid & grid = object_grid( );
    Coordinate top = bounds( ).top( );
    Coordinate bottom = bounds( ).bottom( );

    Drawing grid_drawing;

    if( _fill_blocks )
    {
        grid.traverse( [ & ]( Grid::Block & block )
                       {
            if( fill_block( block ) )
            {
                grid_drawing.draw( FILLED_BLOCK_COLOR, Polygon( {
                    Coordinate( bottom.x( ) + ( GRID_BLOCK_SIZE * block.x ), bottom.y( ) + ( GRID_BLOCK_SIZE * block.y ) ),
                    Coordinate( min( bottom.x( ) + ( GRID_BLOCK_SIZE * ( block.x + 1 ) ), top.x( ) ), bottom.y( ) + ( GRID_BLOCK_SIZE * block.y ) ),
                    Coordinate( min( bottom.x( ) + ( GRID_BLOCK_SIZE * ( block.x + 1 ) ), top.x( ) ), min( bottom.y( ) + ( GRID_BLOCK_SIZE * ( block.y + 1 ) ), top.y( ) ) ),
                    Coordinate( bottom.x( ) + ( GRID_BLOCK_SIZE * block.x ), min( bottom.y( ) + ( GRID_BLOCK_SIZE * ( block.y + 1 ) ), top.y( ) ) ) } ) );
            } } );
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

    camera->capture_debug( new Visible( grid_drawing ), true );
}
#endif

void World::render( )
{
    Camera * camera = m_camera;
    camera->clear_subjects( );

    m_lighting->clear_light_sources( );
    
#ifdef AXN_DEBUG
    if( m_display_forebackground )
#endif
        for_each( object, m_background_objects )
        {
            object->render_object( );
            
            if( object->visible( ) )
            {
                camera->capture( object );
            }
        }

    m_terrain->render_object( );
    camera->capture( m_terrain );
    
#ifdef AXN_DEBUG
    if( Debug::active )
    {
        if( m_terrain->draw_debug )
        {
            Drawing debug_overlay = m_terrain->debug_overlay( );
            debug_overlay.move( m_terrain->position( ) );
            camera->capture_debug( new Visible( debug_overlay ), true );
        }
        
        render_object_grid( camera, true, []( const Grid::Block & block ) { return block.objects.size( ); } );
    }
#endif

    for_each( object, m_objects )
    {
        for_each( light, object->lights( ) )
        {
            m_lighting->add_light_source( light );
        }

        object->render_object( );

        if( object->visible( ) )
        {
            camera->capture( object );

#ifdef AXN_DEBUG
            if( Debug::active )
            {
                if( object->draw_debug )
                {
                    Drawing debug_overlay = object->debug_overlay( );
                    debug_overlay.move( object->position( ) );
                    
                    camera->capture_debug( new Visible( debug_overlay ), true );
                }
            }
#endif
        }
    }
    
#ifdef AXN_DEBUG
    if( m_display_forebackground )
#endif
    {
        for_each( object, m_foreground_objects )
        {
            object->render_object( );
            
            if( object->visible( ) )
            {
                camera->capture( object );
            }
        }
    }

    render_bounds( camera );

    camera->render( );
}

void World::update( )
{
    ++m_age;

    // update all objects
    static auto object_sort = []( const Object * const & obj1, const Object * const & obj2 )
    {
        if( obj1->z( ) != obj2->z( ) )
        {
            return ( obj1->z( ) < obj2->z( ) );
        }

        if( obj1->layer_position( ) != obj2->layer_position( ) )
        {
            return ( obj1->layer_position( ) < obj2->layer_position( ) );
        }

        if( obj1->age( ) != obj2->age( ) )
        {
            return ( obj1->age( ) > obj2->age( ) );
        }

        return false;
    };

    add_objects_from_queue( );
    m_objects.sort( object_sort, true );
    
    update_objects( m_objects );
    
#ifdef AXN_DEBUG
    if( m_display_forebackground )
#endif
    {
        m_foreground_objects.sort( object_sort, true );
        update_objects( m_foreground_objects );
        
        m_background_objects.sort( object_sort, true );
        update_objects( m_background_objects );
    }

    update_object( m_terrain );

    m_players.erase_if( [ & ]( Player * player )
                        { return player->deleted( ); } );

    auto erase_deleted_objects = [ & ]( varray<Object *> & objects )
    {
        objects.erase_if( [ & ]( Object * object )
                           {
            if( !object )
            {
                return true;
            }
            else if( object->deleted( ) )
            {
                remove_object( object );
                return true;
            }
            else
            {
                return false;
            } } );
    };
    
    erase_deleted_objects( m_objects );
    erase_deleted_objects( m_foreground_objects );
    erase_deleted_objects( m_background_objects );

    if( m_camera )
    {
        if( m_players.size( ) )
        {
            m_camera->target( player_main( )->position( ) );
        }

        m_camera->update( );
    }
}

void World::update_object( Object * object )
{
    if( object )
    {
        object->update_object( );
    }
}

const varray<Object *> & World::objects( ) const
{
    return m_objects;
}

varray<Object *> World::objects_in_range( const FixedRectangle & _range )
{
    uset<Object *> objects_set;

    m_object_grid.traverse( [ & ]( Grid::Block & block )
                            {
        for_each( object, block.objects )
        {
            objects_set.insert( object );
        } } );

    varray<Object *> objects;
    for_each( object, objects_set )
    {
        objects.insert_back( object );
    }

    return objects;
}

void World::add_object( Object * object )
{
    if( object )
    {
        m_object_queue.push( object );
    }
}

void World::remove_object( Object * object )
{
    if( object )
    {
        if( object->z( ) == ONE )
        {
            m_object_grid.erase( object );
        }
        
        safe_delete( object );
    }
}

void World::add_objects_from_queue( )
{
    while( m_object_queue.size( ) )
    {
        Object * object = m_object_queue.front( );

        assign_layer_position( object );
        
        if( object->foreground( ) )
        {
            m_foreground_objects.insert_back( object );
        }
        else if( object->background( ) )
        {
            m_background_objects.insert_back( object );
        }
        else
        {
            m_objects.insert_back( object );
            
            if( object->interactive( ) )
            {
                m_object_grid.add( object );
            }
        }

        m_object_queue.pop( );
    }
}

void World::clear_objects( )
{
    m_object_grid.clear( );

    while( m_object_queue.size( ) )
    {
        safe_delete( m_object_queue.front( ) );
        m_object_queue.pop( );
    }

    for_each( object, m_objects )
    {
        remove_object( object );
    }

    for_each( object, m_foreground_objects )
    {
        remove_object( object );
    }

    for_each( object, m_background_objects )
    {
        remove_object( object );
    }
    
    m_players.clear( );
    m_objects.clear( );
    m_foreground_objects.clear( );
    m_background_objects.clear( );
}

void World::assign_layer_position( Object * object )
{
    object->layer_position( queue<uint>( ) );
}

uint World::age( ) const
{
    return m_age;
}

const FixedRectangle & World::bounds( ) const
{
    return m_bounds;
}

void World::bounds( const FixedRectangle & _bounds )
{
    m_object_grid.init( m_bounds = _bounds );
}

Player * World::add_player( const Coordinate & _position )
{
    Player * new_player = m_players.insert_back( create_player( _position ) );

    m_camera->target( new_player->position( ), true );

    add_object( new_player );

    return new_player;
}

Camera * World::camera( )
{
    return m_camera;
}

Player * World::player( const uint _player_number )
{
    if( m_players.valid_index( _player_number ) )
    {
        return m_players[ _player_number ];
    }
    else
    {
        return nullptr;
    }
}

Player * World::player_main( )
{
    return player( 0 );
}

void World::wind( const Vector & _wind )
{
    m_wind = _wind;
}

const Lighting * World::lighting( ) const
{
    return m_lighting;
}

bool World::lighting_active( ) const
{
    return m_lighting_active;
}

void World::lighting_active( bool active )
{
    m_lighting_active = active;
}

const Terrain * World::terrain( ) const
{
    return m_terrain;
}

Vector World::wind( ) const
{
    return m_wind;
}

void World::Grid::init( const FixedRectangle & _bounds )
{
    m_bounds = _bounds;

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
}

World::Grid::Block & World::Grid::block( const uint _x, const uint _y )
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

uint World::Grid::x( const Planc & _x ) const
{
    Planc x_translated = _x - m_bounds.bottom( ).x( );

    if( x_translated >= ZERO )
    {
        return floor( x_translated / GRID_BLOCK_SIZE );
    }
    else
    {
        return ZERO;
    }
}

uint World::Grid::y( const Planc & _y ) const
{
    Planc y_translated = _y - m_bounds.bottom( ).y( );

    if( y_translated >= ZERO )
    {
        return floor( y_translated / GRID_BLOCK_SIZE );
    }
    else
    {
        return ZERO;
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

void World::Grid::traverse( const FixedRectangle & _range, function<void( World::Grid::Block & )> f )
{
    Span<uint> xx = x_range( _range );
    Span<uint> yy = y_range( _range );

    for_range( x, xx.range( ) + 1 )
    {
        for_range( y, yy.range( ) + 1 )
        {
            f( block( x + xx.min( ), y + yy.min( ) ) );
        }
    }
}

void World::Grid::add( Object * object )
{
    if( object->z( ) == ONE )
    {
        traverse( object->hit_box( ), [ & ]( Grid::Block & block )
                        { block.objects.insert( object ); } );
    }
}

void World::Grid::erase( Object * object )
{
    if( object->z( ) == ONE )
    {
        traverse( object->hit_box( ), [ & ]( Grid::Block & block )
                         { block.objects.erase( object ); } );
    }
}

void World::Grid::clear( )
{
    return traverse( [ & ]( Grid::Block & block )
                     { block.objects.clear( ); } );
}
