#ifndef Thorns_hpp
#define Thorns_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Hazard.hpp"

namespace mtmercy
{

class Thorns : public Hazard
{

public:

    Thorns( ptr<Room> in_room, Point cref in_start, Point cref in_end );

    virtual bool collide( ptr<Object> in_object ) override;

};

} // namespace mtmercy

#endif /* Thorns_hpp */
