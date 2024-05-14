#ifndef Foreground_hpp
#define Foreground_hpp

#include "Object.hpp"

namespace axn
{
namespace reality
{

class Foreground : public Object
{
public:
    virtual ~Foreground( ) { }
    Foreground( World * world ) : Object( world )
    {
        gravity_ratio( 0.0 );
        terrain_boundaries( false );
    }
};

} // namespace reality
} // namespace axn

#endif /* Foreground_hpp */
