#include "Plains.hpp"
#include "Climber.hpp"
#include "Arrow.hpp"
#include "GrassTerrain.hpp"
#include "Thorns.hpp"
#include "Sky.hpp"
#include "DistantBird.hpp"
#include "MountainRange.hpp"
#include "Hills.hpp"
#include "PineTree.hpp"
#include "AspineTree.hpp"
#include "Waterfall.hpp"
#include "Stalagmite.hpp"
#include "Bush.hpp"
#include "CampFire.hpp"
#include "Skull.hpp"
#include "Bird.hpp"
#include "Fox.hpp"
#include "Butterfly.hpp"
#include "Porkupine.hpp"
#include "Gopher.hpp"
#include "Unalope.hpp"
#include "Snail.hpp"
#include "Wisp.hpp"

using namespace mtmercy;
using mtmercy::Plains;

Player * Plains::create_player( Coordinate cref _position ) { return new Climber( this, _position ); }

Player * Plains::add_player( Coordinate cref _position ) { return Room::add_player( _position ); }

void Plains::create( )
{
    Room::create( FixedRectangle( 5000.0, 2500.0 ) );

    wind( Vector( 0.75, 0.0 ) );

    add_object( new Sky( this ) );

    add_object( new MountainRange( this, 4000.0, 0.04, GRAY_DARK ) );
    add_object( new MountainRange( this, 2000.0, 0.05, GRAY_MID ) );
    add_object( new MountainRange( this, -1000.0, 0.07, GRAY_LIGHT ) );
    // add_object( new Hills( this, -1500.0, 0.1 ) );

    // add_object( new DistantBird( this, Coordinate( -600.0, 800.0 ) ) );
    // add_object( new DistantBird( this, Coordinate( -666.0, 866.0 ) ) );
    // add_object( new DistantBird( this, Coordinate( -550.0, 900.0 ) ) );

    add_object( new Waterfall( this, Coordinate( 0.0, 50.0 ), METER * 4.0, METER * 8.0, 0.95 ) );

    varray<AspineTree *> aspines;
    
    list<dec> aspine_zs = { 1.0 }; // { 1.0, 0.5 };
    for_each( z, aspine_zs )
    {
        terrain( )->traverse_x( Span<Planc>( 50.0, 500.0 ),
                                [ & ] ( Coordinate cref c, const TerrainEdge * e )
        {
            aspines.insert_back( new AspineTree( this, c, z ) );
        } );
    }
    
    Random::shuffle( aspines );
    
    for_each( aspine, aspines )
    {
        add_object( aspine );
    }

    add_object( new CampFire( this, Coordinate( 50.0, 20.0 ) ) );

    // add_object( new Thorns( this, Coordinate( 100, 20 ), Coordinate( 150, 15 ) ) );

    // add_object( new Skull( this, Coordinate( 250.0, 500.0 ) ) );

    // add_object( new Butterfly( this, Coordinate( 200, 50 ) ) );
    // add_object( new Snail( this, Coordinate( 200.0, 150.0 ) ) );

    // add_object( new Bird( this, Coordinate( -100.0, 100.0 ) ) );
    // add_object( new Fox( this, Coordinate( 200.0, 150.0 ) ) );
    // add_object( new Gopher( this, Coordinate( 150, 20 ) ) );
    // add_object( new Unalope( this, Coordinate( 150, 20 ) ) );
    // add_object( new Porkupine( this, Coordinate( 100, 30 ) ) );
    add_object( new Wisp( this, Coordinate( 200.0, 150.0 ) ) );

    add_player( Coordinate( 0.0, 300.0 ) );
}

Terrain * Plains::generate_terrain( )
{
    varray<varray<Coordinate>> terrain_coordinates = { { // Coordinate( -1100.0, 1000.0 ),
                                                         Coordinate( -1000.0, 0.0 ),
                                                         Coordinate( 0.0, 10.0 ),
                                                         // Coordinate( -800.0, 5.0 ),
                                                         // Coordinate( -100.0, 15.0 ),
                                                         // Coordinate( 100.0, 25.0 ),
                                                         // Coordinate( 200.0, 100.0 ),
                                                         // Coordinate( 350.0, 75.0 ),
                                                         // Coordinate( 500.0, 25.0 ),
                                                         // Coordinate( 800.0, 5.0 ),
                                                         Coordinate( 1000.0, 0.0 ) } };

    return new GrassTerrain( this, terrain_coordinates );
}
