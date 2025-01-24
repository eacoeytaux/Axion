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

    uint m_wing_cycle_offset;
    uint m_flight_cycle_offset;
};

} // namespace mtmercy

#endif /* DistantBird_hpp */
