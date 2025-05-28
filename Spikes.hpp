#ifndef Spikes_hpp
#define Spikes_hpp

#include "MountMerciless.hpp"
#include "Object.hpp"

namespace mtmercy
{

class Spike : public Object
{
    
public:
    
    Spike( Room *, Coordinate cref base );
    
protected:
    
    virtual bool collide( Object * ) override;
    
};

} // namespace mtmercy

#endif /* Spikes_hpp */
