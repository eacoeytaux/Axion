#ifndef TrainingGrounds_hpp
#define TrainingGrounds_hpp

#include "axn/world/Room.hpp"

namespace mtmercy
{

class TrainingGrounds : public Room
{

public:

    TrainingGrounds( ptr<World> in_world ) : Room( in_world ) { }

protected:

    virtual void create( ) override;

    virtual ptr<Terrain> generate_terrain( ) override;

    virtual ptr<Player> create_player( Point cref in_position ) override;

};

} // namespace mtmercy

#endif /* TrainingGrounds_hpp */
