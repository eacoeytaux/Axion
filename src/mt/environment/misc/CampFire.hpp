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

};

} // namespace mtmercy

#endif /* CampFire_hpp */
