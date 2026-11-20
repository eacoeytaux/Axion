#include "mt/environment/water/WaterDrop.hpp"

#include "axn/world/Terrain.hpp"

var_const( span<Planc> RADIUS_LARGE = { 1.1, 1.2 } );
var_const( span<Planc> RADIUS_SMALL = { 0.5, 0.7 } );

var_const( span<uint> SPLASH_DROP_COUNT = { 3, 5 } );
var_const( span<Planc> SPLASH_DAMPEN = { 0.2, 0.3 } );
var_const( span<Angle> SPLASH_OFFSET = { ( RIGHT * 0.15 ), ( RIGHT * 0.4 ) } );

var_const( span<ticks> DRIP_WAIT_DEFAULT = { 180.0, 500.0 } );

var_const( ticks DRIP_DEBUG_TIME = ( 10 ) );

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

void WaterDrop::ground( Terrain::Bumper cref in_ground )
{
    Object::ground( in_ground );

    if( is_pos( age( ) ) )
    {
        if( in_ground.node( ) )
        {
            if( !m_splashed )
            {
                bool flipped = Random::rbool( );
                for_range( i, Random::rint( SPLASH_DROP_COUNT ) )
                {
                    room( )->add_object( new WaterDrop( room( ), position( ), VectorA( in_ground.node( )->normal( ) + negated( negated( Random::rAngle( SPLASH_OFFSET ), ( i % 2 ) ), flipped ), velocity( ).magnitude( ) * Random::rPlanc( SPLASH_DAMPEN ) ) ) );
                }
            }

            mark_deleted( );
        }
    }
}

WaterDropper::WaterDropper( ptr<Room> in_room, Point cref in_position, dec in_z ) : Object( in_room, in_position, in_z ), m_drip( DRIP_WAIT_DEFAULT )
{
    #if defined( AXN_DEBUG )
    draw_debug = true;
    #endif

    stationary( true );
    solid( false );
    interactive( false );

    m_drip.randomize( ); // C! start partway through so droppers don't all drip in sync
}

#if defined( AXN_DEBUG )
Drawing WaterDropper::debug_overlay( ) const
{
    Drawing debug_overlay;

    bool dripped = ( ( m_drip.duration( ) - m_drip.remaining( ) ) <= DRIP_DEBUG_TIME );
    debug_overlay.draw( dripped ? GREEN : RED, Circle( ) );

    return( debug_overlay );
}
#endif

void WaterDropper::update( )
{
    Object::update( );

    if( m_drip.tick_auto_reset( ) )
    {
        room( )->add_object( new WaterDrop( room( ), position( ), z( ) ) );
    }
}
