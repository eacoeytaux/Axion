#ifndef Snail_hpp
#define Snail_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Mob.hpp"

namespace mtmercy
{

class Snail : public Mob
{

public:

    Snail( ptr<Room> in_room, Point cref in_position );

    virtual void render( ) override;
    virtual void update( ) override;

private:

};

} // namespace mtmercy

#endif /* Snail_hpp */
