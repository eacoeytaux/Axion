#ifndef Hills_hpp
#define Hills_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class Hills : public Object
{

public:

    Hills( ptr<Room> in_room, Planc cref in_base_bottom, dec in_z );

};

} // namespace mtmercy

#endif /* Hills_hpp */
