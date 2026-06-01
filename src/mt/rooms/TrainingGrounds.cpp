#include "mt/rooms/TrainingGrounds.hpp"

#include "axn/world/Door.hpp" // todo

#include "mt/terrain/PlainsTerrain.hpp"

#include "mt/background/Sky.hpp"
#include "mt/background/Hills.hpp"
#include "mt/background/MountainRange.hpp"
#include "mt/background/DistantBird.hpp"

#include "mt/environment/misc/CampFire.hpp"
#include "mt/environment/misc/Bone.hpp"

#include "mt/player/Climber.hpp"
#include "mt/player/Arrow.hpp"

//#include "mt/hazards/Thorns.hpp"
#include "mt/hazards/Spikes.hpp"

#include "mt/environment/plants/Bush.hpp"
#include "mt/environment/plants/PineTree.hpp"
#include "mt/environment/plants/AspineTree.hpp"
#include "mt/environment/water/Waterfall.hpp"
#include "mt/environment/water/WaterDrop.hpp"
//#include "mt/environment/rocks/Stalite.hpp"
//#include "mt/environment/rocks/Boulder.hpp"
#include "mt/environment/critters/Butterfly.hpp"
#include "mt/environment/critters/Snail.hpp"
#include "mt/environment/misc/Sign.hpp"

//#include "mt/mobs/Bird.hpp"
//#include "mt/mobs/enemies/Fox.hpp"
//#include "mt/mobs/enemies/Porkupine.hpp"
//#include "mt/mobs/enemies/Gopher.hpp"
//#include "mt/mobs/enemies/Unalope.hpp"
#include "mt/mobs/enemies/Wisp.hpp"

var_const( Planc DOOR_PLAYER_WIDTH = ( 2.5 ) );
var_const( Planc DOOR_PLAYER_HEIGHT = ( 1.5 ) );

// TODO remove static player count
ptr<Player> TrainingGrounds::create_player( Point cref in_position ) { static uint player_count = 0; return( new Climber( ++player_count, this, in_position ) ); }

void TrainingGrounds::create( )
{
    Room::create( FixedRectangle( 5000.0, 2500.0 ) );

    wind( Vector( 0.75, 0.0 ) );

    add_object( new Sky( this ) );

    add_object( new MountainRange( this, 4000.0, 0.04, GRAY_MID_DARK ) );
    add_object( new MountainRange( this, 2000.0, 0.05, GRAY_MID ) );
    add_object( new MountainRange( this, -1000.0, 0.07, GRAY_MID_LIGHT ) );
    // add_object( new Hills( this, -1500.0, 0.1 ) );

    add_object( new DistantBird( this, Point( -600.0, 800.0 ) ) );
    add_object( new DistantBird( this, Point( -666.0, 866.0 ) ) );
    add_object( new DistantBird( this, Point( -550.0, 900.0 ) ) );

    add_object( new Waterfall( this, Point( -1500.0, 0.0 ), ( METER * 4.0 ), ( METER * 8.0 ) ) );

    varray<ptr<Object>> aspines;

    aspines.insert_back( new AspineTree( this, Point( -25.0, 0.0 ) ) );

    list<dec> aspine_zs = { 1.0 }; // { 1.0, 0.5 };
    for_each( z, aspine_zs )
    {
        terrain( )->traverse_x( Span<Planc>( 50.0, 500.0 ), [ & ] ( Point cref in_pt, ptr<const Terrain::Edge> in_edge )
        {
            aspines.insert_back( new AspineTree( this, in_pt, z ) );
        } );
    }

    add_objects( Random::shuffled( aspines ) );

    add_player( Point( 350.0, 50.0 ) );

    add_door( new Door( this, Point( -325.0, -100.0 ), player_main( )->width( ) * DOOR_PLAYER_WIDTH, player_main( )->height( ) * DOOR_PLAYER_HEIGHT ) );

    add_object( new CampFire( this, Point( 0.0, 20.0 ) ) );

    add_object( new Femur( this, Point( 725.0, -200.0 ) ) );
    add_object( new Femur( this, Point( 675.0, -200.0 ) ) );
    add_object( new Skull( this, Point( 700.0, -200.0 ) ) );

    add_object( new Butterfly( this, Point( 0.0, 100.0 ) ) );
    add_object( new Butterfly( this, Point( 100.0, 100.0 ) ) );
    add_object( new Butterfly( this, Point( -100.0, 100.0 ) ) );

    add_object( new Wisp( this, Point( 650.0, 120.0 ) ) );
    add_object( new Wisp( this, Point( 700.0, 150.0 ) ) );
    add_object( new Wisp( this, Point( 750.0, 120.0 ) ) );

    add_object( new WaterDrop( this, Point( 100.0, 400.0 ) ) );

    // add_object( new Spike( this, Point( 675.0, -225.0 ) ) );
    // add_object( new Spike( this, Point( 700.0, -225.0 ) ) );
    // add_object( new Spike( this, Point( 725.0, -225.0 ) ) );

    // add_object( new Sign( this, Point( -70.0, -30.0 ) ) );

    // add_object( new Thorns( this, Point( 50.0, 0.0 ), Point( 150.0, 15.0 ) ) );

    // add_object( new Boulder( this, Point( -60.0, -10.0 ), 50.0 ) );

    // add_object( new Snail( this, Point( 100.0, 50.0 ) ) );

    // add_object( new Bird( this, Point( -150.0, 80.0 ) ) );
    // add_object( new Fox( this, Point( 150.0, 130.0 ) ) );
    // add_object( new Gopher( this, Point( 100.0, 0.0 ) ) );
    // add_object( new Unalope( this, Point( 100.0, 0.0 ) ) );
    // add_object( new Porkupine( this, Point( 50.0, 10.0 ) ) );
}

ptr<Terrain> TrainingGrounds::generate_terrain( )
{
    vvarray<Point> terrain_points;

    // ground
    terrain_points.insert_back( {
        Point( bounds( ).lower_bound_x( ), bounds( ).lower_bound_y( ) ),
        Point( bounds( ).lower_bound_x( ) + 100.0, 0.0 ),
        Point( -450.0, 0.0 ),
        Point( -400.0, -100.0 ),
        Point( -250.0, -100.0 ),
        Point( -200.0, 0.0 ),
        Point( 150.0, 0.0 ),
        Point( 550.0, -50.0 ),
        Point( 650.0, -100.0 ),
        Point( 660.0, -250.0 ),
        Point( 740.0, -250.0 ),
        Point( 750.0, -100.0 ),
        Point( 850.0, -50.0 ),
        Point( 1250.0, 0.0 ),
        Point( 1500.0, 0.0 ),
        Point( 1500.0, 0.0 ),
        Point( bounds( ).upper_bound_x( ) - 100.0, 0.0 ),
        Point( bounds( ).upper_bound_x( ), bounds( ).lower_bound_y( ) ) } );

    //terrain_points.insert_back( {
    //    Point( 1600.0, 550.0 ),
    //    Point( 3200.0, 500.0 ),
    //    Point( 0.0, 500.0 ) } );

    return( new PlainsTerrain( this, terrain_points ) );
}
