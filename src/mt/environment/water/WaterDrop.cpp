#include "mt/environment/water/WaterDrop.hpp"

#include "axn/world/Terrain.hpp"

var_const( Span<Planc> RADIUS_LARGE = { 1.1, 1.2 } );
var_const( Span<Planc> RADIUS_SMALL = { 0.5, 0.7 } );

var_const( Span<uint> SPLASH_DROP_COUNT = { 3, 5 } );
var_const( Span<Planc> SPLASH_DAMPEN = { 0.2, 0.3 } );
var_const( Span<Angle> SPLASH_OFFSET = { ( RIGHT * 0.15 ), ( RIGHT * 0.4 ) } );

var_const( ColorSlider COLOR = ( ColorSlider( CYAN, WHITE ) ) );

WaterDrop::WaterDrop( ptr<Room> in_room, Point cref in_position, dec in_z ) : Object( in_room, in_position, in_z ), m_splashed( false )
{
    m_color = COLOR.color_start( );
    m_radius = Random::rPlanc( RADIUS_LARGE );
}

WaterDrop::WaterDrop( ptr<Room> in_room, Point cref in_position, Vector cref in_upward, dec in_z ) : Object( in_room, in_position, in_upward, in_z ), m_splashed( true )
{
    m_color = COLOR.color_at( Random::rdec( ) );
    m_radius = Random::rPlanc( RADIUS_SMALL );
}

void WaterDrop::render( )
{
    Object::render( );

    draw( m_color, Polygon::circle( m_radius ) );
}

void WaterDrop::ground( ptr<Terrain::Node> in_ground, Terrain::Bumper cref in_bumper )
{
    if( is_pos( age( ) ) )
    {
        if( in_ground )
        {
            mark_deleted( );

            if( !m_splashed )
            {
                bool flipped = Random::rbool( );
                for_range( i, Random::rint( SPLASH_DROP_COUNT ) )
                {
                    room( )->add_object( new WaterDrop( room( ), position( ), VectorA( in_ground->normal( ) + negated( negated( Random::rAngle( SPLASH_OFFSET ), ( i % 2 ) ), flipped ), velocity( ).magnitude( ) * Random::rPlanc( SPLASH_DAMPEN ) ) ) );
                }
            }
            else
            {
                mark_deleted( );
            }
        }
    }
}
