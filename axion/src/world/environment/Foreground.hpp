#ifndef Foreground_hpp
#define Foreground_hpp

#include "axn.hpp"
#include "Object.hpp"

namespace axn {
namespace reality {

class Foreground : public Object {
public:
    virtual ~Foreground( ) {}
    Foreground( World* world ) : Object( world ) {}
};

} // namespace reality
} // namespace axn

#endif /* Foreground_hpp */
