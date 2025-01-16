#ifndef MountainRange_hpp
#define MountainRange_hpp

#include "Object.hpp"

namespace mtmercy
{

class MountainRange : public Object
{
public:
    virtual ~MountainRange( ) { }
    MountainRange( Room *, Planc base_bottom, dec z, Color cref color );
};

} // namespace mtmercy

#endif /* MountainRange_hpp */
