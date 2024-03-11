#include "Terrain.hpp"

AXN_NAMESPACES
using axn::reality::Terrain;

const planc THICKNESS = 6;
const planc OUTLINE_THICKNESS = 3;
const planc DEPTH_LENGTH = 1000; // todo this should be more variable
const planc DEPTH_COLOR_LENGTH = 200;
const Color GRASS_COLOR_LIGHT = Color::rgb( 0x00C000 );
const Color GRASS_COLOR_DARK = Color::rgb( 0x008000 );
const Color DIRT_COLOR = Color::rgb( 0x803002 );

Terrain::~Terrain( ) {
    for_each( edge, m_edges ) delete edge;
    m_edges.clear( );
    for_each( vertex, m_vertices ) delete vertex;
    m_vertices.clear( );
}

Terrain::Terrain( World* world, const varray<Coordinate> & _vertices ) : Object( world ) {
    gravity_ratio( 0.f );

    // TODO use _vertices
    TerrainVertex* v0 = make_vertex( Coordinate( -800.f, 0.f ) );
    TerrainVertex* v1 = make_vertex( Coordinate( -100.f, 5.f ) );
    TerrainVertex* v2 = make_vertex( Coordinate( 100.f, 25.f ) );
    TerrainVertex* v3 = make_vertex( Coordinate( 200.f, 100.f ) );
    TerrainVertex* v4 = make_vertex( Coordinate( 350.f, 75.f ) );
    TerrainVertex* v5 = make_vertex( Coordinate( 500.f, 25.f ) );
    TerrainVertex* v6 = make_vertex( Coordinate( 800.f, 0.f ) );

    make_edge( v0, v1 );
    make_edge( v1, v2 );
    make_edge( v2, v3 );
    make_edge( v3, v4 );
    make_edge( v4, v5 );
    make_edge( v5, v6 );
}

const Terrain & Terrain::render( ) const {
    Object::render( );
    
    for_each( edge, edges( ) )
    {
        Shape dirt( {
            edge->vertex2( )->position( ),
            edge->vertex1( )->position( ),
            edge->vertex1( )->position( ) + Vector( 0, -DEPTH_COLOR_LENGTH ),
            edge->vertex2( )->position( ) + Vector( 0, -DEPTH_COLOR_LENGTH ) } );
        
        draw( { DIRT_COLOR, DIRT_COLOR, BLACK, BLACK }, dirt );
        
        dirt = Shape( {
            edge->vertex2( )->position( ) + Vector( 0, -DEPTH_COLOR_LENGTH ),
            edge->vertex1( )->position( ) + Vector( 0, -DEPTH_COLOR_LENGTH ),
            edge->vertex1( )->position( ) + Vector( 0, -DEPTH_LENGTH ),
            edge->vertex2( )->position( ) + Vector( 0, -DEPTH_LENGTH ) } );
        
        draw( BLACK, dirt );
    }

    // todo use drawing outline instead
    for_each( vertex, vertices( ) )
        draw( GRASS_COLOR_DARK,
             Shape::equilateral( 8, ( THICKNESS.half( ) ) + OUTLINE_THICKNESS, vertex->position( ) ) );
    for_each( edge, edges( ) )
        draw( GRASS_COLOR_DARK,
             edge->line( ), THICKNESS + ( OUTLINE_THICKNESS * 2.f ) );
    for_each( vertex, vertices( ) )
        draw( GRASS_COLOR_LIGHT,
             Shape::equilateral( 8, THICKNESS.half( ), vertex->position( ) ) );
    for_each( edge, edges( ) )
        draw( GRASS_COLOR_LIGHT,
             edge->line( ), THICKNESS );

    return *this;
}

TerrainVertex* Terrain::make_vertex( const Coordinate & _position ) {
    TerrainVertex* vertex( new TerrainVertex( _position ) );
    m_vertices.push_back( vertex );
    return vertex;
}

TerrainEdge* Terrain::make_edge( TerrainVertex* v1, TerrainVertex* v2 ) {
    TerrainEdge* edge( new TerrainEdge( v1, v2 ) );
    m_edges.push_back( edge );
    v1->edge2( edge );
    v2->edge1( edge );
    return edge;
}

Terrain & Terrain::update( ) {
    Object::update( );
    return *this;
}

const varray<TerrainVertex*> & Terrain::vertices( ) const { return m_vertices; }

const varray<TerrainEdge*> & Terrain::edges( ) const { return m_edges; }

#ifdef AXN_DEBUG
class TerrainDebugOverlay : public Visible {
private:
    const Terrain* m_terrain;

public:
    TerrainDebugOverlay( const Terrain* terrain ) { m_terrain = terrain; }

    const TerrainDebugOverlay & render( ) const override {
        Visible::render( );

        const ufloat LINE_THICKNESS = 1.f;
        const Color LINE_COLOR = MAGENTA;

        for_each( edge, m_terrain->edges( ) ) draw( LINE_COLOR, edge->line( ), LINE_THICKNESS );

        return *this;
    }
};

Drawing Terrain::debug_overlay( ) const {
    const ufloat LINE_THICKNESS = 1.f;
    const Color LINE_COLOR = MAGENTA;

    Drawing debug_overlay;
    for_each( edge, edges( ) )
        debug_overlay.add( LINE_COLOR, edge->line( ), LINE_THICKNESS );

    return debug_overlay;
}
#endif
