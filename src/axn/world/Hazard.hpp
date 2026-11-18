#ifndef Hazard_hpp
#define Hazard_hpp

#include "axn/world/Object.hpp"

namespace axn
{
namespace reality
{

class Hazard : public Object
{

public:

    Hazard( ptr<Room> in_room, Point cref in_position = ORIGIN ) : Object( in_room, in_position )
    {
        #if defined( AXN_DEBUG )
        draw_debug_color = MAGENTA;
        #endif
    }

};

} // namespace reality
} // namespace axn

#endif /* Hazard_hpp */
