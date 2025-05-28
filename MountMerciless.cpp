#include "MountMerciless.hpp"
#include "Plains.hpp"
#include "Climber.hpp"
#include "Arrow.hpp"
#include "GrassTerrain.hpp"
#include "Thorns.hpp"
#include "Sky.hpp"
#include "DistantBird.hpp"
#include "MountainRange.hpp"
#include "Hills.hpp"
#include "Spikes.hpp"
#include "PineTree.hpp"
#include "AspineTree.hpp"
#include "Waterfall.hpp"
#include "Stalite.hpp"
#include "Boulder.hpp"
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

typeT inline bool score_layer_position_i( object_layer_param & p )
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
    o( CampFire )
    o( Bush )
    o( Terrain )
    o( AspineTree )
    o( AspineTree::Leaf )

    return p.scores;
    
};

} // namespace

void MountMerciless::assign_layer_position( Object * object )
{
    object->layer_position( score_layer_position( object ) );
}

void MountMerciless::create( )
{
    World::create( );
    
    m_current_room = new Plains( this );
    m_current_room->init( );
}
