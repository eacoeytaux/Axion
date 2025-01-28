#ifndef CampFire_hpp
#define CampFire_hpp

#include "MountMerciless.hpp"
#include "Fire.hpp"

namespace mtmercy
{

class CampFire : public Fire
{

public:

    CampFire( Room *, Coordinate cref position );
    
};

} // namespace mtmercy

#endif /* CampFire_hpp */
