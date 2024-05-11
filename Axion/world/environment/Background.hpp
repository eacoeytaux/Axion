#ifndef Background_hpp
#define Background_hpp

#include "Object.hpp"

namespace axn
{
namespace reality
{

class Background : public Object
{
public:
    virtual ~Background( ) { }
    Background( World * world ) : Object( world )
    {
        z( 0 );
        gravity_ratio( 0.0 );
        terrain_boundaries( false );
    }
};

} // namespace reality
} // namespace axn

#endif /* Background_hpp */
