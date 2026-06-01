#ifndef MountMerciless_hpp
#define MountMerciless_hpp

#include "axn/world/World.hpp"
#include "axn/world/Object.hpp"
#include "axn/world/Terrain.hpp"

namespace mtmercy
{

class MtMerciless : public World
{

public:

    virtual string name( ) const override { return( "Mount Merciless" ); }

protected:

    virtual void create( ) override;

    virtual void assign_layer_position( ptr<Object> in_object ) override;

    virtual dec air_resistance( ) const override { return( 0.025 ); } // todo
    virtual Vector gravity( ) const override { return( VectorY( -0.6125 ) ); } // todo

};

section( Colors )
{

    var_const( Color PLANT_GREEN_1 = ( Color::rgb( 0x00C000 ) ) );
    var_const( Color PLANT_GREEN_2 = ( Color::rgb( 0x008000 ) ) );
    var_const( Color PLANT_GREEN_3 = ( Color::rgb( 0x006000 ) ) );
    var_const( Color PLANT_GREEN_4 = ( Color::rgb( 0x004000 ) ) );

    var_const( Color DIRT_COLOR_1 = ( Color::rgb( 0x803000 ) ) );
    var_const( Color DIRT_COLOR_2 = ( Color::rgb( 0x441E00 ) ) );

    var_const( Color WOOD_COLOR_1 = ( Color::rgb( 0xE89048 ) ) );

}

} // namespace mtmercy

using namespace mtmercy;

#endif /* MountMerciless_hpp */
