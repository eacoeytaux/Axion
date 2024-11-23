#ifndef MountMerciless_hpp
#define MountMerciless_hpp

#include "World.hpp"

namespace mtmercy
{

class MountMerciless : public World
{
protected:
    virtual void create( ) override;
    virtual Terrain * generate_terrain( ) override;

    virtual Player * add_player( const Coordinate & position ) override;
    virtual Player * create_player( const Coordinate & position ) override;

    virtual void assign_layer_position( Object * ) override;
};

} // namespace mtmercy

#endif /* MountMerciless_hpp */
