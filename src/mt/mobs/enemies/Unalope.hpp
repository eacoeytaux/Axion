#ifndef Unalope_hpp
#define Unalope_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Enemy.hpp"

namespace mtmercy
{

class Unalope : public Enemy
{

public:

    Unalope( ptr<Room> in_room, Point cref in_position );

    virtual void render( ) override;

    virtual void update( ) override;

};

} // namespace mtmercy

#endif /* Unalope_hpp */
