#ifndef Fox_hpp
#define Fox_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Enemy.hpp"

namespace mtmercy
{

class Fox : public Enemy
{

public:

    Fox( ptr<Room> in_room, Point cref in_position );

    virtual void render( ) override;

};

} // namespace mtmercy

#endif /* Fox_hpp */
