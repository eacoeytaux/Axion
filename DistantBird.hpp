#ifndef DistantBird_hpp
#define DistantBird_hpp

#include "Object.hpp"

namespace mtmercy
{

class DistantBird : public Object
{
public:
    virtual ~DistantBird( ) { }
    DistantBird( World *, const Coordinate & position );

    virtual void render( ) override;

private:
    uint m_wing_cycle_offset;
    uint m_flight_cycle_offset;
};

} // namespace mtmercy

#endif /* DistantBird_hpp */
