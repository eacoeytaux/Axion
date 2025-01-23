#ifndef GrassTerrain_hpp
#define GrassTerrain_hpp

#include "MountMerciless.hpp"
#include "Terrain.hpp"

namespace mtmercy
{

class GrassTerrain : public Terrain
{
public:
    GrassTerrain( Room *, const varray<varray<Coordinate>> & vertices = { } );

    virtual Color cref dust_color( ) const;
};

} // namespace mtmercy

#endif /* GrassTerrain_hpp */
