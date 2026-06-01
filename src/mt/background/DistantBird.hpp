#ifndef DistantBird_hpp
#define DistantBird_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class DistantBird : public Object
{

public:

    DistantBird( ptr<Room> in_room, Point cref in_position );

    virtual void render( ) override;

private:

    Cycle m_wing_offset;
    Cycle m_flight_offset;

};

} // namespace mtmercy

#endif /* DistantBird_hpp */
