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

    LightSource( ) { }
    LightSource( Point cref in_source, Planc cref in_distance );

    Point cref position( ) const;
    void position( Point cref in_pos );

    Planc radius( ) const;
    void radius( Planc cref in_radius );

    // dec flicker( ) const;
    // void flicker( dec in_flicker );

    // Color cref tint( ) const;
    // void tint( Color cref in_color );

    default_equal( LightSource );

private:

    Point m_position;
    Planc m_radius;

    // dec m_flicker;
    // color m_tint;

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
    Slider<dec> m_darkness_slider;

    Color m_ambient_color = TRANSPARENT;

};

} // namespace graphics
} // namespace axn

#endif /* Lighting_hpp */
