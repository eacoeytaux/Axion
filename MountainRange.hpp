#ifndef MountainRange_hpp
#define MountainRange_hpp

#include "Object.hpp"

namespace mtmercy
{

class MountainRange : public Object
{
public:
    virtual ~MountainRange( ) { }
    MountainRange( World *, Planc base_bottom, double z, const Color & color );
};

} // namespace mtmercy

#endif /* MountainRange_hpp */
