#include "MountMerciless.hpp"
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
#include "Fire.hpp"
#include "Skull.hpp"
#include "Bird.hpp"
#include "Fox.hpp"
#include "Butterfly.hpp"
#include "Porkupine.hpp"
#include "Gopher.hpp"
#include "Snail.hpp"

using namespace mtmercy;
using mtmercy::MountMerciless;

namespace
{

struct object_layer_param
{
    object_layer_param( const Object * o ) : object( o ) { }

    uint score = 0;
    queue<uint> scores;
    const Object * object = nullptr;
};

template <typename T>
inline bool score_layer_position_i( object_layer_param & p )
{
    if( dynamic_cast<const T *>( p.object ) )
    {
        p.scores.push( p.score + 1 );
        return p.score = 0;
    }
    else
    {
        return ++p.score;
    }
};

#define o( c ) if( score_layer_position_i<c>( p ) ) { }
inline queue<uint> score_layer_position( Object * object )
{
    object_layer_param p( object );

    if( !dynamic_cast<const Object *>( object ) )
    {
        return p.scores;
    }

    o( Thorns )

        o( Enemy )
        o( Player )
        o( Mob )

        o( Gopher )

        o( Arrow )

        o( Terrain )

        o( Fire )

        o( AspineTree )

        return p.scores;
};

} // namespace

void MountMerciless::assign_layer_position( Object * object )
{
    object->layer_position( score_layer_position( object ) );
}

Player * MountMerciless::create_player( const Coordinate & _position )
{
    return new Climber( this, _position );
}

Player * MountMerciless::add_player( const Coordinate & _position )
{
    Player * player = World::add_player( _position );
    return player;
}

void MountMerciless::create( )
{
    World::create( FixedRectangle( 5000.0, 2500.0 ) );

    wind( Vector( 0.75, 0.0 ) );

    add_object( new Sky( this ) );

    add_object( new MountainRange( this, 4000.0, 0.04, GRAY_DARK ) );
    add_object( new MountainRange( this, 2000.0, 0.05, GRAY_MID ) );
    add_object( new MountainRange( this, -1000.0, 0.07, GRAY_LIGHT ) );
    // add_object( new Hills( this, -1500.0, 0.1 ) );

    // add_object( new DistantBird( this, Coordinate( -600.0, 800.0 ) ) );
    // add_object( new DistantBird( this, Coordinate( -666.0, 866.0 ) ) );
    // add_object( new DistantBird( this, Coordinate( -550.0, 900.0 ) ) );

    // add_object( new Waterfall( this, Coordinate( 0.0, 10.0 ), METER * 4.0, METER * 8.0, 0.95 ) );

    terrain( )->traverse_x( Span<Planc>( 50.0, 500.0 ),
                            [ & ] ( const Coordinate & c, const TerrainEdge * e )
    {
        add_object( new AspineTree( this, c ) );
    } );

    // terrain( )->traverse_x( Span<Planc>( 50.0, 500.0 ),
    //                        [ & ]( const Coordinate & c, const TerrainEdge * e )
    //                        { add_object( new AspineTree( this, c, 0.5 ) ); } );

    // terrain( )->traverse_x( Span<Planc>( 50.0, 500.0 ),
    //                         [ & ]( const Coordinate & c, const TerrainEdge * e )
    //                         { add_object( new Stalagmite( this, c, 100.0, 1.0, GRAY_DARK ) ); } );

    add_object( new Fire( this, Coordinate( 50, 20 ) ) );

    // add_object( new Thorns( this, Coordinate( 100, 20 ), Coordinate( 150, 15 ) ) );

    // add_object( new Skull( this, Coordinate( 250.0, 500.0 ) ) );

    // add_object( new Bird( this, Coordinate( -100.0, 100.0 ) ) );
    // add_object( new Fox( this, Coordinate( 200.0, 150.0 ) ) );
    add_object( new Gopher( this, Coordinate( 150, 20 ) ) );
    add_object( new Porkupine( this, Coordinate( 100, 30 ) ) );
    // add_object( new Butterfly( this, Coordinate( 200, 50 ) ) );
    // add_object( new Snail( this, Coordinate( 200.0, 150.0 ) ) );
}

Terrain * MountMerciless::generate_terrain( )
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
    // Coordinate( 1100.0, 1000.0 ) } }; //, {
                                      // Coordinate( 200.0, 100.0 ),
                                      // Coordinate( 150.0, 150.0 ),
                                      // Coordinate( 100.0, 100.0 ) } };

    return new GrassTerrain( this, terrain_coordinates );
}
