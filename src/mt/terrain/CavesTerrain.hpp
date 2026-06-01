#ifndef CavesTerrain_hpp
#define CavesTerrain_hpp

#include "mt/MountMerciless.hpp"
#include "axn/world/Terrain.hpp"

namespace mtmercy
{

class CavesTerrain : public Terrain
{

public:

    CavesTerrain( ptr<Room> in_room, vvarray<Point> cref in_vertices = { } );

};

} // namespace mtmercy

#endif /* CavesTerrain_hpp */
