#ifndef Dust_hpp
#define Dust_hpp

#include "Object.hpp"

namespace mtmercy
{

class Thorns : public Object
{
public:
    virtual ~Thorns( ) { }
    
    Thorns( Room *, const Coordinate &, const Coordinate & );
    
    bool collide( Object * ) override;

private:
};

} // namespace mtmercy

#endif /* Dust_hpp */
