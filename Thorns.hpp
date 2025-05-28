#ifndef Dust_hpp
#define Dust_hpp

#include "MountMerciless.hpp"
#include "Object.hpp"

namespace mtmercy
{

class Thorns : public Object
{
    
public:
    
    Thorns( Room *, const Coordinate &, const Coordinate & );
    
    bool collide( Object * ) override;

};

} // namespace mtmercy

#endif /* Dust_hpp */
