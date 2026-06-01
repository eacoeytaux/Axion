#ifndef Stalite_hpp
#define Stalite_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Object.hpp"

namespace mtmercy
{

class Stalite : public Object
{

public:

    Stalite( ptr<Room> in_room, bool in_hanging, Point cref in_base, Planc cref in_width, Planc cref in_height, dec in_z = 1.0 );

};

} // namespace mtmercy

#endif /* Stalite_hpp */
