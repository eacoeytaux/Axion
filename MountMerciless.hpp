#ifndef MountMerciless_hpp
#define MountMerciless_hpp

#include "World.hpp"

namespace mtmercy
{

class MountMerciless : public World
{
public:
    virtual Player * create_player( const Coordinate & position ) override;

protected:
    virtual World & create( ) override;
    virtual Terrain * generate_terrain( ) override;
};

} // namespace mtmercy

#endif /* MountMerciless_hpp */
