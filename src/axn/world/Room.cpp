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

section( SOUND_RANGES )
{
    var_const( Planc SOUND_RANGE_THICKNESS = ( 1.0 ) ); // C! a playing sound's umbra and penumbra outlines
    var_const( Color SOUND_UMBRA_COLOR = ( GREEN ) ); // C!
    var_const( Color SOUND_PENUMBRA_COLOR = ( YELLOW ) ); // C! the penumbra's outer edge, drawn only when it has one
    var_const( Color SOUND_COLOR = ( GREEN ) ); // C! the dot where it was played
    var_const( Planc SOUND_DOT_RADIUS = ( 1.5 ) ); // C!
}
#endif

Room::~Room( )
{
    destroy( );
}

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

void Room::init( )
{
    m_age = 0;

    create( );

    add_objects_from_queue( );
}

void Room::render( )
{
    ptr<Camera> camera = world( )->camera( );

    #if defined( AXN_DEBUG )
    // C! every setting the render uses, read once here, so the whole frame is drawn with the same settings
    const bool c_show_terrain = Settings::get( Settings::DEBUG_SHOW_TERRAIN );
    const bool c_show_background = Settings::get( Settings::DEBUG_SHOW_BACKGROUND );
    const bool c_show_background_all = Settings::get( Settings::DEBUG_SHOW_BACKGROUND_ALL );
    const bool c_show_foreground = Settings::get( Settings::DEBUG_SHOW_FOREGROUND );
    const bool c_show_objects = Settings::get( Settings::DEBUG_SHOW_OBJECTS );
    const bool c_show_grid = Settings::get( Settings::DEBUG_GRID );
    #endif

    camera->clear_subjects( );

    if( m_lighting ) { m_lighting->clear_light_sources( ); }

    lambdaf( render_terrain )( )
    {
        if( m_terrain )
        {
            #if defined( AXN_DEBUG )
            if( c_show_terrain )
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
            // C! a light reaches further than the drawing, so an object whose light reaches the view counts as in view
            lambdaf( light_in_view )( )
            {
                return_false_if( !m_lighting || !in_light_source );

                for_each_const( light_source, object->light_sources( ) )
                {
                    return_true_if( camera->in_view( FixedRectangle( x2( x2( light_source.radius( ) ) ), x2( x2( light_source.radius( ) ) ) ) + object->position( ), object->z( ) ) );
                }

                return( false );
            };

            // C! the largest the object's drawing has ever been, at its current position, decides whether it's worth redrawing: one further out of
            // C! view than the camera's render buffer isn't redrawn (every object used to be redrawn first, then culled)
            bool skip_render = ( object->ever_rendered( ) && object->z( ) && !camera->in_render_range( object ) && !light_in_view( ) );

            if( !skip_render )
            {
                object->render_object( );
            }
            #if defined( AXN_DEBUG )
            else
            {
                // C! checks the skip: redrawn anyway, it has to still be out of view, otherwise the camera's render buffer is too small for how much
                // C! this object's drawing changes in a frame (it's drawn this time, so nothing goes missing in debug)
                object->render_object( );

                if( camera->in_view( object->bounding_box( ) + object->position( ), object->z( ) ) )
                {
                    breakpoint;

                    skip_render = false;
                }
            }
            #endif

            bool in_view = ( !skip_render && ( !object->z( ) || camera->in_view( object->bounding_box( ) + object->position( ), object->z( ) ) ) );

            if( m_lighting && in_light_source )
            {
                m_lighting->add_light_sources( object->light_sources( ) );

                in_view = ( in_view || ( !skip_render && light_in_view( ) ) );
            }

            if( in_view )
            {
                if( object->background( ) )
                {
                    #if defined( AXN_DEBUG )
                    if( c_show_background && ( c_show_background_all || ( object->z( ) != 1.0 ) ) )
                        #endif
                        camera->capture_background( object );
                }
                else if( object->foreground( ) )
                {
                    #if defined( AXN_DEBUG )
                    if( c_show_foreground )
                        #endif
                        camera->capture_foreground( object );
                }
                else
                {
                    #if defined( AXN_DEBUG )
                    if( c_show_objects )
                        #endif
                        camera->capture( object );

                    #if defined( AXN_DEBUG )
                    if( ( Debug::active ) && ( object->draw_debug ) ) { camera->capture_debug( new Visible( object->debug_overlay( ).move( object->position( ) ) ), true ); }
                    #endif
                }
            }
        }
    };

    render_terrain( );
    render_objects( m_objects, true );
    render_objects( m_background_objects, false );
    render_objects( m_foreground_objects, false );

    #if defined( AXN_DEBUG )
    if( Debug::active && c_show_grid ) { render_object_grid( camera ); }

    if( Debug::active && Settings::get( Settings::DEBUG_AUDIO ) && world( ) && !is_empty( world( )->heard_sounds( ) ) ) // C! the umbra and penumbra of each sound while it plays
    {
        Drawing sound_ranges;

        for_each_const( heard, world( )->heard_sounds( ) )
        {
            sound_ranges.draw( SOUND_COLOR, Polygon::circle( SOUND_DOT_RADIUS, heard.position ) );
            sound_ranges.draw( SOUND_UMBRA_COLOR, Polygon::circle( heard.umbra, heard.position ), SOUND_RANGE_THICKNESS, true );
            if( is_pos( heard.penumbra ) ) { sound_ranges.draw( SOUND_PENUMBRA_COLOR, Polygon::circle( heard.umbra + heard.penumbra, heard.position ), SOUND_RANGE_THICKNESS, true ); }
        }

        camera->capture_debug( new Visible( sound_ranges ), true );
    }
    #endif
}

void Room::update( )
{
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
            if( in_obj1->layer_order( ) != in_obj2->layer_order( ) )
            {
                bool empty1 = is_empty( in_obj1->layer_order( ) );
                bool empty2 = is_empty( in_obj2->layer_order( ) );

                if( !empty1 && !empty2 )
                {
                    return( in_obj1->layer_order( ).front( ) < in_obj2->layer_order( ).front( ) );
                }
                else if( !empty1 )
                {
                    return( true );
                }
                else if( !empty2 )
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
    // C! TODO Terrain isn't an Object and has no update( ), so this can't be done as written: decide whether terrain should animate (give Terrain an update( )) or drop this

    m_age++;
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

void Room::pass( varray<ptr<Object>> cref in_objects, ptr<Door> in_door )
{
    // C! objects come in at the door they came through, or the first door when that isn't one of this room's
    // C! (the first door used to be read even when there were none)
    ptr<Door> door = in_door;

    if( is_null( door ) || !doors( ).contains( door ) )
    {
        door = ( doors( ).empty( ) ? ptr<Door>( nullptr ) : *doors( ).begin( ) );
    }

    Point entrance = Point( 0.0, 50.0 ); // C! with no door at all, where things used to come out

    // C! how far each point of in_points goes along in_direction: the most, and the middle of the least and the most
    lambdaf( reach )( varray<Point> cref in_pts, Vector cref in_direction, dec ref out_most, dec ref out_middle )
    {
        dec least = 0.0;
        for_range( k, in_pts.size( ) )
        {
            const dec c_along = ( dec( in_pts[ k ].x( ) ) * dec( in_direction.dx( ) ) ) + ( dec( in_pts[ k ].y( ) ) * dec( in_direction.dy( ) ) );
            if( is_zero( k ) || ( c_along > out_most ) ) { out_most = c_along; }
            if( is_zero( k ) || ( c_along < least ) ) { least = c_along; }
        }
        out_middle = half( least + out_most );
    };

    // C! "down" for things coming out (the door's gravity), and across it
    const Vector c_down = VectorA( is_null( door ) ? VectorY( -1.0 ).angle( ) : door->exit_gravity( ), 1.0 );
    const Vector c_across = VectorA( c_down.angle( ) + RIGHT, 1.0 );

    dec door_down = 0.0; dec door_down_middle = 0.0; dec door_across = 0.0; dec door_across_middle = 0.0;
    const bool c_door_has_space = !is_null( door ) && !is_zero( door->space( ).sides( ) );
    if( c_door_has_space )
    {
        reach( door->space( true ).points( ), c_down, door_down, door_down_middle );
        reach( door->space( true ).points( ), c_across, door_across, door_across_middle );
    }
    else if( !is_null( door ) )
    {
        entrance = door->position( );
    }

    for_range( i, in_objects.size( ) )
    {
        ptr<Object> object = in_objects[ i ];

        continue_if( is_null( object ) ); // C! nothing to pass

        const Point c_from = object->position( );

        object->room( this );
        // C! as far "down" (the door's gravity) as the door goes, and centered across it: with gravity down, the bottom of its hitbox on the
        // C! bottom of the door, centered left to right. With no door, or no hitbox on one or the other, at the entrance itself
        if( !c_door_has_space || is_zero( object->space( ).sides( ) ) )
        {
            object->position( entrance );
        }
        else
        {
            dec object_down = 0.0; dec object_down_middle = 0.0; dec object_across = 0.0; dec object_across_middle = 0.0;
            reach( object->space( ).points( ), c_down, object_down, object_down_middle );
            reach( object->space( ).points( ), c_across, object_across, object_across_middle );

            object->position( Point( ( c_down * ( door_down - object_down ) ) + ( c_across * ( door_across_middle - object_across_middle ) ) ) );
        }
        object->move_light_sources( Vector( c_from, object->position( ) ) ); // C! its lights were placed at its last update, back in the other room
        object->find_ground( ); // C! the room doesn't update while the camera transitions in, so it's grounded now if it's resting on a floor
        object->pass_door( door ); // C! once it's where it'll be

        add_object( object );

        if( cast_as( object, player, Player ) )
        {
            m_players.insert_back( player );
            subscribe_object_input( player );
        }
    }

    add_objects_from_queue( ); // C! into the room's lists straight away, so they're drawn while the camera transitions in (the room doesn't update until it's done)
}

void Room::release( ptr<Object> in_object )
{
    return_if( is_null( in_object ) ); // C! nothing to release

    m_object_grid.remove( in_object );

    m_objects.remove( in_object );
    m_foreground_objects.remove( in_object );
    m_background_objects.remove( in_object );
    m_input_subscribers.remove( in_object );

    if( cast_as( in_object, player, Player ) )
    {
        m_players.remove( player );
    }

    // C! one added this frame is still waiting in the queue
    queue<ptr<Object>> waiting;

    while( m_object_queue.size( ) )
    {
        ptr<Object> object = m_object_queue.pop( );

        if( object != in_object ) { waiting.push( object ); }
    }

    m_object_queue = waiting;
}

void Room::bounds( FixedRectangle cref in_bounds )
{
    m_object_grid.init( m_bounds = in_bounds );
}

Vector Room::gravity( ) const
{
    return( world( )->gravity( ) );
}

// C! the same everywhere in the room unless a room overrides this; a room with its own gravity overrides gravity( )
Vector Room::gravity( Point cref in_position ) const
{
    return( gravity( ) );
}

dec Room::air_resistance( ) const
{
    return( world( )->air_resistance( ) );
}

// C! the same everywhere in the room unless a room overrides this; a room with its own air resistance overrides air_resistance( )
dec Room::air_resistance( Point cref in_position ) const
{
    return( air_resistance( ) );
}

ptr<Player> Room::player( uint in_player_number )
{
    return_null_if( !players( ).valid_index( in_player_number ) );

    return( players( )[ in_player_number ] );
}

ptr<Player> Room::player_main( )
{
    return( player( 0 ) );
}

ptr<Player> Room::add_player( Point cref in_position )
{
    ptr<Player> new_player = m_players.insert_back( create_player( in_position ) );

    world( )->camera( )->target( new_player->position( ), true );

    add_object( new_player );
    subscribe_object_input( new_player );

    return( new_player );
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
        if( !is_empty( in_block.objects( ) ) || !is_empty( in_block.terrain_nodes( ) ) )
        {
            Polygon grid( {
                Point( lower.x( ) + ( GRID_BLOCK_SIZE * in_block.x( ) ), lower.y( ) + ( GRID_BLOCK_SIZE * in_block.y( ) ) ),
                Point( min( lower.x( ) + ( GRID_BLOCK_SIZE * ( in_block.x( ) + 1 ) ), upper.x( ) ), lower.y( ) + ( GRID_BLOCK_SIZE * in_block.y( ) ) ),
                Point( min( lower.x( ) + ( GRID_BLOCK_SIZE * ( in_block.x( ) + 1 ) ), upper.x( ) ), min( lower.y( ) + ( GRID_BLOCK_SIZE * ( in_block.y( ) + 1 ) ), upper.y( ) ) ),
                Point( lower.x( ) + ( GRID_BLOCK_SIZE * in_block.x( ) ), min( lower.y( ) + ( GRID_BLOCK_SIZE * ( in_block.y( ) + 1 ) ), upper.y( ) ) ) } );

            if( !is_empty( in_block.objects( ) ) )
            {
                grid_drawing.draw( HAS_OBJECTS_COLOR, grid );
            }

            if( !is_empty( in_block.terrain_nodes( ) ) )
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

ptr<Object> Room::add_object( ptr<Object> in_object )
{
    if( in_object )
    {
        m_object_queue.push( in_object );
    }

    return( in_object );
}

ptr<Object> Room::remove_object( ptr<Object> in_object, bool in_delete )
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

        //m_objects.remove( in_object );

        if( in_delete )
        {
            safe_delete( in_object );
        }
    }

    return( in_object );
}

ptr<Object> Room::update_object( ptr<Object> in_object )
{
    Assert( !is_null( in_object ) );

    in_object->update_object( );

    return( in_object );
}

ptr<Door> Room::add_door( ptr<Door> in_door )
{
    m_doors.insert( in_door );

    return( add_object( in_door ) );
}

uset<ptr<Room>> Room::connected_rooms( ) const
{
    uset<ptr<Room>> rooms;

    if( !is_empty( doors( ) ) )
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
                    continue_if( is_null( door ) || is_null( door->out( ) ) || is_null( door->out( )->room( ) ) ); // C! a door that doesn't lead anywhere yet adds no room

                    rooms_to_check.push( door->out( )->room( ) );
                }
            }
        }
    }

    return( rooms );
}

void Room::subscribe_object_input( ptr<Object> in_object )
{
    m_input_subscribers.insert_back( in_object );
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

void Room::wind( Vector cref in_wind )
{
    m_wind = in_wind;
}

Vector Room::wind( ) const
{
    return( m_wind );
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

void Room::create( FixedRectangle cref in_bounds )
{
    bounds( in_bounds );

    m_doors.clear( );

    m_terrain = generate_terrain( );

    m_lighting = generate_lighting( );

    if( is_null( m_lighting ) )
    {
        m_lighting = new Lighting( ); // C! the default, for rooms that don't make their own
    }
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

void Room::add_objects_from_queue( )
{
    while( m_object_queue.size( ) )
    {
        ptr<Object> object = m_object_queue.front( );

        object->layer_order( world( ) ? world( )->layer_order( object ) : Visible::LayerOrder( ) ); // C! a room with no world (like the tests' rooms) has no layer order to give

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

    if( !is_neg( x_translated ) )
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

    if( !is_neg( y_translated ) )
    {
        return( floor( y_translated / GRID_BLOCK_SIZE ) );
    }
    else
    {
        return( 0 );
    }
}

span<uint> Room::Grid::x_range( FixedRectangle cref in_r ) const
{
    return( span<uint>( x( in_r.lower( ).x( ) ), x( in_r.upper( ).x( ) ) ) );
}

span<uint> Room::Grid::y_range( FixedRectangle cref in_r ) const
{
    return( span<uint>( y( in_r.lower( ).y( ) ), y( in_r.upper( ).y( ) ) ) );
}

void Room::Grid::traverse( FixedRectangle cref in_range, function<void( Room::Grid::Block ref in_block )> cref in_func )
{
    span<uint> x_span = x_range( in_range );
    span<uint> y_span = y_range( in_range );

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
    span<uint> x_span = x_range( in_range );
    span<uint> y_span = y_range( in_range );

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
