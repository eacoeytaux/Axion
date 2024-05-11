#ifndef TerrainEdge_hpp
#define TerrainEdge_hpp

#include "Geometry.hpp"

namespace axn
{
namespace reality
{

class TerrainVertex;
class TerrainEdge
{
public:
    virtual ~TerrainEdge( ) { }
    TerrainEdge( TerrainVertex * v1, TerrainVertex * v2, double resistance = 0.1 );
    TerrainEdge( const TerrainEdge & );

    Line line( ) const;
    Vector vector( ) const;

    TerrainVertex * vertex1( ) const;
    TerrainVertex * vertex2( ) const;

    Angle normal( ) const;

    double resistance( ) const;

private:
    TerrainVertex * m_v1 = nullptr;
    TerrainVertex * m_v2 = nullptr;

    double m_resistance;
};

} // namespace reality
} // namespace axn

#endif /* TerrainEdge_hpp */
