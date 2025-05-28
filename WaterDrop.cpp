#include "WaterDrop.hpp"

#include "Terrain.hpp"

using mtmercy::WaterDrop;

namespace
{

const Span<Planc> RADIUS_LARGE = { 1.1, 1.2 };
const Span<Planc> RADIUS_SMALL = { 0.5, 0.7 };

const Span<uint> SPLASH_DROP_COUNT = { 3, 5 };
const Span<Planc> SPLASH_DAMPEN = { 0.2, 0.3 };
const Span<Angle> SPLASH_OFFSET = { ( RIGHT * 0.15 ), ( RIGHT * 0.4 ) };

const uint DROP_LARGE_EDGE_COUNT = 8;
const uint DROP_SMALL_EDGE_COUNT = 6;

cColorSlider COLOR = ColorSlider( CYAN, WHITE );

} // namespace

WaterDrop::WaterDrop( Room * room, Coordinate cref _position, const dec _z ) : Object( room, _position, _z ), m_splashed( false )
{
    m_color = COLOR.color_start( );
    m_radius = Random::rPlanc( RADIUS_LARGE );
}

WaterDrop::WaterDrop( Room * room, Coordinate cref _position, Vector cref _upward, const dec _z ) : Object( room, _position, _upward, _z ), m_splashed( true )
{
    m_color = COLOR.color_at( Random::rdec( ) );
    m_radius = Random::rPlanc( RADIUS_SMALL );
}

void WaterDrop::render( )
{
    Object::render( );
    
    draw( m_color, Polygon::equilateral( m_splashed ? DROP_SMALL_EDGE_COUNT : DROP_LARGE_EDGE_COUNT, m_radius ) );
}

void WaterDrop::ground( TerrainEdge * edge )
{
    if( is_pos( age( ) ) )
    {
        Object::ground( edge );
        
        if( edge )
        {
            mark_deleted( );
            
            if( !m_splashed )
            {
                bool flipped = Random::rbool( );
                for_range( i, Random::rint( SPLASH_DROP_COUNT ) )
                {
                    room( )->add_object( new WaterDrop( room( ), position( ), VectorA( edge->normal( ) + negate( negate( Random::rAngle( SPLASH_OFFSET ), ( i % 2 ) ), flipped ), velocity( ).magnitude( ) * Random::rPlanc( SPLASH_DAMPEN ) ) ) );
                }
            }
        }
    }
}
