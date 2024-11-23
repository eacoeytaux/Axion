#ifndef Drawing_hpp
#define Drawing_hpp

#include "Color.hpp"
#include "Geometry.hpp"

namespace axn
{
namespace graphics
{

class Drawing : public Transformable
{
    friend class Camera; // be friendly to the camera and smile
public:
    virtual ~Drawing( ) { }
    Drawing( const Coordinate & center = ORIGIN );
    
    transform_functions( Drawing );

    uint polygon_count( ) const;

    void reserve( uint reserve_size );
    void reserve_more( uint reserve_size );
    void clear( bool reserve_mem = true );

    Coordinate center( ) const;
    Drawing & center( const Coordinate & );

    Drawing & draw( const Drawing & );

    Drawing & draw( const Drawing &,
                    const Color & color );

    Drawing & draw( const varray<Color> & colors,
                    const Polygon & polygon,
                    dec thickness = FILLED,
                    bool preserve_thickness = false,
                    bool extend_lines = false );

    Drawing & draw( const Color & color,
                    const Polygon & polygon,
                    dec thickness = FILLED,
                    bool preserve_thickness = false,
                    bool extend_lines = false );

    Drawing & draw( const Color & color1,
                    const Color & color2,
                    const Line & line,
                    dec thickness = ONE,
                    bool preserve_thickness = false,
                    bool extend_lines = false );

    Drawing & draw( const Color & color,
                    const Line & line,
                    dec thickness = ONE,
                    bool preserve_thickness = false,
                    bool extend_lines = false );

#ifdef AXN_DEBUG
    Drawing & draw( const Color & color,
                    const Vector & vector,
                    dec arrow_head_length,
                    dec thickness = ONE,
                    bool preserve_thickness = false );
#endif
    
    const FixedRectangle & bounding_box( ) const { return m_bounding_box; }

    Drawing & override_color( const Color & color );
    Drawing & clear_override_color( );
    
    Drawing & filter_function( const function<void( Color & )> & );
    Drawing & clear_filter_function( );
    
    bool has_border( ) const { return ( m_border_width && m_border_color.a( ) ); }
    Color border_color( ) const { return m_border_color; }
    void border_color( const Color & color ) { m_border_color = color; }

    Planc border_width( ) const { return m_border_width; }
    void border_width( const Planc & width ) { m_border_width = width; }

    void border( const Color & color, const Planc & width )
    {
        border_color( color );
        border_width( width );
    }

    bool translucent( ) const;
    
    bool operator==( const Drawing & drawing ) const { return false; }
    bool operator!=( const Drawing & drawing ) const { return true; }

private:
    struct ColoredPolygon
    {
        Polygon polygon;
        varray<Color> colors;
        dec thickness = FILLED;
        bool preserve_thickness = false;
        bool extend_lines = false;
        bool opaque = true;

        default_equal( ColoredPolygon );
    };

    void add_internal( const varray<Color> & colors, const Polygon &, dec thickness, bool preserve_thickness );

    const varray<ColoredPolygon> & colored_polygons( bool transformed = true ) const;
    varray<ColoredPolygon> colored_polygons_border( bool transformed = true ) const;
    
    mutable FixedRectangle m_bounding_box;

    mutable Coordinate m_center;
    mutable varray<ColoredPolygon> m_colored_polygons;

    mutable bool m_translucent = false;
    
    bool m_override_color_set = false;
    Color m_override_color;
    
    bool m_filter_function_set = false;
    std::function<void( Color & )> m_filter_function = [ ]( Color & ) { };

    Planc m_border_width;
    Color m_border_color;
};

} // namespace graphics
} // namespace axn

#endif /* Drawing_hpp */
