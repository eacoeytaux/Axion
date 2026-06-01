#include "axn/world/Room.hpp"

#include "axn/world/Door.hpp"
#include "axn/world/Player.hpp"
#include "axn/world/Terrain.hpp"
#include "axn/graphics/Camera.hpp"
#include "axn/graphics/Lighting.hpp"

var_const( uint GRID_BLOCK_SIZE = ( 256 ) );

#if defined( AXN_DEBUG )
section( OBJECT_GRID )
{
    var_const( dec GRID_LINE_THICKNESS = ( 1.0 ) );
    var_const( Color HAS_OBJECTS_COLOR = ( GREEN.a( 0.15 ) ) );
    var_const( Color HAS_TERRAIN_COLOR = ( YELLOW.a( 0.15 ) ) );
    var_const( Color GRID_LINE_COLOR = ( Settings::get( Settings::DEBUG_BACKGROUND_COLOR ) ? BLACK : WHITE ) );
}
#endif

Room::Room( ptr<World> in_world )
{
    m_world = in_world;

    m_objects = varray<ptr<Object>>( );
    m_object_queue = queue<ptr<Object>>( );
    m_background_objects = varray<ptr<Object>>( );
    m_foreground_objects = varray<ptr<Object>>( );

    destroy( ); // from destruction comes creation

    fill_color( BLACK );
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
}

void Room::create( FixedRectangle cref in_bounds )
{
    bounds( in_bounds );

    m_doors.clear( );

    m_terrain = generate_terrain( );
    // world( )->assign_layer_position( m_terrain );

    m_lighting = new Lighting( );
}

void Room::input( list<ptr<Input>> cref in_inputs )
{
    if( !Engine::paused( ) )
    {
        for_each( input, in_inputs )
        {
            for_each( object, m_input_subscribers )
            {
                object->input( input );
            }
        }
    }
}

#if defined( AXN_DEBUG )
void Room::render_object_grid( ptr<Camera> camera ) const
{
    Grid cref grid = m_object_grid;
    Point upper = bounds( ).upper( );
    Point lower = bounds( ).lower( );

    Drawing grid_drawing;

    grid.traverse_const( [ & ] ( Grid::Block cref in_block )
    {
        if( in_block.objects( ).size( ) || in_block.terrain_nodes( ).size( ) )
        {
            Polygon grid( {
                Point( lower.x( ) + ( GRID_BLOCK_SIZE * in_block.x( ) ), lower.y( ) + ( GRID_BLOCK_SIZE * in_block.y( ) ) ),
                Point( min( lower.x( ) + ( GRID_BLOCK_SIZE * ( in_block.x( ) + 1 ) ), upper.x( ) ), lower.y( ) + ( GRID_BLOCK_SIZE * in_block.y( ) ) ),
                Point( min( lower.x( ) + ( GRID_BLOCK_SIZE * ( in_block.x( ) + 1 ) ), upper.x( ) ), min( lower.y( ) + ( GRID_BLOCK_SIZE * ( in_block.y( ) + 1 ) ), upper.y( ) ) ),
                Point( lower.x( ) + ( GRID_BLOCK_SIZE * in_block.x( ) ), min( lower.y( ) + ( GRID_BLOCK_SIZE * ( in_block.y( ) + 1 ) ), upper.y( ) ) ) } );

            if( in_block.objects( ).size( ) )
            {
                grid_drawing.draw( HAS_OBJECTS_COLOR, grid );
            }

            if( in_block.terrain_nodes( ).size( ) )
            {
                grid_drawing.draw( HAS_TERRAIN_COLOR, grid );
            }
        }
    } );

    for_range( x, grid.x_range( ).range( ) - 1 )
    {
        grid_drawing.draw( GRID_LINE_COLOR, Line( Point( lower.x( ) + ( GRID_BLOCK_SIZE * ( x + 1 ) ), lower.y( ) ), Point( lower.x( ) + ( GRID_BLOCK_SIZE * ( x + 1 ) ), upper.y( ) ) ), GRID_LINE_THICKNESS, true );
    }

    for_range( y, grid.y_range( ).range( ) - 1 )
    {
        grid_drawing.draw( GRID_LINE_COLOR, Line( Point( lower.x( ), lower.y( ) + ( GRID_BLOCK_SIZE * ( y + 1 ) ) ), Point( upper.x( ), lower.y( ) + ( GRID_BLOCK_SIZE * ( y + 1 ) ) ) ), GRID_LINE_THICKNESS, true );
    }

    grid_drawing.draw( GRID_LINE_COLOR, bounds( ), GRID_LINE_THICKNESS, true );

    camera->capture_debug( new Visible( grid_drawing ), true );
}
#endif

void Room::render( )
{
    ptr<Camera> camera = world( )->camera( );

    camera->clear_subjects( );

    if( m_lighting ) m_lighting->clear_light_sources( );

    lambdaf( render_terrain )( )
    {
        if( m_terrain )
        {
            #if defined( AXN_DEBUG )
            if( Settings::get( Settings::DEBUG_SHOW_TERRAIN ) )
                #endif
                camera->capture( m_terrain );

            #if defined( AXN_DEBUG )
            if( Debug::active ) { camera->capture_debug( new Visible( m_terrain->debug_overlay( ) ), true ); }
            #endif
        }
    };

    lambdaf( render_objects )( varray<ptr<Object>> cref in_objects, bool in_light_source )
    {
        for_each( object, in_objects )
        {
            static dec death_fade = 1.0;
            death_fade -= 0.01;

            object->color_filter( [ ] ( Color ref in_color ) { in_color.a( in_color.a( ) * death_fade ); } );

            object->render_object( );

            bool in_view = !object->z( ) || camera->in_view( object->bounding_box( ) + object->position( ), object->z( ) );

            if( m_lighting && in_light_source )
            {
                m_lighting->add_light_sources( object->light_sources( ) );

                for_each( light_source, object->light_sources( ) )
                {
                    break_if( in_view || ( in_view = camera->in_view( FixedRectangle( x2( x2( light_source.radius( ) ) ), x2( x2( light_source.radius( ) ) ) ) + object->position( ), object->z( ) ) ) );
                }
            }

            if( in_view )
            {
                if( object->background( ) )
                {
                    #if defined( AXN_DEBUG )
                    if( Settings::get( Settings::DEBUG_SHOW_BACKGROUND ) &&
                        ( Settings::get( Settings::DEBUG_SHOW_BACKGROUND_ALL ) || ( object->z( ) != 1.0 ) ) )
                        #endif
                        camera->capture_background( object );
                }
                else if( object->foreground( ) )
                {
                    #if defined( AXN_DEBUG )
                    if( Settings::get( Settings::DEBUG_SHOW_FOREGROUND ) )
                        #endif
                        camera->capture_foreground( object );
                }
                else
                {
                    #if defined( AXN_DEBUG )
                    if( Settings::get( Settings::DEBUG_SHOW_OBJECTS ) )
                        #endif
                        camera->capture( object );

                    #if defined( AXN_DEBUG )
                    if( ( Debug::active ) && ( object->draw_debug ) ) { camera->capture_debug( new Visible( object->debug_overlay( ).move( object->position( ) ) ), true ); }
                    #endif
                }
            }
        }
    };

    render_objects( m_background_objects, false );
    render_terrain( );
    render_objects( m_objects, true );
    render_objects( m_foreground_objects, false );

    #if defined( AXN_DEBUG )
    if( ( Debug::active ) && Settings::get( Settings::DEBUG_GRID ) ) { render_object_grid( camera ); }
    #endif
}

void Room::update( )
{
    m_age++;

    m_players.remove_if( [ & ] ( ptr<Player> in_player ) { return( !in_player || in_player->deleted( ) ); } );

    lambdaf( remove_deleted_objects )( varray<ptr<Object>> ref in_objects )
    {
        in_objects.remove_if( [ & ] ( ptr<Object> in_object )
        {
            if( !in_object )
            {
                return( true );
            }
            else if( in_object->deleted( ) )
            {
                remove_object( in_object );
                return( true );
            }
            else
            {
                return( false );
            }
        } );
    };

    remove_deleted_objects( m_objects );
    remove_deleted_objects( m_background_objects );
    remove_deleted_objects( m_foreground_objects );

    add_objects_from_queue( );

    // update all objects
    lambdaf( object_sort )( ptr<const Object> cref in_obj1, ptr<const Object> cref in_obj2 )
    {
        if( in_obj1->z( ) != in_obj2->z( ) )
        {
            return( in_obj1->z( ) < in_obj2->z( ) );
        }
        else
        {
            if( in_obj1->layer_position( ) != in_obj2->layer_position( ) )
            {
                if( in_obj1->layer_position( ).size( ) && in_obj2->layer_position( ).size( ) )
                {
                    return( in_obj1->layer_position( ).front( ) < in_obj2->layer_position( ).front( ) );
                }
                else if( in_obj1->layer_position( ).size( ) )
                {
                    return( true );
                }
                else if( in_obj2->layer_position( ).size( ) )
                {
                    return( false );
                }
            }

            if( in_obj1->age( ) != in_obj2->age( ) )
            {
                return( in_obj1->age( ) > in_obj2->age( ) );
            }
        }

        return( false );
    };

    #if defined( AXN_DEBUG )
    if( Settings::get( Settings::DEBUG_SHOW_BACKGROUND ) )
        #endif
    {
        m_background_objects.sort( object_sort, true );
        update_objects( m_background_objects );
    }

    m_objects.sort( object_sort, true );
    update_objects( m_objects );

    #if defined( AXN_DEBUG )
    if( Settings::get( Settings::DEBUG_SHOW_FOREGROUND ) )
        #endif
    {
        m_foreground_objects.sort( object_sort, true );
        update_objects( m_foreground_objects );
    }

    // TODO update_object( m_terrain );
}

void Room::update_object( ptr<Object> in_object )
{
    Assert( !is_null( in_object ) );

    in_object->update_object( );
}

varray<ptr<Object>> cref Room::objects( ) const
{
    return( m_objects );
}

varray<ptr<Object>> Room::objects_in_range( FixedRectangle cref in_range )
{
    uset<ptr<Object>> objects_set;
    varray<ptr<Object>> objects;

    m_object_grid.traverse( in_range, [ & ] ( Grid::Block ref block )
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

    return( objects );
}

varray<ptr<Terrain::Node>> Room::terrain_in_range( FixedRectangle cref in_range )
{
    uset<ptr<Terrain::Node>> terrain_set;
    varray<ptr<Terrain::Node>> terrain;

    m_object_grid.traverse( in_range, [ & ] ( Grid::Block ref block )
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

    return( terrain );
}

ptr<Object> Room::add_object( ptr<Object> in_object )
{
    if( in_object )
    {
        m_object_queue.push( in_object );
    }

    return( in_object );
}

void Room::remove_object( ptr<Object> in_object )
{
    if( in_object )
    {
        if( is_one( in_object->z( ) ) )
        {
            m_object_grid.remove( in_object );
        }

        if( m_input_subscribers.contains( in_object ) )
        {
            m_input_subscribers.remove( in_object );
        }

        safe_delete( in_object );
    }
}

void Room::add_objects_from_queue( )
{
    while( m_object_queue.size( ) )
    {
        ptr<Object> object = m_object_queue.front( );

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

    safe_delete( m_object_queue );

    for_each( object, m_objects ) { remove_object( object ); }
    for_each( object, m_foreground_objects ) { remove_object( object ); }
    for_each( object, m_background_objects ) { remove_object( object ); }

    m_players.clear( );
    m_objects.clear( );
    m_foreground_objects.clear( );
    m_background_objects.clear( );
    m_input_subscribers.clear( );
}

FixedRectangle Room::bounds( dec in_z ) const
{
    FixedRectangle bounds = m_bounds;

    bounds.width( bounds.width( ) / in_z );
    bounds.height( bounds.height( ) / in_z );

    return( bounds );
}

void Room::bounds( FixedRectangle cref in_bounds )
{
    m_object_grid.init( m_bounds = in_bounds );
}

ptr<Player> Room::add_player( Point cref in_position )
{
    ptr<Player> new_player = m_players.insert_back( create_player( in_position ) );

    world( )->camera( )->target( new_player->position( ), true );

    add_object( new_player );
    subscribe_object_input( new_player );

    return( new_player );
}

ptr<Player> Room::player( uint in_player_number )
{
    if( m_players.valid_index( in_player_number ) )
    {
        return( m_players[ in_player_number ] );
    }
    else
    {
        return( nullptr );
    }
}

ptr<Player> Room::player_main( )
{
    return( player( 0 ) );
}

uset<ptr<Room>> Room::connected_rooms( ) const
{
    uset<ptr<Room>> rooms;

    if( doors( ).size( ) )
    {
        queue<ptr<Room>> rooms_to_check;

        rooms_to_check.push( doors( ).begin( ).operator*( )->room( ) );

        while( rooms_to_check.size( ) )
        {
            ptr<Room> room = rooms_to_check.pop( );

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

    return( rooms );
}

ptr<Door> Room::add_door( ptr<Door> in_door )
{
    m_doors.insert( in_door );

    return( add_object( in_door ) );
}


void Room::subscribe_object_input( ptr<Object> in_object )
{
    m_input_subscribers.insert_back( in_object );
}

Vector Room::gravity( ) const
{
    return( world( )->gravity( ) ); // todo
}

dec Room::air_resistance( ) const
{
    return( world( )->air_resistance( ) ); // todo
}

Vector Room::gravity( Point cref in_position ) const
{
    return( world( )->gravity( ) ); // todo
}

dec Room::air_resistance( Point cref in_position ) const
{
    return( world( )->air_resistance( ) ); // todo
}

void Room::wind( Vector cref in_wind )
{
    m_wind = in_wind;
}

ptr<const Lighting> Room::lighting( ) const
{
    return( m_lighting );
}

bool Room::lighting_active( ) const
{
    return( m_lighting_active );
}

void Room::lighting_active( bool in_active )
{
    m_lighting_active = in_active;
}

ptr<const Terrain> Room::terrain( ) const
{
    return( m_terrain );
}

Vector Room::wind( ) const
{
    return( m_wind );
}

void Room::Grid::init( FixedRectangle cref in_bounds )
{
    m_bounds = in_bounds;

    m_grid_x_size = ceil( in_bounds.width( ) / dec( GRID_BLOCK_SIZE ) );
    m_grid_y_size = ceil( in_bounds.height( ) / dec( GRID_BLOCK_SIZE ) );

    m_grid.resize( m_grid_x_size, varray<Block>( m_grid_y_size ) );

    for_range( x, m_grid_x_size )
    {
        for_range( y, m_grid_y_size )
        {
            block( x, y ).init( x, y );
        }
    }
}

Room::Grid::Block ref Room::Grid::block( uint in_x, uint in_y )
{
    if( valid_x( in_x ) && valid_y( in_y ) )
    {
        return( m_grid[ in_x ][ in_y ] );
    }
    else
    {
        return( m_out_of_bounds_block );
    }
}

const Room::Grid::Block ref Room::Grid::block_const( uint in_x, uint in_y ) const
{
    if( valid_x( in_x ) && valid_y( in_y ) )
    {
        return( m_grid[ in_x ][ in_y ] );
    }
    else
    {
        return( m_out_of_bounds_block );
    }
}

uint Room::Grid::x( Planc cref in_x ) const
{
    Planc x_translated = in_x - m_bounds.lower( ).x( );

    if( x_translated >= 0.0 )
    {
        return( floor( x_translated / GRID_BLOCK_SIZE ) );
    }
    else
    {
        return( 0 );
    }
}

uint Room::Grid::y( Planc cref in_y ) const
{
    Planc y_translated = in_y - m_bounds.lower( ).y( );

    if( y_translated >= 0.0 )
    {
        return( floor( y_translated / GRID_BLOCK_SIZE ) );
    }
    else
    {
        return( 0 );
    }
}

Span<uint> Room::Grid::x_range( FixedRectangle cref in_r ) const
{
    return( Span<uint>( x( in_r.lower( ).x( ) ), x( in_r.upper( ).x( ) ) ) );
}

Span<uint> Room::Grid::y_range( FixedRectangle cref in_r ) const
{
    return( Span<uint>( y( in_r.lower( ).y( ) ), y( in_r.upper( ).y( ) ) ) );
}

void Room::Grid::traverse( FixedRectangle cref in_range, function<void( Room::Grid::Block ref in_block )> cref in_func )
{
    Span<uint> x_span = x_range( in_range );
    Span<uint> y_span = y_range( in_range );

    for_range( x, x_span.range( ) + 1 )
    {
        for_range( y, y_span.range( ) + 1 )
        {
            in_func( block( x + x_span.min( ), y + y_span.min( ) ) );
        }
    }
}

void Room::Grid::traverse_const( FixedRectangle cref in_range, function<void( Room::Grid::Block cref in_block )> cref in_func ) const
{
    Span<uint> x_span = x_range( in_range );
    Span<uint> y_span = y_range( in_range );

    for_range( x, x_span.range( ) + 1 )
    {
        for_range( y, y_span.range( ) + 1 )
        {
            in_func( block_const( x + x_span.min( ), y + y_span.min( ) ) );
        }
    }
}

void Room::Grid::insert( ptr<Object> in_object )
{
    if( is_one( in_object->z( ) ) )
    {
        traverse( in_object->hitboxes_bounds( ), [ & ] ( Grid::Block ref in_block )
        {
            in_block.insert( in_object );
        } );
    }
}

void Room::Grid::remove( ptr<Object> in_object )
{
    if( is_one( in_object->z( ) ) )
    {
        traverse( in_object->hitboxes_bounds( ), [ & ] ( Grid::Block ref in_block )
        {
            in_block.remove( in_object );
        } );
    }
}

void Room::Grid::insert_terrain( ptr<Terrain::Node> in_terrain_node )
{
    traverse( in_terrain_node->bounding_box( ), [ & ] ( Grid::Block ref in_block )
    {
        in_block.insert_terrain( in_terrain_node );
    } );
}

void Room::Grid::remove_terrain( ptr<Terrain::Node> in_terrain_node )
{
    traverse( in_terrain_node->bounding_box( ), [ & ] ( Grid::Block ref in_block )
    {
        in_block.remove_terrain( in_terrain_node );
    } );
}

void Room::Grid::clear( )
{
    traverse( [ & ] ( Grid::Block ref in_block )
    {
        in_block.clear( );
    } );
}
