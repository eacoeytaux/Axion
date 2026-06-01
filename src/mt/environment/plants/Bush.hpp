#ifndef Bush_hpp
#define Bush_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class Bush : public Object
{

public:

    Bush( ptr<Room> in_room, Point cref in_root, dec in_z = 1.0 );

};

} // namespace mtmercy

#endif /* Bush_hpp */
