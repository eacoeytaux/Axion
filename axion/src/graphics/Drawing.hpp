#ifndef Drawing_hpp
#define Drawing_hpp

#include "axn.hpp"
#include "Geometry.hpp"
#include "Color.hpp"

namespace axn {
namespace graphics {

class Drawing {
    friend class Camera; // be friendly to the camera and smile
public:
    virtual ~Drawing( ) {}
    Drawing( const Coordinate & center = ORIGIN );

    uint polygon_count( ) const;

    Drawing & reserve( uint reserve_size );
    Drawing & reserve_more( uint reserve_size );
    Drawing & clear( bool reserve_mem = true );

    Coordinate center( ) const;
    Drawing & center( const Coordinate & );
    Drawing & move( const Vector & );
    Drawing & scale( ufloat, const Coordinate & = ORIGIN );
    Drawing & rotate( const Angle &, const Coordinate & = ORIGIN );
    Drawing & mirror( const Vector & );
    Drawing & mirror_x( ) {
        mirror( X_HAT );
        return *this;
    }
    Drawing & mirror_y( ) {
        mirror( Y_HAT );
        return *this;
    }

    Drawing & add( const Drawing & );
    Drawing & add( const varray<Color> & colors, const Shape & shape,
                   ufloat thickness = FILLED, bool preserve_thickness = false );
    Drawing & add( const Color & color, const Shape & shape, ufloat thickness = FILLED,
                   bool preserve_thickness = false );
    Drawing & add( const Color & color1, const Color & color2, const Line & line,
                   ufloat thickness = 1.f, bool preserve_thickness = false );
    Drawing & add( const Color & color, const Line & line, ufloat thickness = 1.f,
                   bool preserve_thickness = false );
#ifdef AXN_DEBUG
    Drawing & add( const Color & color, const Vector & vector, const ufloat arrow_head_length,
                   const ufloat thickness = 1.f, bool preserve_thickness = false );
#endif
    
    bool has_border( ) const { return ( m_border_width && m_border_color.a( ) ); }
    Color border_color( ) const { return m_border_color; }
    planc border_width( ) const { return m_border_width; }
    Drawing & border_color ( const Color & color ) { m_border_color = color; return *this; }
    Drawing & border_width( planc width ) { m_border_width = width; return *this; }
    Drawing & border ( const Color & color, planc width ) { border_color( color ); border_width( width ); return *this; }

    bool translucent( ) const;

private:
    struct ColoredShape {
        Shape shape;
        varray<Color> colors;
        ufloat thickness = FILLED;
        bool preserve_thickness = false;
    };

    Drawing & add_internal( const varray<Color> & colors, const Shape &, ufloat thickness,
                            bool preserve_thickness );

    const varray<ColoredShape> & colored_shapes( ) const;
    varray<ColoredShape> colored_shapes_border( ) const;

    Coordinate m_center;
    varray<ColoredShape> m_colored_shapes;
    varray<ColoredShape> m_colored_shapes_opaque;
    varray<ColoredShape> m_colored_shapes_translucent;
    varray<ColoredShape> m_colored_shapes_border;
    
    bool m_translucent = false;
    
    planc m_border_width;
    Color m_border_color;
};

} // namespace graphics
} // namespace axn

#endif /* Drawing_hpp */
