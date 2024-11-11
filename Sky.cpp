#include "Sky.hpp"
#include "World.hpp"

using mtmercy::Sky;

namespace
{
const uint DAY_LENGTH = 2048;

const dec SUNRISE_START = 0.2;
const dec SUNRISE_END = 0.25;
const dec NOON = 0.5;
const dec SUNSET_START = 0.75;
const dec SUNSET_END = 0.8;

const Span<uint> CLOUD_REGEN_RATE = { 0, 0 }; // 0 for no clouds

const Color MIDNIGHT_COLOR = BLACK;
const Color SUNRISE_COLOR = Color::rgb( 0xFF7800 );
const Color NOON_COLOR = BLUE;
const Color SUNSET_COLOR = Color::rgb( 0xFF7800 );

const ColorSlider SKY_MIDNIGHT_TO_SUNRISE = ColorSlider( MIDNIGHT_COLOR, SUNRISE_COLOR );
const ColorSlider SKY_SUNRISE = ColorSlider( SUNRISE_COLOR, SUNRISE_COLOR );
const ColorSlider SKY_SUNRISE_TO_NOON = ColorSlider( SUNRISE_COLOR, NOON_COLOR );
const ColorSlider SKY_NOON = ColorSlider( NOON_COLOR, NOON_COLOR );
const ColorSlider SKY_NOON_TO_SUNSET = ColorSlider( NOON_COLOR, SUNSET_COLOR );
const ColorSlider SKY_SUNSET = ColorSlider( SUNSET_COLOR, SUNSET_COLOR );
const ColorSlider SKY_SUNSET_TO_MIDNIGHT = ColorSlider( SUNSET_COLOR, MIDNIGHT_COLOR );
} // namespace

Sky::Sky( World * world ) : Object( world )
{
    background( true );
    
    needs_render_always( true );
    
    gravity_ratio( ZERO );
    terrain_boundaries( false );

    z( 0.0 );

    m_clouds = varray<Cloud *>( );
    if( CLOUD_REGEN_RATE.max( ) )
    {
        m_has_clouds = true;
        m_cloud_timer.reset( Random::rint( CLOUD_REGEN_RATE ) );

        m_clouds.insert_back( new Cloud( world ) );
        world->add_object( m_clouds.back( ) );
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

    dec day = (dec)( age( ) % DAY_LENGTH ) / (dec)( DAY_LENGTH );

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
        dec d = ( day - SUNSET_END ) / ( 1.0 - SUNSET_END );
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
          Rectangle( world( )->camera( )->width( ) + 2.0, world( )->camera( )->height( ) + 2.0, ORIGIN ) ); // added + 2 just in case
}

void Sky::update( )
{
    Object::update( );

    if( m_has_clouds )
    {
        if( m_cloud_timer.tick( ) )
        {
            m_cloud_timer.reset( Random::rint( CLOUD_REGEN_RATE ) );
            m_clouds.insert_back( new Cloud( world( ) ) );
            world( )->add_object( m_clouds.back( ) );
        }
    }
}
