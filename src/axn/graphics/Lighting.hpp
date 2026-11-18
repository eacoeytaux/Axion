#ifndef Lighting_hpp
#define Lighting_hpp

#include "axn/geometry/Geometry.hpp"
#include "axn/graphics/Color.hpp"

namespace axn
{
namespace graphics
{

class LightSource
{

public:

    LightSource( Point cref in_position, Planc cref in_radius, dec in_flicker = 0.0 ) { position( in_position ); radius( in_radius ); flicker( in_flicker ); }
    LightSource( Point cref in_position, Planc cref in_radius, Color cref in_tint, dec in_flicker = 0.0 ) { position( in_position ); radius( in_radius ); tint( in_tint ); flicker( in_flicker ); }

    Point cref position( ) const { return( m_position ); }
    void position( Point cref in_position ) { m_position = in_position; }

    Planc cref radius( ) const { return( m_radius ); }
    void radius( Planc cref in_radius ) { m_radius = in_radius; }

    Color cref tint( ) const { return( m_tint ); }
    void tint( Color cref in_color ) { m_tint = in_color; }

    dec flicker( ) const { return( m_flicker ); }
    void flicker( dec in_flicker ) { m_flicker = in_flicker; }

    equal_default( LightSource );

private:

    Point m_position = ORIGIN;
    Planc m_radius = P0;

    Color m_tint;

    dec m_flicker = 0.0;

};

class Lighting
{

public:

    Lighting( );

    bool darkness_active( ) const;
    void darkness_active( bool in_active );

    dec darkness_intensity( ) const;
    void darkness_intensity( dec in_intensity );

    varray<LightSource> cref light_sources( ) const;
    varray<LightSource> cref add_light_sources( varray<LightSource> cref in_sources );
    LightSource cref add_light_source( LightSource cref in_source );
    void clear_light_sources( );

    Color cref ambient_color( ) const;
    void ambient_color( Color cref in_color );

private:

    varray<LightSource> m_light_sources;

    bool m_darkness_active = false;
    slider<dec> m_darkness_slider;

    Color m_ambient_color = CLEAR;

};

} // namespace graphics
} // namespace axn

#endif /* Lighting_hpp */
