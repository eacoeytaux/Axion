#ifndef Stalagmite_hpp
#define Stalagmite_hpp

#include "MountMerciless.hpp"
#include "Object.hpp"

namespace mtmercy
{

class Stalagmite : public Object
{
public:
    Stalagmite( Room *, const Coordinate & base, Planc height, dec z, const Color & color );
    
};

} // namespace mtmercy

#endif /* Stalagmite_hpp */
