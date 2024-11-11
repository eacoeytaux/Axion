#ifndef Hills_hpp
#define Hills_hpp

#include "Object.hpp"

namespace mtmercy
{

class Hills : public Object
{
public:
    virtual ~Hills( ) { }
    Hills( World *, Planc base_bottom, dec z );
};

} // namespace mtmercy

#endif /* Hills_hpp */
