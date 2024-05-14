#include "MountMerciless.hpp"

#include "Climber.hpp"

#include "GrassTerrain.hpp"
#include "Sky.hpp"
#include "Bush.hpp"
#include "PineTree.hpp"
#include "AspenTree.hpp"
#include "Hills.hpp"
#include "MountainRange.hpp"
#include "Waterfall.hpp"
#include "DistantBird.hpp"
#include "Skull.hpp"
#include "Fire.hpp"

#include "Bird.hpp"
#include "Fox.hpp"
#include "Snail.hpp"

using mtmercy::MountMerciless;

Player * MountMerciless::create_player( const Coordinate & _position )
{
    return new Climber( this, _position );
}

World & MountMerciless::create( )
{
    World::create( );

    wind( Vector( 1.5, 0.0 ) );

    background( new Sky( this ) );

    add_object( new MountainRange( this, 4000.0, 0.04, GRAY_DARK ) );
    add_object( new MountainRange( this, 2000.0, 0.05, GRAY_MID ) );
    add_object( new MountainRange( this, -1000.0, 0.07, GRAY_LIGHT ) );
    // add_object( new Hills( this, 1000.0, 0.1 ) );

    add_object( new DistantBird( this, Coordinate( -600.0, 800.0 ) ) );
    add_object( new DistantBird( this, Coordinate( -666.0, 866.0 ) ) );
    add_object( new DistantBird( this, Coordinate( -550.0, 900.0 ) ) );

    // add_object( new Waterfall( this, Coordinate( 0.0, 10.0 ), METER * 4.0, METER * 8.0, 0.95 ) );

    terrain( )->traverse_x( Span<Planc>( 50.0, 500.0 ), [ & ]( const Coordinate & c, const TerrainEdge * e )
                            { add_object( new AspenTree( this, c ) ); } );

    add_object( new Fire( this, Coordinate( 50, 20 ) ) );

    // add_object( new Skull( this, Coordinate( 250.0, 500.0 ) ) );

    // add_object( new Bird( this, Coordinate( -100.0, 100.0 ) ) );
    // add_object( new Fox( this, Coordinate( 200.0, 150.0 ) ) );
    add_object( new Snail( this, Coordinate( 200.0, 150.0 ) ) );

    return *this;
}

Terrain * MountMerciless::generate_terrain( )
{
    varray<Coordinate> terrain_coordinates = {
        Coordinate( -1000.0, 0.0 ),
        Coordinate( 0.0, 10.0 ),
        // Coordinate( -800.0, 5.0 ),
        // Coordinate( -100.0, 15.0 ),
        // Coordinate( 100.0, 25.0 ),
        // Coordinate( 200.0, 100.0 ),
        // Coordinate( 350.0, 75.0 ),
        // Coordinate( 500.0, 25.0 ),
        // Coordinate( 800.0, 5.0 ),
        Coordinate( 1000.0, 0.0 ) };

    return new GrassTerrain( this, terrain_coordinates );
}
