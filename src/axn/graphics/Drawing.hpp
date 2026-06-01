#ifndef Drawing_hpp
#define Drawing_hpp

#include "axn/graphics/Color.hpp"
#include "axn/geometry/Geometry.hpp"

namespace axn
{
namespace graphics
{

var_const( dec FILLED = ( 0.0 ) );

class Drawing : public Transformable
{

    friend class Camera; // be friendly to the camera and smile

public:

    Drawing( ) { }

    Drawing( Point cref in_center ) { Drawing::center( in_center ); }

    virtual Drawing ref transform( Transform cref in_trans ) override;

    transform_functions( Drawing );

    Drawing ref draw( Drawing cref in_drawing );

    Drawing ref draw( Drawing cref in_drawing,
                      Color cref in_color );

    Drawing ref draw( varray<Color> cref in_colors,
                      Polygon cref in_polygon,
                      dec in_thickness = FILLED,
                      bool in_preserve_thickness = false,
                      bool in_extend_lines = false );

    Drawing ref draw( Color cref in_color,
                      Polygon cref in_polygon,
                      dec in_thickness = FILLED,
                      bool in_preserve_thickness = false,
                      bool in_extend_lines = false );

    Drawing ref draw( Color cref in_color1,
                      Color cref in_color2,
                      Line cref in_line,
                      dec in_thickness = 1.0,
                      bool in_preserve_thickness = false,
                      bool in_extend_lines = false );

    Drawing ref draw( Color cref in_color,
                      Line cref in_line,
                      dec in_thickness = 1.0,
                      bool in_preserve_thickness = false,
                      bool in_extend_lines = false );

    Drawing ref draw( Color cref in_color,
                      Path cref in_path,
                      dec in_thickness = 1.0,
                      bool in_preserve_thickness = false,
                      bool in_extend_lines = false );

    #if defined( AXN_DEBUG )
    Drawing ref draw( Color cref in_color,
                      Vector cref in_vector,
                      dec in_arrow_head_length,
                      dec in_thickness = 1.0,
                      bool in_preserve_thickness = false );
    #endif

    Drawing ref add_hole( Polygon cref in_hole ); // nothing will be drawn in the hole
    Drawing ref add_bounds( Polygon cref in_bounds ); // nothing will be drawn out of bounds

    Drawing ref clear_bounds( ); // todo this should go away because erase and crop only affect before it

    Drawing ref color_filter( function<void( Color ref in_color )> cref in_color_filter ) { m_color_filter = in_color_filter; m_color_filter_set = true; rethis; }
    Drawing ref clear_color_filter( ) { m_color_filter_set = false; rethis; }

    bool opaque( ) const { return( m_opaque ); }

    Point center( ) const { return( m_center ); }
    Drawing ref center( Point cref in_center ) { m_center = in_center; rethis; }

    FixedRectangle cref bounding_box( ) const { return( m_bounding_box ); }

    void clear( bool in_reserve_mem = true )
    {
        Transformable::clear_transform( );

        m_nodes.clear( !in_reserve_mem );

        m_bounding_box.width( 0.0 );
        m_bounding_box.height( 0.0 );
        m_bounding_box.center( ORIGIN );

        m_opaque = true;

        clear_color_filter( );
    }

    void optimize( ); // should only be called when a drawing is reused // todo make use of this

    bool operator==( Drawing cref in_drawing ) const { return( false ); }
    bool operator!=( Drawing cref in_drawing ) const { return( true ); }

public:

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

    struct Node
    {

        Transform transform = IDENTITY_TRANSFORM; // first thing applied

        varray<ColoredPolygon> crop_shapes; // non-ordered
        varray<ColoredPolygon> hole_shapes; // non-ordered

        varray<ColoredPolygon> colored_shapes; // LIFO

        vvarray<Node> drawings; // LIFO

        default_equal( Node );

    };

    varray<Node> nodes( ) const;

protected:

    Node ref back_node( bool in_add_new = false );

private:

    mutable varray<Node> m_nodes;

    mutable Point m_center = ORIGIN;

    mutable FixedRectangle m_bounding_box;

    mutable bool m_opaque = true;

    bool m_color_filter_set = false;
    function<void( Color ref in_color )> m_color_filter = [ ] ( Color ref in_color ) { };

};

} // namespace graphics
} // namespace axn

#endif /* Drawing_hpp */
