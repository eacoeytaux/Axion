#ifndef Caves_hpp
#define Caves_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Room.hpp"

namespace mtmercy
{

class Caves : public Room
{

public:

    Caves( ptr<World> in_world ) : Room( in_world ) { }

protected:

    virtual void create( ) override;

    virtual ptr<Terrain> generate_terrain( ) override;

    virtual ptr<Player> create_player( Point cref in_position ) override;

};

} // namespace mtmercy

#endif /* Caves_hpp */
