#ifndef Boulder_hpp
#define Boulder_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class Boulder : public Object
{

public:

    Boulder( ptr<Room> in_room, Point cref in_base, Planc cref in_scale, dec in_z = 1.0 );

};

} // namespace mtmercy

#endif /* Boulder_hpp */
