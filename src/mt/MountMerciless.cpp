#include "mt/MountMerciless.hpp"

#include "axn/world/Door.hpp" // todo

#include "mt/rooms/TrainingGrounds.hpp"
#include "mt/rooms/Caves.hpp"

#include "axn/world/Enemy.hpp"
#include "axn/world/Projectile.hpp"
#include "axn/world/Hazard.hpp"

#include "mt/player/Climber.hpp"
#include "mt/player/Arrow.hpp"

#include <concepts>

using namespace mtmercy;
#define assign( ... ) if( ( cast_const( in_object, __VA_ARGS__ ) && ( order.push( score + 1 ) && !( score = 0 ) ) ) || ++score ) { } macro_end

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

Visible::LayerOrder MtMerciless::layer_order( ptr<const Object> in_object ) const
{
    uint score = 0;
    Visible::LayerOrder order;

    assign( Terrain );
    assign( Projectile );
    assign( Enemy ); // should see enemies over player
    assign( Player ); // should see player over non-crucial objects
    assign( Mob ); // non-enemies are least important
    assign( Arrow );

    assign( Object );

    assign( Hazard );
    assign( Door );

    return( order );
}

dec MtMerciless::air_resistance( ) const { return( 0.025 ); }
Vector MtMerciless::gravity( ) const { return( VectorY( -0.625 ) ); }
