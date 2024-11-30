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
    
    Drawing & draw( const Color & color,
                    const Path & path,
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

    Drawing & override_color( const Color & );
    Drawing & clear_override_color( );
    
    Drawing & filter_function( const function<void( Color & )> & );
    Drawing & clear_filter_function( );

    Drawing & erase( const Polygon & );
    Drawing & add_bound( const Polygon & );
    Drawing & clear_bounds( );

    bool translucent( ) const;
    
    bool operator==( const Drawing & ) const { return false; }
    bool operator!=( const Drawing & ) const { return true; }

private:
    struct ColoredPolygon
    {
        Polygon polygon;
        varray<Color> colors;
        dec thickness = FILLED;
        bool preserve_thickness = false;
        bool extend_lines = false;
        bool opaque = true;

        bool fill = false;
        bool hole = false;
        bool reset = false;

        default_equal( ColoredPolygon );
    };

    const varray<ColoredPolygon> & colored_polygons( bool transformed = true ) const;
<<<<<<< HEAD

=======
    
>>>>>>> 12b1d6c248f2c345cf2a4a47ac760538a65c6906
    mutable varray<ColoredPolygon> m_colored_polygons;
    
    mutable Coordinate m_center;
    
    mutable FixedRectangle m_bounding_box;

    mutable bool m_translucent = false;
    
    bool m_filter_function_set = false;
    std::function<void( Color & )> m_filter_function = [ ]( Color & ) { };
};

} // namespace graphics
} // namespace axn

#endif /* Drawing_hpp */
