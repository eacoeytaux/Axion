#ifndef Bush_hpp
#define Bush_hpp

#include "Object.hpp"

namespace mtmercy
{

class Bush : public Object
{
public:
    Bush( World *, Coordinate cref root, dec z = 1.0 );
};

} // namespace mtmercy

#endif /* Bush_hpp */
