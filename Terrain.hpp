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
    virtual bool passable( ) const { return true; }
    virtual Angle normal( ) const = 0;
    virtual dec resistance( ) const = 0;

    virtual FixedRectangle bounding_box( ) const = 0;
};

class TerrainEdge;
class TerrainVertex : public TerrainNode
{
public:
    TerrainVertex( Coordinate cref pos );

    Coordinate cref position( ) const;

    TerrainEdge * edge1( ) const;
    TerrainEdge * edge2( ) const;

    void edge1( TerrainEdge * );
    void edge2( TerrainEdge * );

    Angle normal( ) const override;
    dec resistance( ) const override;

    FixedRectangle bounding_box( ) const override;

private:
    Coordinate m_position;
    TerrainEdge * m_e1 = nullptr;
    TerrainEdge * m_e2 = nullptr;

    dec m_resistance;
};

class TerrainEdge : public TerrainNode
{
public:
    TerrainEdge( TerrainVertex * v1, TerrainVertex * v2, dec resistance = 0.1 );
    TerrainEdge( TerrainEdge cref );

    Line line( ) const;
    Vector vector( ) const;

    TerrainVertex * vertex1( ) const;
    TerrainVertex * vertex2( ) const;

    Angle normal( ) const override;
    dec resistance( ) const override;

    FixedRectangle bounding_box( ) const;

private:
    TerrainVertex * m_v1 = nullptr;
    TerrainVertex * m_v2 = nullptr;

    dec m_resistance;
};

class Terrain : public Object
{
public:
    virtual ~Terrain( );
    Terrain( Room *, const varray<varray<Coordinate>> & vertices = { } );

    #ifdef AXN_DEBUG
    virtual Drawing debug_overlay( ) const override;
    #endif

    const varray<varray<TerrainVertex *>> & vertices( ) const;
    const varray<varray<TerrainEdge *>> & edges( ) const;

    void traverse_x( const Span<Planc> & _distance_x, const function<void( Coordinate cref, const TerrainEdge * )> & f ) const;
    void traverse_x( Planc cref _distance_x, const function<void( Coordinate cref, const TerrainEdge * )> & f ) const { traverse_x( Span<Planc>( _distance_x, _distance_x ), f ); }

    virtual Color cref dust_color( ) const { return TRANSPARENT; }

private:
    void make( varray<Coordinate> cref positions, bool loop );

    varray<varray<TerrainVertex *>> m_vertices;
    varray<varray<TerrainEdge *>> m_edges;
};

} // namespace reality
} // namespace axn

#endif /* Terrain_hpp */
