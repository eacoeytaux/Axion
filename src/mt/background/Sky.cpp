#include "mt/background/Sky.hpp"

var_const( uint DAY_LENGTH = ( 2048 ) );

var_const( dec SUNRISE_START = ( 0.2 ) );
var_const( dec SUNRISE_END = ( 0.25 ) );
var_const( dec NOON = ( 0.5 ) );
var_const( dec SUNSET_START = ( 0.75 ) );
var_const( dec SUNSET_END = ( 0.8 ) );

var_const( Span<ticks> CLOUD_REGEN_RATE = { 1200.0 } ); // 0 for no clouds

var_const( Color MIDNIGHT_COLOR = ( BLACK ) );
var_const( Color SUNRISE_COLOR = ( Color::rgb( 0xFF7800 ) ) );
var_const( Color NOON_COLOR = ( BLUE ) );
var_const( Color SUNSET_COLOR = ( Color::rgb( 0xFF7800 ) ) );

var_const( ColorSlider SKY_MIDNIGHT_TO_SUNRISE = ( ColorSlider( MIDNIGHT_COLOR, SUNRISE_COLOR ) ) );
var_const( ColorSlider SKY_SUNRISE = ( ColorSlider( SUNRISE_COLOR, SUNRISE_COLOR ) ) );
var_const( ColorSlider SKY_SUNRISE_TO_NOON = ( ColorSlider( SUNRISE_COLOR, NOON_COLOR ) ) );
var_const( ColorSlider SKY_NOON = ( ColorSlider( NOON_COLOR, NOON_COLOR ) ) );
var_const( ColorSlider SKY_NOON_TO_SUNSET = ( ColorSlider( NOON_COLOR, SUNSET_COLOR ) ) );
var_const( ColorSlider SKY_SUNSET = ( ColorSlider( SUNSET_COLOR, SUNSET_COLOR ) ) );
var_const( ColorSlider SKY_SUNSET_TO_MIDNIGHT = ( ColorSlider( SUNSET_COLOR, MIDNIGHT_COLOR ) ) );

Sky::Sky( ptr<Room> in_room ) : Object( in_room )
{
    background( true );

    needs_render_always( true );

    no_gravity( );
    terrain_bound( false );

    z( 0.0 );

    if( CLOUD_REGEN_RATE.max( ) )
    {
        add_cloud( );
    }
    else
    {
        m_has_clouds = false;
    }
}

void Sky::render( )
{
    Object::render( );

    Color top_color;
    Color bottom_color;

    dec day = dec( age( ) % DAY_LENGTH ) / dec( DAY_LENGTH );

    if( day <= SUNRISE_START )
    {
        dec d = day / SUNRISE_START;
        top_color = SKY_MIDNIGHT_TO_SUNRISE.color_at( d );
    }
    else if( day <= SUNRISE_END )
    {
        dec d = ( day - SUNRISE_START ) / ( SUNRISE_END - SUNRISE_START );
        top_color = SKY_SUNRISE.color_at( d );
    }
    else if( day <= NOON )
    {
        dec d = ( day - SUNRISE_END ) / ( NOON - SUNRISE_END );
        top_color = SKY_SUNRISE_TO_NOON.color_at( d );
    }
    else if( day <= SUNSET_START )
    {
        dec d = ( day - NOON ) / ( SUNSET_START - NOON );
        top_color = SKY_NOON_TO_SUNSET.color_at( d );
    }
    else if( day <= SUNSET_END )
    {
        dec d = ( day - SUNSET_START ) / ( SUNSET_END - SUNSET_START );
        top_color = SKY_SUNSET.color_at( d );
    }
    else if( day <= 1.0 )
    {
        dec d = ( day - SUNSET_END ) / one_minus( SUNSET_END );
        top_color = SKY_SUNSET_TO_MIDNIGHT.color_at( d );
    }

    bottom_color = top_color;

    {
        top_color = BLUE;
        bottom_color = WHITE;
    }

    draw( { top_color,
            top_color,
            bottom_color,
            bottom_color },
          Polygon::rectangle( room( )->camera( )->width( ) + 2.0, room( )->camera( )->height( ) + 2.0, ORIGIN ) ); // added + 2 just in case
}

void Sky::update( )
{
    Object::update( );

    if( m_has_clouds )
    {
        if( m_cloud_timer.tick( ) )
        {
            add_cloud( );
        }
    }
}

ptr<Cloud> Sky::add_cloud( )
{
    m_has_clouds = true;
    m_cloud_timer.reset( Random::rdec( CLOUD_REGEN_RATE ) );

    ptr<Cloud> cloud = m_clouds.insert_back( new Cloud( room( ) ) );

    cloud->position( Vector( ( cloud->bounds( ).lower_bound_x( ) - half( cloud->drawing_bounds( ).width( ) ) ), ( 5000.0 ) ) ); // todo 5000

    return( room( )->add_object( cloud ) );
}
