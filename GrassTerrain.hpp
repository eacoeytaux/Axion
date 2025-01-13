#ifndef GrassTerrain_hpp
#define GrassTerrain_hpp

#include "Terrain.hpp"

namespace mtmercy
{

class GrassTerrain : public Terrain
{
public:
    virtual ~GrassTerrain( ) { }

    GrassTerrain( World *, const varray<varray<Coordinate>> & vertices = { } );

    virtual Color cref dust_color( ) const;
};

} // namespace mtmercy

#endif /* GrassTerrain_hpp */
