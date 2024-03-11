#ifndef Lighting_hpp
#define Lighting_hpp

#include "axn.hpp"
#include "Geometry.hpp"
#include "Color.hpp"
#include "Drawing.hpp"
#include "LightSource.hpp"
#include "Visible.hpp"

namespace axn {
namespace graphics {

class Camera;

class Lighting : public Visible {
public:
    virtual ~Lighting( ) {}
    Lighting( );

    const Lighting & render( Camera* ) const;

    Lighting & add_light_source( const LightSource & );
    Lighting & clear_light_sources( );

    bool darkness_active( ) const;
    Lighting & darkness_active( bool );
    ufloat darkness_intensity( ) const;
    Lighting & darkness_intensity( ufloat );

private:
    //const Lighting & render_lights( const RectangleFixed & window ) const;
    //const Lighting & render_darkness( const RectangleFixed & window ) const;
    //
    //const Lighting & fill_boundary_dark( const RectangleFixed & boundary ) const;
    //const Lighting & fill_boundary_clear( const RectangleFixed & boundary,
    //                                      const LightSource & source ) const;
    //const Lighting & fill_boundary_edge( const RectangleFixed & boundary,
    //                                     const LightSource & source,
    //                                     const Quadrant & quadrant ) const;
    //const Lighting & fill_boundary_recursive( const RectangleFixed & boundary,
    //                                          const varray<LightSource> & possible_sources,
    //                                          const varray<LightSource> & sources = { } ) const;

    varray<LightSource> m_light_sources;
    bool m_darkness_active = false;
    Slider<float> m_darkness_slider = Slider<float>( 1 );
};

} // namespace graphics
} // namespace axn

#endif /* Lighting_hpp */
