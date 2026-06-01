#ifndef Dust_hpp
#define Dust_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Hazard.hpp"

namespace mtmercy
{

class Thorns : public Hazard
{

public:

    Thorns( ptr<Room> in_room, Point cref in_start, Point cref in_end );

    bool collide( ptr<Object> in_object ) override;

};

} // namespace mtmercy

#endif /* Dust_hpp */
