#include "mt/rooms/Caves.hpp"

#include "axn/world/Door.hpp" // todo

#include "mt/terrain/CavesTerrain.hpp"

#include "mt/environment/rocks/Stalaite.hpp"

#include "mt/environment/misc/CampFire.hpp"
#include "mt/environment/misc/Bone.hpp"

#include "mt/player/Climber.hpp"

var_const( Planc DOOR_PLAYER_WIDTH = ( METER * 1.5 ) );
var_const( Planc DOOR_PLAYER_HEIGHT = ( METER * 2.75 ) );

void Caves::create( )
{
    Room::create( FixedRectangle( 5000.0, 2000.0 ) );

    fill_color( GRAY_ULTRA_DARK );

    lighting_active( true );

    // C! no player of its own any more: the player comes in through the door from the training grounds (see World::pass( ))

    add_door( new Door( this, Point( 0.0, 0.0 ), DOOR_PLAYER_WIDTH, DOOR_PLAYER_HEIGHT ) );

    add_object( new CampFire( this, Point( -100.0, 20.0 ) ) );
    add_object( new CampFire( this, Point( +100.0, 20.0 ) ) );
    add_object( new CampFire( this, Point( 0.0, 150.0 ) ) );
}

ptr<Terrain> Caves::generate_terrain( )
{
    skip_override;

    vvarray<Point> terrain_pts;

    // ground
    terrain_pts.insert_back( {
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

    terrain_pts.insert_back( {
        Point( bounds( ).upper_bound_x( ), bounds( ).upper_bound_y( ) ),
        Point( bounds( ).upper_bound_x( ), bounds( ).upper_bound_y( ) - 500.0 ),
        Point( bounds( ).lower_bound_x( ), bounds( ).upper_bound_y( ) - 500.0 ),
        Point( bounds( ).lower_bound_x( ), bounds( ).upper_bound_y( ) ) } );

    return( new CavesTerrain( this, terrain_pts ) );
}

// C! numbered from the room's own players (a static count kept going up when the room was made again)
ptr<Player> Caves::create_player( Point cref in_position ) { skip_override; return( new Climber( ( player_count( ) + 1 ), this, in_position ) ); }
