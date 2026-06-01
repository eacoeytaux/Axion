#ifndef PineTree_hpp
#define PineTree_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class PineTree : public Object
{

public:

    PineTree( ptr<Room> in_room, Point cref in_root, dec in_z = 1.0 );

    virtual void render( ) override;

private:

    varray<Drawing> m_leave_bunches_and_trunk;
    varray<Point> m_leave_bunches_rotation_points;

};

} // namespace mtmercy

#endif /* PineTree_hpp */
