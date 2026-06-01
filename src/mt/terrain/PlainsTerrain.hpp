#ifndef PlainsTerrain_hpp
#define PlainsTerrain_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Terrain.hpp"

namespace mtmercy
{

class PlainsTerrain : public Terrain
{

public:

    PlainsTerrain( ptr<Room> in_room, vvarray<Point> cref in_vertices = { } );

    bool grassy( uint in_type ) const;

    bool creates_dust( uint in_type ) const;
    Color dust_color( uint in_type ) const;

};

} // namespace mtmercy

#endif /* PlainsTerrain_hpp */
