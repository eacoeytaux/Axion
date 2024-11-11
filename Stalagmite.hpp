#ifndef Stalagmite_hpp
#define Stalagmite_hpp

#include "Object.hpp"

namespace mtmercy
{

class Stalagmite : public Object
{
public:
    virtual ~Stalagmite( ) { }
    Stalagmite( World *, const Coordinate & base, Planc height, dec z, const Color & color );
};

} // namespace mtmercy

#endif /* Stalagmite_hpp */
