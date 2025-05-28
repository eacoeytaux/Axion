#ifndef Stalite_hpp
#define Stalite_hpp

#include "MountMerciless.hpp"
#include "Object.hpp"

namespace mtmercy
{

class Stalite : public Object
{
    
public:
    
    Stalite( Room *, bool hanging, const Coordinate & base, Planc cref width, Planc cref height, dec z = 1.0 );
    
};

} // namespace mtmercy

#endif /* Stalite_hpp */
