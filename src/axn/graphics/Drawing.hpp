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

    //friend class Camera; // be friendly to the camera and smile

public:

    Drawing( ) { }

    Drawing( Point cref in_center ) { Drawing::center( in_center ); }

    virtual Drawing ref transform( Transform cref in_trans ) override;

    transform_functions( Drawing );

    Drawing ref draw( Drawing cref in_drawing );

    Drawing ref draw( Drawing cref in_drawing,
                      Color cref in_color );

    Drawing ref draw( varray<Color> cref in_colors,
                      Polygon cref in_poly,
                      dec in_thickness = FILLED,
                      bool in_preserve_thickness = false,
                      bool in_extend_lines = false );

    Drawing ref draw( Color cref in_color,
                      Polygon cref in_poly,
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

    Drawing ref color_filter( function<void( Color ref in_color )> cref in_color_filter ) { m_color_filter = in_color_filter; m_color_filter_set = true; rethis; }
    Drawing ref clear_color_filter( ) { m_color_filter_set = false; rethis; }

    bool opaque( ) const { return( m_opaque ); }

    // C! how see-through the whole drawing is when it's rendered (1 is as drawn, 0 is invisible), applied to it as one: where one of its own
    // C! opaque polygons covers another, the covered part stays hidden as it fades, while what a see-through polygon covers still shows.
    // C! Use it to fade a whole object rather than lowering each color's alpha. Only a drawing that's rendered itself (a Visible) uses it,
    // C! not one drawn into another
    declmp opacity( ) const { return( m_opacity ); }
    Drawing ref opacity( declmp in_opacity ) { m_opacity = in_opacity; rethis; }

    bool empty( ) const; // C! nothing drawn: no polygons in any node or nested drawing

    Point cref center( ) const { return( m_center ); }
    Drawing ref center( Point cref in_center ) { m_center = in_center; rethis; }

    // C! the box around everything drawn, after the drawing's transforms and cut down by its crops (a point at the origin when empty)
    // C! worked out again from the polygons the first time it's asked for after a change
    FixedRectangle cref bounding_box( ) const;

    // C! the union of every box bounding_box( ) has had, the current one included: it never shrinks (clear( ) keeps it), so a drawing that
    // C! changes between frames stays inside it, and Room::render( ) uses it to decide what's worth redrawing
    FixedRectangle cref max_bounding_box( ) const;

    void clear( bool in_reserve_mem = true );

    // C! packs the drawing into as few nodes as possible without changing how it looks, and drops crops and holes that affect nothing
    // C! it's slow, so call it once (while loading) on drawings that get rendered every frame, not every frame
    // C! it keeps the look when the drawing is rendered or drawn into a drawing without crops or holes; inside one with crops, a crop no longer
    // C! moves with this drawing's own transforms (the renderer redraws crops under each inner node's transform)
    void optimize( );

    struct ColoredPolygon
    {

        Polygon poly;

        varray<Color> colors;

        dec thickness = FILLED;
        bool preserve_thickness = false;
        bool extend_lines = false;

        bool opaque = true;

        bool bakeable( Transform cref in_transform ) const; // C! whether in_transform can go into the points without changing how the polygon is drawn

        equal_default( ColoredPolygon );

    };

    class Nodes;

    struct Node
    {

        Transform transform = IDENTITY_TRANSFORM; // first thing applied

        varray<Polygon> crop_polys; // non-ordered
        varray<Polygon> hole_polys; // non-ordered

        varray<ColoredPolygon> colored_polys; // LIFO

        varray<Nodes> drawings; // LIFO

        bool transform_only( ) const { return( crop_polys.empty( ) && hole_polys.empty( ) && colored_polys.empty( ) && drawings.empty( ) ); } // C! nothing but a transform
        bool empty( ) const { return( transform_only( ) && transform.is_identity( ) ); } // C! does nothing at all

        equal_default( Node );

    };

    // C! a drawing's nodes, or a nested drawing's: everything Drawing works out by walking through them, nested drawings included
    class Nodes : public varray<Node>
    {

    public:

        Nodes( ) { }
        Nodes( varray<Node> cref in_nodes ) : varray<Node>( in_nodes ) { }

        uint node_count( ) const;

        bool draws_nothing( ) const; // C! no polygons in any node or nested drawing (empty( ) is still whether there are no nodes)

        bool has_crops_or_holes( ) const;

        bool all_identity( ) const; // C! every transform is the identity

        // C! sets every polygon to one color when in_set_color is true, runs the filter over the colors when in_use_filter is true,
        // C! and updates each polygon's opaque flag; returns whether every polygon is opaque
        bool recolor( bool in_set_color, Color cref in_color, bool in_use_filter, function<void( Color ref in_color )> cref in_filter );

        // C! adds the box around these nodes' polygons to in_bounds (in_any says whether it has anything in it yet), see Drawing.cpp
        void add_bounding_box( Transform cref in_outer, varray<FixedRectangle> in_crops, bool ref in_any, FixedRectangle ref in_bounds ) const;

        // C! -- the steps of Drawing::optimize( ), see Drawing.cpp -- //

        bool add_polygon_bounds( varray<FixedRectangle> ref in_poly_bounds ) const;

        void remove_unused_crops_and_holes( varray<FixedRectangle> cref in_poly_bounds, bool ref in_crops_hold_everything );
        void remove_crops( );

        void merge( bool in_crop_free_context );

        Nodes optimized_crop_free( ) const;

    private:

        struct TransformedColoredPolygon
        {

            ColoredPolygon colored_poly;
            Transform transform = IDENTITY_TRANSFORM;

        };

        void flatten( uint in_count, Transform cref in_outer, varray<TransformedColoredPolygon> ref in_transformed_polys ) const;

        static Nodes rebuilt( varray<TransformedColoredPolygon> cref in_polys );

    };

    // C! a temporary drawing hands its nodes over by value, so for_each( node, object.debug_overlay( ).nodes( ) ) doesn't loop over a destroyed drawing
    Nodes cref nodes( ) cref;
    Nodes nodes( ) &&;

    equal_never( Drawing );

protected:

    Node ref back_node( bool in_add_new = false );

private:

    mutable Nodes m_nodes;

    mutable Point m_center = ORIGIN;

    mutable FixedRectangle m_bounding_box;
    mutable bool m_bounding_box_dirty = false;

    mutable FixedRectangle m_max_bounding_box;
    mutable bool m_max_bounding_box_set = false; // C! false until something has been drawn

    mutable bool m_opaque = true;

    declmp m_opacity = 1.0;

    bool m_color_filter_set = false;
    function<void( Color ref in_color )> m_color_filter = [ ] ( Color ref in_color ) { };

};

} // namespace graphics
} // namespace axn

#endif /* Drawing_hpp */
