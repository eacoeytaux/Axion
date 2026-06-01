#ifndef Boulder_hpp
#define Boulder_hpp

#include "MountMerciless.hpp"
#include "Object.hpp"

namespace mtmercy
{

class Boulder : public Object
{
    
public:
    
    Boulder( Room *, const Coordinate & base, Planc cref scale, dec z = 1.0 );
    
};

} // namespace mtmercy

#endif /* Boulder_hpp */
