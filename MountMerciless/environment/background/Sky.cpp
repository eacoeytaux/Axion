#include "Sky.hpp"

#include "World.hpp"

using mtmercy::Sky;

namespace
{
const uint DAY_LENGTH = 2048;

const double SUNRISE_START = 0.2;
const double SUNRISE_END = 0.25;
const double NOON = 0.5;
const double SUNSET_START = 0.75;
const double SUNSET_END = 0.8;

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

Sky::Sky( World * world ) : Background( world )
{
    drawing_always_dirty( true );

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

const Sky & Sky::render( ) const
{
    Object::render( );

    Color top_color;
    Color bottom_color;

    double day = (double)( age( ) % DAY_LENGTH ) / (double)( DAY_LENGTH );

    if( day <= SUNRISE_START )
    {
        double d = day / SUNRISE_START;
        top_color = SKY_MIDNIGHT_TO_SUNRISE.color_at( d );
    }
    else if( day <= SUNRISE_END )
    {
        double d = ( day - SUNRISE_START ) / ( SUNRISE_END - SUNRISE_START );
        top_color = SKY_SUNRISE.color_at( d );
    }
    else if( day <= NOON )
    {
        double d = ( day - SUNRISE_END ) / ( NOON - SUNRISE_END );
        top_color = SKY_SUNRISE_TO_NOON.color_at( d );
    }
    else if( day <= SUNSET_START )
    {
        double d = ( day - NOON ) / ( SUNSET_START - NOON );
        top_color = SKY_NOON_TO_SUNSET.color_at( d );
    }
    else if( day <= SUNSET_END )
    {
        double d = ( day - SUNSET_START ) / ( SUNSET_END - SUNSET_START );
        top_color = SKY_SUNSET.color_at( d );
    }
    else if( day <= 1.0 )
    {
        double d = ( day - SUNSET_END ) / ( 1.0 - SUNSET_END );
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
          Shape::rectangle( world( )->active_camera( )->width( ) + 2.0, world( )->active_camera( )->height( ) + 2.0, ORIGIN ) ); // added + 2 just in case

    return *this;
}

Sky & Sky::update( )
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
    return *this;
}
