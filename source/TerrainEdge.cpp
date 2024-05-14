#include "TerrainEdge.hpp"

#include "TerrainVertex.hpp"

using axn::reality::TerrainEdge;

TerrainEdge::TerrainEdge( TerrainVertex * _v1, TerrainVertex * _v2, const double _resistance ) : m_v1( _v1 ), m_v2( _v2 ), m_resistance( _resistance )
{
    Assert( (bool)m_v1 );
    Assert( (bool)m_v2 );
}

TerrainEdge::TerrainEdge( const TerrainEdge & _edge )
{
    m_v1 = _edge.vertex1( );
    m_v2 = _edge.vertex2( );
}

Line TerrainEdge::line( ) const { return Line( m_v2->position( ), m_v1->position( ) ); }

Vector TerrainEdge::vector( ) const { return Vector( m_v1->position( ), m_v2->position( ) ); }

TerrainVertex * TerrainEdge::vertex1( ) const { return m_v1; }

TerrainVertex * TerrainEdge::vertex2( ) const { return m_v2; }

Angle TerrainEdge::normal( ) const { return vector( ).angle( ) + RIGHT_ANGLE; }

double TerrainEdge::resistance( ) const { return m_resistance; }
