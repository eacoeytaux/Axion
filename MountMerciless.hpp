#ifndef MountMerciless_hpp
#define MountMerciless_hpp

#include "World.hpp"
#include "Object.hpp"
#include "Terrain.hpp"

#include "ColorPalette.hpp"

namespace mtmercy
{

class MtMerciless : public World
{

protected:

    virtual void create( ) override;

    virtual void assign_layer_position( Object * ) override;

    virtual dec air_resistance( ) const { return ( 0.025 ); }
    virtual Vector gravity( ) const { return ( VectorY( -0.6125 ) ); }

};

} // namespace mtmercy

#endif /* MountMerciless_hpp */
