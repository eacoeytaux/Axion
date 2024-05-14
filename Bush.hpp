#ifndef Bush_hpp
#define Bush_hpp

#include "Object.hpp"

namespace mtmercy
{

class Bush : public Object
{
public:
    virtual ~Bush( ) { }
    Bush( World *, const Coordinate & root, double z = 1.0 );
};

} // namespace mtmercy

#endif /* Bush_hpp */
