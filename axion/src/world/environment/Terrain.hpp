#ifndef Terrain_hpp
#define Terrain_hpp

#include "axn.hpp"
#include "Graphics.hpp"
#include "Object.hpp"
#include "TerrainVertex.hpp"
#include "TerrainEdge.hpp"

namespace axn {
namespace reality {

class Terrain : public Object {
public:
    virtual ~Terrain( );
    Terrain( World*, const varray<Coordinate> & vertices = { } );

    const varray<TerrainVertex*> & vertices( ) const;
    const varray<TerrainEdge*> & edges( ) const;

#ifdef AXN_DEBUG
    virtual Drawing debug_overlay( ) const override;
#endif

    virtual const Terrain & render( ) const override;
    virtual Terrain & update( ) override;

private:
    TerrainVertex* make_vertex( const Coordinate & position );
    TerrainEdge* make_edge( TerrainVertex* v1, TerrainVertex* v2 );

    varray<TerrainVertex*> m_vertices;
    varray<TerrainEdge*> m_edges;
};

} // namespace reality
} // namespace axn

#endif /* Terrain_hpp */
