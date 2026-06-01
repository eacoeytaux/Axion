#include "Room.hpp"
#include "Camera.hpp"
#include "Player.hpp"
#include "Terrain.hpp"
#include "Lighting.hpp"

namespace
{

cuint GRID_BLOCK_SIZE = 256;
cuint START_AGE = 0; // 1024;

} // namespace

Room::Room( World * world )
{
    m_world = world;
    m_objects = varray<Object *>( );
    m_object_queue = queue<Object *>( );
    m_foreground_objects = varray<Object *>( );
    m_background_objects = varray<Object *>( );

    destroy( ); // from destruction comes creation
}

Room::~Room( )
{
    destroy( );
}

void Room::destroy( )
{
    clear_objects( );

    safe_delete( m_lighting );
    safe_delete( m_terrain );
}

void Room::reset( )
{
    destroy( );
    init( );
}

void Room::init( )
{
    m_age = 0;

    create( );

    add_objects_from_queue( );

    // age world before adding player
    for_range( i, START_AGE )
    {
        update( );
    }
}

void Room::create( FixedRectangle cref _bounds )
{
    bounds( _bounds );

    m_doors.clear( );

    m_terrain = generate_terrain( );
    // world( )->assign_layer_position( m_terrain );

    m_lighting = new Lighting( );
}

void Room::input( const list<Input *> & _inputs )
{
    if( !Engine::paused( ) )
    {
        for_each( input, _inputs )
        {
            for_each( player, players( ) )
            {
                player->input( input );
            }
        }
    }
}

void Room::render_bounds( Camera * camera )
{
    cColor BOUNDS_COLOR = RED;
    cPlanc BOUNDS_THICKNESS = 5.0;

    static uint PULSE = 0;
    cuint PULSE_SPAN = 30;
    cuint PULSE_THICKNESS = BOUNDS_THICKNESS;
    cdec PULSE_ALPHA_START = 0.8;

    static uint PULSE_COLOR = 0;
    cuint PULSE_COLOR_SPAN = 71;

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

#if defined ( AXN_DEBUG )
void Room::render_object_grid( Camera * camera ) const
{
    cdec GRID_LINE_THICKNESS = 1.0;
    cColor GRID_LINE_COLOR = WHITE.a( 0.25 );
    cColor HAS_OBJECTS_COLOR = GREEN.a( 0.15 );
    cColor HAS_TERRAIN_COLOR = YELLOW.a( 0.15 );

    Grid cref grid = m_object_grid;
    Coordinate upper = bounds( ).upper( );
    Coordinate lower = bounds( ).lower( );

    Drawing grid_drawing;

    grid.traverse_const( [ & ] ( const Grid::Block & block )
    {
        if( block.objects( ).size( ) || block.terrain_nodes( ).size( ) )
        {
            Polygon grid( {
                Coordinate( lower.x( ) + ( GRID_BLOCK_SIZE * block.x( ) ), lower.y( ) + ( GRID_BLOCK_SIZE * block.y( ) ) ),
                Coordinate( min( lower.x( ) + ( GRID_BLOCK_SIZE * ( block.x( ) + 1 ) ), upper.x( ) ), lower.y( ) + ( GRID_BLOCK_SIZE * block.y( ) ) ),
                Coordinate( min( lower.x( ) + ( GRID_BLOCK_SIZE * ( block.x( ) + 1 ) ), upper.x( ) ), min( lower.y( ) + ( GRID_BLOCK_SIZE * ( block.y( ) + 1 ) ), upper.y( ) ) ),
                Coordinate( lower.x( ) + ( GRID_BLOCK_SIZE * block.x( ) ), min( lower.y( ) + ( GRID_BLOCK_SIZE * ( block.y( ) + 1 ) ), upper.y( ) ) ) } );

            if( block.objects( ).size( ) )
            {
                grid_drawing.draw( HAS_OBJECTS_COLOR, grid );
            }

            if( block.terrain_nodes( ).size( ) )
            {
                grid_drawing.draw( HAS_TERRAIN_COLOR, grid );
            }
        }
    } );

    for_range( x, grid.x_range( ).range( ) - 1 )
    {
        grid_drawing.draw( GRID_LINE_COLOR, Line( Coordinate( lower.x( ) + ( GRID_BLOCK_SIZE * ( x + 1 ) ), lower.y( ) ), Coordinate( lower.x( ) + ( GRID_BLOCK_SIZE * ( x + 1 ) ), upper.y( ) ) ), GRID_LINE_THICKNESS, true );
    }

    for_range( y, grid.y_range( ).range( ) - 1 )
    {
        grid_drawing.draw( GRID_LINE_COLOR, Line( Coordinate( lower.x( ), lower.y( ) + ( GRID_BLOCK_SIZE * ( y + 1 ) ) ), Coordinate( upper.x( ), lower.y( ) + ( GRID_BLOCK_SIZE * ( y + 1 ) ) ) ), GRID_LINE_THICKNESS, true );
    }

    grid_drawing.draw( GRID_LINE_COLOR, bounds( ), GRID_LINE_THICKNESS, true );

    camera->capture_debug( new Visible( grid_drawing ), true );
}
#endif

void Room::render( )
{
    Camera * camera = world( )->camera( );
    camera->clear_subjects( );

    m_lighting->clear_light_sources( );

    auto render_terrain = [ & ] ( )
    {
        #if defined ( AXN_DEBUG )
        if( Debug::active ) { camera->capture_debug( new Visible( m_terrain->debug_overlay( ) ), true ); }
        if( Settings::get( Settings::DEBUG_SHOW_TERRAIN ) )
            #endif
        {
            camera->capture( m_terrain );
        }
    };

    auto render_objects = [ & ] ( varray<Object *> cref objects, bool light_source )
    {
        for_each( object, objects )
        {
            object->render_object( );

            if( !object->z( ) || camera->in_view( object->bounding_box( ) + object->position( ), object->z( ) ) )
            {
                if( light_source ) { m_lighting->add_light_sources( object->light_sources( ) ); }

                camera->capture( object );

                #if defined ( AXN_DEBUG )
                if( Debug::active && object->draw_debug ) { camera->capture_debug( new Visible( object->debug_overlay( ).move( object->position( ) ) ), true ); }
                #endif
            }
        }
    };

    render_terrain( );

    render_objects( m_objects, true );

    #if defined ( AXN_DEBUG )
    if( Settings::get( Settings::DEBUG_SHOW_BACKGROUND ) )
        #endif
    {
        render_objects( m_background_objects, false );
        render_objects( m_foreground_objects, false );
    }

    #if defined ( AXN_DEBUG )
    if( Debug::active && Settings::get( Settings::DEBUG_GRID ) ) { render_object_grid( camera ); }
    #endif

    render_bounds( camera );
}

void Room::update( )
{
    ++m_age;

    m_players.remove_if( [ & ] ( Player * player ) { return ( !player || player->deleted( ) ); } );

    auto remove_deleted_objects = [ & ] ( varray<Object *> & objects )
    {
        objects.remove_if( [ & ] ( Object * object )
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
            }
        } );
    };

    remove_deleted_objects( m_objects );
    remove_deleted_objects( m_foreground_objects );
    remove_deleted_objects( m_background_objects );

    add_objects_from_queue( );

    // update all objects
    static auto object_sort = [ ] ( const Object * cref obj1, const Object * cref obj2 )
    {
        if( obj1->z( ) != obj2->z( ) )
        {
            return ( obj1->z( ) < obj2->z( ) );
        }

        if( obj1->layer_position( ) != obj2->layer_position( ) )
        {
            if( obj1->layer_position( ).size( ) && obj2->layer_position( ).size( ) )
            {
                return ( obj1->layer_position( ).front( ) < obj2->layer_position( ).front( ) );
            }
            else if( obj1->layer_position( ).size( ) )
            {
                return true;
            }
            else if( obj2->layer_position( ).size( ) )
            {
                return false;
            }
        }

        if( obj1->age( ) != obj2->age( ) )
        {
            return ( obj1->age( ) > obj2->age( ) );
        }

        return false;
    };

    m_objects.sort( object_sort, true );

    update_objects( m_objects );

    #if defined ( AXN_DEBUG )
    if( Settings::get( Settings::DEBUG_SHOW_BACKGROUND ) )
        #endif
    {
        m_foreground_objects.sort( object_sort, true );
        update_objects( m_foreground_objects );

        m_background_objects.sort( object_sort, true );
        update_objects( m_background_objects );
    }

    // TODO update_object( m_terrain );
}

void Room::update_object( Object * object )
{
    Assert( !is_null( object ) );
    object->update_object( );
}

const varray<Object *> & Room::objects( ) const
{
    return m_objects;
}

varray<Object *> Room::objects_in_range( FixedRectangle cref _range )
{
    uset<Object *> objects_set;
    varray<Object *> objects;

    m_object_grid.traverse( _range, [ & ] ( Grid::Block & block )
    {
        for_each( object, block.objects( ) )
        {
            if( !objects_set.contains( object ) )
            {
                objects_set.insert( object );
                objects.insert_back( object );
            }
        }
    } );

    return objects;
}

varray<Terrain::Node *> Room::terrain_in_range( FixedRectangle cref _range )
{
    uset<Terrain::Node *> terrain_set;
    varray<Terrain::Node *> terrain;

    m_object_grid.traverse( _range, [ & ] ( Grid::Block & block )
    {
        for_each( node, block.terrain_nodes( ) )
        {
            if( !terrain_set.contains( node ) )
            {
                terrain_set.insert( node );
                terrain.insert_back( node );
            }
        }
    } );

    return terrain;
}

void Room::add_object( Object * object )
{
    if( object )
    {
        m_object_queue.push( object );
    }
}

void Room::remove_object( Object * object )
{
    if( object )
    {
        if( object->z( ) == 1.0 )
        {
            m_object_grid.remove( object );
        }

        safe_delete( object );
    }
}

void Room::add_objects_from_queue( )
{
    while( m_object_queue.size( ) )
    {
        Object * object = m_object_queue.front( );

        world( )->assign_layer_position( object );

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
                m_object_grid.insert( object );
            }
        }

        m_object_queue.pop( );
    }
}

void Room::clear_objects( )
{
    m_object_grid.clear( );

    while( m_object_queue.size( ) )
    {
        // todo safe_delete( m_object_queue.front( ) );
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

uint Room::age( ) const
{
    return m_age;
}

FixedRectangle cref Room::bounds( ) const
{
    return m_bounds;
}

void Room::bounds( FixedRectangle cref _bounds )
{
    m_object_grid.init( m_bounds = _bounds );
}

Player * Room::add_player( Coordinate cref _position )
{
    Player * new_player = m_players.insert_back( create_player( _position ) );

    world( )->camera( )->target( new_player->position( ), true );

    add_object( new_player );

    return new_player;
}

Camera * Room::camera( )
{
    return world( )->camera( );
}

Player * Room::player( cuint _player_number )
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

Player * Room::player_main( )
{
    return player( 0 );
}

oset<Room *> Room::connected_rooms( ) const
{
    oset<Room *> rooms;

    if( doors( ).size( ) )
    {
        queue<Room *> rooms_to_check;

        rooms_to_check.push( doors( ).begin( ).operator*( )->room( ) );

        while( rooms_to_check.size( ) )
        {
            Room * room = rooms_to_check.pop( );

            if( !rooms.contains( room ) )
            {
                rooms.insert( room );

                for_each( door, room->doors( ) )
                {
                    rooms_to_check.push( door->out( )->room( ) );
                }
            }
        }
    }

    return rooms;
}

Vector Room::gravity( Coordinate cref _position ) const
{
    return ( world( )->gravity( ) );
}

dec Room::air_resistance( Coordinate cref _position ) const
{
    return ( world( )->air_resistance( ) );
}

void Room::wind( Vector cref _wind )
{
    m_wind = _wind;
}

const Lighting * Room::lighting( ) const
{
    return m_lighting;
}

bool Room::lighting_active( ) const
{
    return m_lighting_active;
}

void Room::lighting_active( bool active )
{
    m_lighting_active = active;
}

const Terrain * Room::terrain( ) const
{
    return m_terrain;
}

Vector Room::wind( ) const
{
    return m_wind;
}

void Room::Grid::init( FixedRectangle cref _bounds )
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

Room::Grid::Block & Room::Grid::block( cuint _x, cuint _y )
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

const Room::Grid::Block & Room::Grid::block_const( cuint _x, cuint _y ) const
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

uint Room::Grid::x( Planc cref _x ) const
{
    Planc x_translated = _x - m_bounds.lower( ).x( );

    if( x_translated >= 0.0 )
    {
        return floor( x_translated / GRID_BLOCK_SIZE );
    }
    else
    {
        return 0;
    }
}

uint Room::Grid::y( Planc cref _y ) const
{
    Planc y_translated = _y - m_bounds.lower( ).y( );

    if( y_translated >= 0.0 )
    {
        return floor( y_translated / GRID_BLOCK_SIZE );
    }
    else
    {
        return 0;
    }
}

Span<uint> Room::Grid::x_range( FixedRectangle cref _r ) const
{
    return Span<uint>( x( _r.lower( ).x( ) ), x( _r.upper( ).x( ) ) );
}

Span<uint> Room::Grid::y_range( FixedRectangle cref _r ) const
{
    return Span<uint>( y( _r.lower( ).y( ) ), y( _r.upper( ).y( ) ) );
}

void Room::Grid::traverse( FixedRectangle cref _range, function<void( Room::Grid::Block & )> f )
{
    Span<uint> x_span = x_range( _range );
    Span<uint> y_span = y_range( _range );

    for_range( x, x_span.range( ) + 1 )
    {
        for_range( y, y_span.range( ) + 1 )
        {
            f( block( x + x_span.min( ), y + y_span.min( ) ) );
        }
    }
}

void Room::Grid::traverse_const( FixedRectangle cref _range, function<void( const Room::Grid::Block & )> f ) const
{
    Span<uint> x_span = x_range( _range );
    Span<uint> y_span = y_range( _range );

    for_range( x, x_span.range( ) + 1 )
    {
        for_range( y, y_span.range( ) + 1 )
        {
            f( block_const( x + x_span.min( ), y + y_span.min( ) ) );
        }
    }
}

void Room::Grid::insert( Object * object )
{
    if( object->z( ) == 1.0 )
    {
        traverse( object->hitboxes_bounds( ), [ & ] ( Grid::Block & block )
        {
            block.insert( object );
        } );
    }
}

void Room::Grid::remove( Object * object )
{
    if( object->z( ) == 1.0 )
    {
        traverse( object->hitboxes_bounds( ), [ & ] ( Grid::Block & block )
        {
            block.remove( object );
        } );
    }
}

void Room::Grid::insert( Terrain::Node * terrain_node )
{
    traverse( terrain_node->bounding_box( ), [ & ] ( Grid::Block & block )
    {
        block.insert( terrain_node );
    } );
}

void Room::Grid::remove( Terrain::Node * terrain_node )
{
    traverse( terrain_node->bounding_box( ), [ & ] ( Grid::Block & block )
    {
        block.remove( terrain_node );
    } );
}

void Room::Grid::clear( )
{
    return traverse( [ & ] ( Grid::Block & block )
    {
        block.clear( );
    } );
}
