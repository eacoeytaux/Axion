#ifndef Hills_hpp
#define Hills_hpp

#include "Object.hpp"

namespace mtmercy
{

class Hills : public Object
{
public:
    virtual ~Hills( ) { }
    Hills( World *, Planc base_bottom, double z );

    virtual const Hills & render( ) const override;

private:
    Drawing m_hills;
};

} // namespace mtmercy

#endif /* Hills_hpp */
