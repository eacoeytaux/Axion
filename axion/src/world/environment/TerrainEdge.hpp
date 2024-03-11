#ifndef TerrainEdge_hpp
#define TerrainEdge_hpp

#include "axn.hpp"
#include "Geometry.hpp"

namespace axn {
namespace reality {

class TerrainVertex;
class TerrainEdge {
public:
    virtual ~TerrainEdge( ) {}
    TerrainEdge( TerrainVertex* v1, TerrainVertex* v2, ufloat resistance = 0.1f );
    TerrainEdge( const TerrainEdge & );

    Line line( ) const;
    TerrainVertex* vertex1( ) const;
    TerrainVertex* vertex2( ) const;

    ufloat resistance( ) const;

private:
    TerrainVertex* m_v1 = nullptr;
    TerrainVertex* m_v2 = nullptr;

    ufloat m_resistance;
};

} // namespace reality
} // namespace axn

#endif /* TerrainEdge_hpp */
