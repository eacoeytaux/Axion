#include "axn/graphics/Lighting.hpp"
#include "axn/graphics/Camera.hpp"

var_const( Color NIGHT_AMBIENT_COLOR = Color::rgb( 0.0, 0.0, 0.5, 0.2 ) );

#if defined( AXN_DEBUG )
var_const( dec DARKNESS_MAX_INTENSITY = ( 0.25 ) );
#endif

Lighting::Lighting( )
{
    m_darkness_slider.value( 1.0 );
    m_ambient_color = NIGHT_AMBIENT_COLOR;
}

bool Lighting::darkness_active( ) const
{
    return( m_darkness_active );
}

void Lighting::darkness_active( bool in_darkness_active )
{
    m_darkness_active = in_darkness_active;
}

dec Lighting::darkness_intensity( ) const
{
    #if defined( AXN_DEBUG )
    if( Debug::active ) { return( min( m_darkness_slider.value( ), DARKNESS_MAX_INTENSITY ) ); }
    #endif

    return( m_darkness_slider.value( ) );
}

void Lighting::darkness_intensity( dec in_intensity )
{
    m_darkness_slider.value( in_intensity );
}

varray<LightSource> cref Lighting::light_sources( ) const
{
    return( m_light_sources );
}

LightSource cref Lighting::add_light_source( LightSource cref in_light_source )
{
    m_light_sources.insert_back( in_light_source );

    return( in_light_source );
}

varray<LightSource> cref Lighting::add_light_sources( varray<LightSource> cref in_light_sources )
{
    for_each( light_source, in_light_sources )
    {
        add_light_source( light_source );
    }

    return( in_light_sources );
}

void Lighting::clear_light_sources( )
{
    m_light_sources.clear( );
}

Color cref Lighting::ambient_color( ) const
{
    return( m_ambient_color );
}

void Lighting::ambient_color( Color cref in_ambient_color )
{
    m_ambient_color = in_ambient_color;
}
