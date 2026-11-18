#ifndef MountainRange_hpp
#define MountainRange_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class MountainRange : public Object
{

public:

    MountainRange( ptr<Room> in_room, Planc cref in_base_bottom, dec in_z, Color cref in_color );

};

} // namespace mtmercy

#endif /* MountainRange_hpp */
