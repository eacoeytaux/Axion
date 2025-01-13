#ifndef AspineTree_hpp
#define AspineTree_hpp

#include "Object.hpp"

namespace mtmercy
{

class AspineTree : public Object
{
public:
    AspineTree( World *, Coordinate cref root, dec z = 1.0 );
};

} // namespace mtmercy

#endif /* AspineTree_hpp */
