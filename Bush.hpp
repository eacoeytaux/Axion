#ifndef Bush_hpp
#define Bush_hpp

#include "MountMerciless.hpp"
#include "Object.hpp"

namespace mtmercy
{

class Bush : public Object
{

public:

    Bush( Room *, Coordinate cref root, dec z = 1.0 );
};

} // namespace mtmercy

#endif /* Bush_hpp */
