#ifndef Terrain_hpp
#define Terrain_hpp

#include "Object.hpp"
#include "TerrainEdge.hpp"
#include "TerrainVertex.hpp"

namespace axn
{
namespace reality
{

class Terrain : public Object
{
public:
    virtual ~Terrain( );
    Terrain( World *, const varray<Coordinate> & vertices = { } );

#ifdef AXN_DEBUG
    virtual Drawing debug_overlay( ) const override;
#endif

    const varray<TerrainVertex *> & vertices( ) const;
    const varray<TerrainEdge *> & edges( ) const;

    void traverse_x( const Span<Planc> & _distance_x, const function<void( const Coordinate &, const TerrainEdge * )> & f ) const;
    void traverse_x( const Planc & _distance_x, const function<void( const Coordinate &, const TerrainEdge * )> & f ) const { return traverse_x( Span<Planc>( _distance_x, _distance_x ), f ); }

    virtual const Color & dust_color( ) const { return TRANSPARENT; }

private:
    TerrainVertex * make_vertex( const Coordinate & position );
    TerrainEdge * make_edge( TerrainVertex * v1, TerrainVertex * v2 );

    varray<TerrainVertex *> m_vertices;
    varray<TerrainEdge *> m_edges;
};

} // namespace reality
} // namespace axn

#endif /* Terrain_hpp */
