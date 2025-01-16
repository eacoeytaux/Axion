#ifndef MountMerciless_hpp
#define MountMerciless_hpp

#include "World.hpp"

namespace mtmercy
{

class MountMerciless : public World
{
protected:
    virtual void create( ) override;

    virtual void assign_layer_position( Object * ) override;
};

} // namespace mtmercy

#endif /* MountMerciless_hpp */
