#include "Lighting.hpp"

#include "Camera.hpp"

Lighting::Lighting( ) { m_darkness_slider.value( 0 ); }

bool Lighting::darkness_active( ) const
{
    return m_darkness_active;
}

Lighting & Lighting::darkness_active( const bool _darkness_active )
{
    m_darkness_active = _darkness_active;
    return *this;
}

double Lighting::darkness_intensity( ) const
{
    return m_darkness_slider.value( );
}

Lighting & Lighting::darkness_intensity( const double _intensity )
{
    m_darkness_slider.value( _intensity );
    return *this;
}

const varray<LightSource> & Lighting::light_sources( ) const
{
    return m_light_sources;
}

Lighting & Lighting::add_light_source( const LightSource & _light_source )
{
    m_light_sources.insert_back( _light_source );
    return *this;
}

Lighting & Lighting::clear_light_sources( )
{
    m_light_sources.clear( );
    return *this;
}
