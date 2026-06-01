#include "mt/rooms/Caves.hpp"

#include "axn/world/Door.hpp" // todo

#include "mt/terrain/CavesTerrain.hpp"

#include "mt/environment/misc/CampFire.hpp"
#include "mt/environment/misc/Bone.hpp"

#include "mt/player/Climber.hpp"

var_const( Planc DOOR_PLAYER_WIDTH = ( 2.5 ) );
var_const( Planc DOOR_PLAYER_HEIGHT = ( 1.5 ) );

// TODO remove static player count
ptr<Player> Caves::create_player( Point cref in_position ) { static uint player_count = 0; return( new Climber( ++player_count, this, in_position ) ); }

void Caves::create( )
{
    Room::create( FixedRectangle( 5000.0, 2000.0 ) );

    fill_color( GRAY_ULTRA_DARK );

    lighting_active( true );

    add_player( Point( 0.0, 50.0 ) );

    add_door( new Door( this, Point( 0.0, 0.0 ), player_main( )->width( ) * DOOR_PLAYER_WIDTH, player_main( )->height( ) * DOOR_PLAYER_HEIGHT ) );

    add_object( new CampFire( this, Point( -100.0, 20.0 ) ) );
    add_object( new CampFire( this, Point( +100.0, 20.0 ) ) );
    add_object( new CampFire( this, Point( 0.0, 150.0 ) ) );
}

ptr<Terrain> Caves::generate_terrain( )
{
    vvarray<Point> terrain_points;

    // ground
    terrain_points.insert_back( {
        Point( bounds( ).lower_bound_x( ), bounds( ).lower_bound_y( ) ),
        Point( bounds( ).lower_bound_x( ), bounds( ).upper_bound_y( ) ),
        Point( bounds( ).lower_bound_x( ) + 100.0, bounds( ).upper_bound_y( ) ),
        Point( bounds( ).lower_bound_x( ) + 150.0, 0.0 ),
        Point( -150.0, 0.0 ),
        Point( +150.0, 0.0 ),
        Point( bounds( ).upper_bound_x( ) - 150.0, 0.0 ),
        Point( bounds( ).upper_bound_x( ) - 100.0, bounds( ).upper_bound_y( ) ),
        Point( bounds( ).upper_bound_x( ), bounds( ).upper_bound_y( ) ),
        Point( bounds( ).upper_bound_x( ), bounds( ).lower_bound_y( ) ) } );

    terrain_points.insert_back( {
        Point( bounds( ).upper_bound_x( ), bounds( ).upper_bound_y( ) ),
        Point( bounds( ).upper_bound_x( ), bounds( ).upper_bound_y( ) - 500.0 ),
        Point( bounds( ).lower_bound_x( ), bounds( ).upper_bound_y( ) - 500.0 ),
        Point( bounds( ).lower_bound_x( ), bounds( ).upper_bound_y( ) ) } );

    return( new CavesTerrain( this, terrain_points ) );
}