#ifndef Drawing_hpp
#define Drawing_hpp

#include "Color.hpp"
#include "Geometry.hpp"

namespace axn
{
namespace graphics
{

class Drawing
{
    friend class Camera; // be friendly to the camera and smile
public:
    virtual ~Drawing( ) { }
    Drawing( const Coordinate & center = ORIGIN );

    uint polygon_count( ) const;

    Drawing & reserve( uint reserve_size );
    Drawing & reserve_more( uint reserve_size );
    Drawing & clear( bool reserve_mem = true );

    Coordinate center( ) const;
    Drawing & center( const Coordinate & );
    Drawing & move( const Vector & );
    Drawing & scale( double, const Coordinate & = ORIGIN );
    Drawing & rotate( const Angle &, const Coordinate & = ORIGIN );
    Drawing & mirror( const Vector & );
    Drawing & mirror_x( ) { return mirror( X_HAT ); }
    Drawing & mirror_y( ) { return mirror( Y_HAT ); }

    Drawing & draw( const Drawing & );
    Drawing & draw( const varray<Color> & colors, const Polygon & polygon, double thickness = FILLED, bool preserve_thickness = false );
    Drawing & draw( const Color & color, const Polygon & polygon, double thickness = FILLED, bool preserve_thickness = false );
    Drawing & draw( const Color & color1, const Color & color2, const Line & line, double thickness = 1.0, bool preserve_thickness = false );
    Drawing & draw( const Color & color, const Line & line, double thickness = 1.0, bool preserve_thickness = false );
#ifdef AXN_DEBUG
    Drawing & draw( const Color & color, const Vector & vector, const double arrow_head_length, const double thickness = 1.0, bool preserve_thickness = false );
#endif

    bool has_border( ) const { return ( m_border_width && m_border_color.a( ) ); }
    Color border_color( ) const { return m_border_color; }
    Planc border_width( ) const { return m_border_width; }
    Drawing & border_color( const Color & color )
    {
        m_border_color = color;
        return *this;
    }
    Drawing & border_width( const Planc & width )
    {
        m_border_width = width;
        return *this;
    }
    Drawing & border( const Color & color, const Planc & width )
    {
        border_color( color );
        border_width( width );
        return *this;
    }

    bool translucent( ) const;

private:
    struct ColoredPolygon
    {
        Polygon polygon;
        varray<Color> colors;
        double thickness = FILLED;
        bool preserve_thickness = false;
        bool opaque = true;
    };

    Drawing & add_internal( const varray<Color> & colors, const Polygon &, double thickness, bool preserve_thickness );

    const varray<ColoredPolygon> & colored_polygons( ) const;
    varray<ColoredPolygon> colored_polygons_border( ) const;

    Coordinate m_center;
    varray<ColoredPolygon> m_colored_polygons;
    Transform m_transform; // TODO use this

    bool m_translucent = false;

    Planc m_border_width;
    Color m_border_color;
};

} // namespace graphics
} // namespace axn

#endif /* Drawing_hpp */
