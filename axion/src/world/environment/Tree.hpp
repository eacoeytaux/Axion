#ifndef Tree_hpp
#define Tree_hpp

#include "axn.hpp"
#include "Object.hpp"

namespace axn {
namespace reality {

class Tree : public Object {
public:
    virtual ~Tree( ) {}
    Tree( World*, const Coordinate & root, ufloat z = 1.f );

    virtual const Tree & render( ) const override;
};

} // namespace reality
} // namespace axn

#endif /* Tree_hpp */
