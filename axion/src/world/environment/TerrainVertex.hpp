#ifndef TerrainVertex_hpp
#define TerrainVertex_hpp

#include "axn.hpp"
#include "Geometry.hpp"

namespace axn {
namespace reality {

class TerrainEdge;
class TerrainVertex {
public:
    virtual ~TerrainVertex( ) {}
    TerrainVertex( const Coordinate & pos );

    Coordinate position( ) const;
    TerrainEdge* edge1( ) const;
    TerrainEdge* edge2( ) const;

    TerrainVertex & edge1( TerrainEdge* );
    TerrainVertex & edge2( TerrainEdge* );

private:
    Coordinate m_pos;
    TerrainEdge* m_e1 = nullptr;
    TerrainEdge* m_e2 = nullptr;
};

} // namespace reality
} // namespace axn

#endif /* TerrainVertex_hpp */
