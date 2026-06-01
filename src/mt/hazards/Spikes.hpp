#ifndef Spikes_hpp
#define Spikes_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Hazard.hpp"

namespace mtmercy
{

class Spike : public Hazard
{

public:

    Spike( ptr<Room> in_room, Point cref in_base );

protected:

    virtual bool collide( ptr<Object> in_object ) override;

};

} // namespace mtmercy

#endif /* Spikes_hpp */
