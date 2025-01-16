#ifndef Lighting_hpp
#define Lighting_hpp

#include "Geometry.hpp"
#include "Color.hpp"
#include "Drawing.hpp"
#include "LightSource.hpp"
#include "Visible.hpp"

namespace axn
{
namespace graphics
{

class Camera;

class Lighting : public Visible
{
public:
    virtual ~Lighting( ) { }

    Lighting( );

    bool darkness_active( ) const;
    void darkness_active( bool );
    dec darkness_intensity( ) const;
    void darkness_intensity( dec );

    const varray<LightSource> & light_sources( ) const;
    void add_light_source( LightSource cref );
    void add_light_sources( varray<LightSource> cref );
    void clear_light_sources( );

    Color cref ambient_color( ) const;
    void ambient_color( Color cref );

private:
    varray<LightSource> m_light_sources;

    bool m_darkness_active = false;
    Slider<dec> m_darkness_slider;

    Color m_ambient_color = TRANSPARENT;
};

} // namespace graphics
} // namespace axn

#endif /* Lighting_hpp */
