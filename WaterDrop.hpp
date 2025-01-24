#ifndef WaterDrop_hpp
#define WaterDrop_hpp

#include "MountMerciless.hpp"
#include "Object.hpp"

namespace mtmercy
{

class WaterDrop : public Object
{

public:

    WaterDrop( Room *, Coordinate cref position );

    virtual void render( ) override;
    virtual void update( ) override;
};

} // namespace mtmercy

#endif /* WaterDrop_hpp */
