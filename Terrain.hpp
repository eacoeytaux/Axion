#ifndef Terrain_hpp
#define Terrain_hpp

#include "Object.hpp"

namespace axn
{
namespace reality
{

class TerrainNode
{
public:
    virtual ~TerrainNode( ) { }

    virtual bool passable( ) const { return true; }
    virtual Angle normal( ) const = 0;
    virtual dec resistance( ) const = 0;
};

class TerrainEdge;
class TerrainVertex : public TerrainNode
{
public:
    virtual ~TerrainVertex( ) { }
    TerrainVertex( const Coordinate & pos );

    Coordinate position( ) const;

    TerrainEdge * edge1( ) const;
    TerrainEdge * edge2( ) const;

    void edge1( TerrainEdge * );
    void edge2( TerrainEdge * );

    Angle normal( ) const override;
    dec resistance( ) const override;

private:
    Coordinate m_position;
    TerrainEdge * m_e1 = nullptr;
    TerrainEdge * m_e2 = nullptr;

    dec m_resistance;
};

class TerrainEdge : public TerrainNode
{
public:
    virtual ~TerrainEdge( ) { }
    TerrainEdge( TerrainVertex * v1, TerrainVertex * v2, dec resistance = 0.1 );
    TerrainEdge( const TerrainEdge & );

    Line line( ) const;
    Vector vector( ) const;

    TerrainVertex * vertex1( ) const;
    TerrainVertex * vertex2( ) const;

    Angle normal( ) const override;
    dec resistance( ) const override;

private:
    TerrainVertex * m_v1 = nullptr;
    TerrainVertex * m_v2 = nullptr;

    dec m_resistance;
};

class Terrain : public Object
{
public:
    virtual ~Terrain( );
    Terrain( World *, const varray<varray<Coordinate>> & vertices = { } );

#ifdef AXN_DEBUG
    virtual Drawing debug_overlay( ) const override;
#endif

    const varray<varray<TerrainVertex *>> & vertices( ) const;
    const varray<varray<TerrainEdge *>> & edges( ) const;

    varray<TerrainEdge *> edges( const FixedRectangle & ) const;

    void traverse_x( const Span<Planc> & _distance_x, const function<void( const Coordinate &, const TerrainEdge * )> & f ) const;
    void traverse_x( const Planc & _distance_x, const function<void( const Coordinate &, const TerrainEdge * )> & f ) const { traverse_x( Span<Planc>( _distance_x, _distance_x ), f ); }

    virtual const Color & dust_color( ) const { return TRANSPARENT; }

private:
    void make( const varray<Coordinate> & positions, bool loop );

    varray<varray<TerrainVertex *>> m_vertices;
    varray<varray<TerrainEdge *>> m_edges;
};

} // namespace reality
} // namespace axn

#endif /* Terrain_hpp */
