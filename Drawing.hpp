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

    Drawing( ) { }

    Drawing( Coordinate cref center ) { Drawing::center( center ); }

    virtual Drawing & transform( Transform cref t ) override;

    transform_functions( Drawing );

    Drawing & draw( Drawing cref );

    Drawing & draw( Drawing cref,
                    Color cref color );

    Drawing & draw( const varray<Color> & colors,
                    Polygon cref polygon,
                    dec thickness = FILLED,
                    bool preserve_thickness = false,
                    bool extend_lines = false );

    Drawing & draw( Color cref color,
                    Polygon cref polygon,
                    dec thickness = FILLED,
                    bool preserve_thickness = false,
                    bool extend_lines = false );

    Drawing & draw( Color cref color1,
                    Color cref color2,
                    Line cref line,
                    dec thickness = 1.0,
                    bool preserve_thickness = false,
                    bool extend_lines = false );

    Drawing & draw( Color cref color,
                    Line cref line,
                    dec thickness = 1.0,
                    bool preserve_thickness = false,
                    bool extend_lines = false );

    Drawing & draw( Color cref color,
                    Path cref path,
                    dec thickness = 1.0,
                    bool preserve_thickness = false,
                    bool extend_lines = false );

    #if defined ( AXN_DEBUG )
    Drawing & draw( Color cref color,
                    Vector cref vector,
                    dec arrow_head_length,
                    dec thickness = 1.0,
                    bool preserve_thickness = false );
    #endif

    Drawing & erase( Polygon cref );
    Drawing & crop( Polygon cref );

    Drawing & clear_bounds( ); // todo this should go away because erase and crop only affect before it

    Drawing & filter_function( const function<void( Color & )> & filter_function ) { m_filter_function = filter_function; m_filter_function_set = true; rethis; }
    Drawing & clear_filter_function( ) { m_filter_function_set = false; rethis; }

    bool opaque( ) const { return m_opaque; }

    uint polygon_count( ) const { return m_colored_polygons.size( ); }

    Coordinate center( ) const { return m_center; }
    Drawing & center( Coordinate cref center ) { m_center = center; rethis; }

    FixedRectangle cref bounding_box( ) const { return m_bounding_box; }

    void reserve( cuint reserve_size ) { m_colored_polygons.reserve( reserve_size ); }
    void reserve_more( cuint reserve_size ) { m_colored_polygons.reserve_more( reserve_size ); }

    void clear( bool reserve_mem = true )
    {
        Transformable::clear_transform( );

        m_colored_polygons.clear( !reserve_mem );

        m_nodes.clear( !reserve_mem );

        m_bounding_box.width( 0.0 );
        m_bounding_box.height( 0.0 );
        m_bounding_box.center( ORIGIN );

        m_opaque = true;

        clear_filter_function( );
    }

    bool operator==( Drawing cref ) const { return false; }
    bool operator!=( Drawing cref ) const { return true; }

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

    const varray<ColoredPolygon> & colored_polygons( bool transformed = true ) const;

    struct Node
    {

        Transform transform = IDENTITY_TRANSFORM; // first thing applied

        varray<ColoredPolygon> crop_shapes; // non-ordered
        varray<ColoredPolygon> hole_shapes; // non-ordered

        varray<ColoredPolygon> colored_shapes; // LIFO

        varray<varray<Node>> drawings; // LIFO

        default_equal( Node );

    };

    varray<Node> nodes( ) const;

protected:

    Node & back_node( bool add_new = false );

private:

    mutable varray<Node> m_nodes;

    mutable varray<ColoredPolygon> m_colored_polygons;

    mutable Coordinate m_center = ORIGIN;

    mutable FixedRectangle m_bounding_box;

    mutable bool m_opaque = true;

    bool m_filter_function_set = false;
    function<void( Color & )> m_filter_function = [ ] ( Color & ) { };

};

} // namespace graphics
} // namespace axn

#endif /* Drawing_hpp */
