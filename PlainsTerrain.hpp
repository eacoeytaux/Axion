#ifndef PlainsTerrain_hpp
#define PlainsTerrain_hpp

#include "MountMerciless.hpp"
#include "Terrain.hpp"

namespace mtmercy
{

class PlainsTerrain : public Terrain
{

public:

    PlainsTerrain( Room *, const varray<varray<Coordinate>> & vertices = { } );

    bool grassy( uint type ) const;

    bool creates_dust( uint type ) const;
    Color dust_color( uint type ) const;

};

} // namespace mtmercy

#endif /* PlainsTerrain_hpp */
