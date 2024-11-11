#include "Lighting.hpp"
#include "Camera.hpp"

namespace
{
const Color NIGHT_AMBIENT_COLOR = Color::rgba( 0.0, 0.0, 0.5, 0.2 );
#ifdef AXN_DEBUG
const dec LIGHT_DEBUG_MAX_INTENSITY = 0.25;
#endif
} // namespace

Lighting::Lighting( )
{
    m_darkness_slider.value( ONE );
    m_ambient_color = NIGHT_AMBIENT_COLOR;
}

bool Lighting::darkness_active( ) const
{
    return m_darkness_active;
}

void Lighting::darkness_active( const bool _darkness_active )
{
    m_darkness_active = _darkness_active;
}

dec Lighting::darkness_intensity( ) const
{
#ifdef AXN_DEBUG
    if( Debug::active )
    {
        return min( m_darkness_slider.value( ), LIGHT_DEBUG_MAX_INTENSITY );
    }
#endif
    return m_darkness_slider.value( );
}

void Lighting::darkness_intensity( const dec _intensity )
{
    m_darkness_slider.value( _intensity );
}

const varray<LightSource> & Lighting::light_sources( ) const
{
    return m_light_sources;
}

void Lighting::add_light_source( const LightSource & _light_source )
{
    m_light_sources.insert_back( _light_source );
}

void Lighting::clear_light_sources( )
{
    m_light_sources.clear( );
}

const Color & Lighting::ambient_color( ) const
{
    return m_ambient_color;
}

void Lighting::ambient_color( const Color & _ambient_color )
{
    m_ambient_color = _ambient_color;
}
