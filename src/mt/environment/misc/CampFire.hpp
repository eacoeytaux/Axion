#ifndef CampFire_hpp
#define CampFire_hpp

#include "mt/MountMerciless.hpp"
#include "mt/environment/misc/Fire.hpp"

namespace mtmercy
{

class CampFire : public Fire
{

public:

    CampFire( ptr<Room> in_room, Point cref in_position );

protected:

    virtual void update( ) override; // C! now and then, while it burns, plays a crackle

};

} // namespace mtmercy

#endif /* CampFire_hpp */
