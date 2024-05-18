#ifndef AspenTree_hpp
#define AspenTree_hpp

#include "Object.hpp"

namespace mtmercy
{

class AspenTree : public Object
{
public:
    virtual ~AspenTree( ) { }
    AspenTree( World *, const Coordinate & root, dec z = 1.0 );
};

} // namespace mtmercy

#endif /* AspenTree_hpp */
