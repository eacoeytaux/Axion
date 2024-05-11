#ifndef GrassTerrain_hpp
#define GrassTerrain_hpp

#include "Terrain.hpp"

namespace mtmercy
{

class GrassTerrain : public Terrain
{
public:
    virtual ~GrassTerrain( ) { }
    GrassTerrain( World *, const varray<Coordinate> & vertices = { } );

    virtual const Color & dust_color( ) const;
};

} // namespace mtmercy

#endif /* GrassTerrain_hpp */
