#ifndef PineTree_hpp
#define PineTree_hpp

#include "MountMerciless.hpp"
#include "Object.hpp"

namespace mtmercy
{

class PineTree : public Object
{

public:

    PineTree( Room *, Coordinate cref root, dec z = 1.0 );

    virtual void render( ) override;

private:

    varray<Drawing> m_leave_bunches_and_trunk;
    varray<Coordinate> m_leave_bunches_rotation_points;
    
};

} // namespace mtmercy

#endif /* PineTree_hpp */
