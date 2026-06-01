#ifndef DistantBird_hpp
#define DistantBird_hpp

#include "MountMerciless.hpp"
#include "Object.hpp"

namespace mtmercy
{

class DistantBird : public Object
{

public:

    DistantBird( Room *, Coordinate cref position );

    virtual void render( ) override;

private:

    Cycle m_wing_offset;
    Cycle m_flight_offset;

};

} // namespace mtmercy

#endif /* DistantBird_hpp */
