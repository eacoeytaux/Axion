#ifndef Hazard_hpp
#define Hazard_hpp

#include "Object.hpp"

namespace axn
{
namespace reality
{

class Hazard : public Object
{

public:

    Hazard( Room * room, Coordinate cref position = ORIGIN ) : Object( room, position )
    {
        #if defined ( AXN_DEBUG )
        draw_debug_color = MAGENTA;
        #endif
    }

};

} // namespace reality
} // namespace axn

#endif /* Hazard_hpp */
