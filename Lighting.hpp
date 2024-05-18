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
    Lighting & darkness_active( bool );
    dec darkness_intensity( ) const;
    Lighting & darkness_intensity( dec );

    const varray<LightSource> & light_sources( ) const;
    Lighting & add_light_source( const LightSource & );
    Lighting & clear_light_sources( );

private:
    varray<LightSource> m_light_sources;

    bool m_darkness_active = false;
    Slider<dec> m_darkness_slider = Slider<dec>( 1.0 );
};

} // namespace graphics
} // namespace axn

#endif /* Lighting_hpp */
