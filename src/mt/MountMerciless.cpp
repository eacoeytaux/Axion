#include "mt/MountMerciless.hpp"

#include "axn/world/Door.hpp" // todo

#include "mt/rooms/TrainingGrounds.hpp"
#include "mt/rooms/Caves.hpp"

#include "axn/world/Enemy.hpp"
#include "axn/world/Projectile.hpp"
#include "axn/world/Hazard.hpp"

#include "mt/player/Climber.hpp"
#include "mt/player/Arrow.hpp"

using namespace mtmercy;

namespace
{

struct object_layer_param
{

    object_layer_param( ptr<const Object> in_object ) : object( in_object ) { }

    uint score = 0;
    queue<uint> scores;
    ptr<const Object> object = nullptr;

};

typeT inline bool score_layer_position_i( object_layer_param ref in_p )
{
    if( cast_const( T1, in_p.object ) )
    {
        in_p.scores.push( in_p.score + 1 );
        return( in_p.score = 0 );
    }
    else
    {
        return( ++in_p.score );
    }
};

#define o( Class ) if( score_layer_position_i<Class>( p ) ) { }
inline queue<uint> score_layer_position( ptr<Object> in_object )
{
    object_layer_param p( in_object );

    return_if( !cast_const( Object, in_object ), p.scores );

    o( Terrain );
    o( Projectile );
    o( Enemy ); // should see enemies over player
    o( Player ); // should see player over non-crucial objects
    o( Mob ); // non-enemies are least important
    o( Arrow );

    o( Object );

    o( Hazard );
    o( Door );

    return( p.scores );

};

} // namespace

void MtMerciless::assign_layer_position( ptr<Object> in_object )
{
    in_object->layer_position( score_layer_position( in_object ) );
}

void MtMerciless::create( )
{
    World::create( );

    ptr<Room> training_grounds = add_room( new TrainingGrounds( this ), true );
    ptr<Room> caves = add_room( new Caves( this ) );

    ptr<Door> door1 = *training_grounds->doors( ).begin( );
    ptr<Door> door2 = *caves->doors( ).begin( );

    if( !is_null( door1 ) ) { door1->out( door2 ); }
    if( !is_null( door2 ) ) { door2->out( door1 ); }
}
