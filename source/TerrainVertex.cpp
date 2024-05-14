#include "TerrainVertex.hpp"

#include "TerrainEdge.hpp"

using axn::reality::TerrainVertex;

TerrainVertex::TerrainVertex( const Coordinate & _pos ) { m_pos = _pos; }

Coordinate TerrainVertex::position( ) const { return m_pos; }

TerrainEdge * TerrainVertex::edge1( ) const { return m_e1; }

TerrainEdge * TerrainVertex::edge2( ) const { return m_e2; }

TerrainVertex & TerrainVertex::edge1( TerrainEdge * e1 )
{
    Assert( (bool)!m_e1, "Edge has already been set" );
    m_e1 = e1;
    return *this;
}

TerrainVertex & TerrainVertex::edge2( TerrainEdge * e2 )
{
    Assert( (bool)!m_e2, "Edge has already been set" );
    m_e2 = e2;
    return *this;
}

Angle TerrainVertex::normal( ) const
{
    if( m_e1 && m_e2 )
    {
        return ( m_e1->normal( ) + m_e2->normal( ) ).half( ).flipped( );
    }
    else if( m_e1 )
    {
        return m_e1->normal( );
    }
    else if( m_e2 )
    {
        return m_e2->normal( );
    }

    return Angle( RIGHT_ANGLE );
}
