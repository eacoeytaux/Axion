#include "axn/graphics/Drawing.hpp"

Drawing ref Drawing::transform( Transform cref in_trans )
{
    skip_override;

    back_node( ).transform.chain( in_trans );

    m_bounding_box_dirty = true; // C! the box moves with the polygons

    rethis;
}

Drawing ref Drawing::draw( Drawing cref in_drawing )
{
    Nodes nodes = in_drawing.nodes( );

    if( m_color_filter_set )
    {
        // C! the filter changes the colors of everything in the nested drawing too, which can change whether it's opaque
        m_opaque = ( nodes.recolor( false, CLEAR, true, m_color_filter ) && m_opaque );
    }
    else
    {
        m_opaque = ( m_opaque && in_drawing.opaque( ) );
    }

    m_bounding_box_dirty = true;

    back_node( !back_node( ).transform.is_identity( ) ||
               !back_node( ).colored_polys.empty( ) ).drawings.insert_back( nodes );

    rethis;
}

Drawing ref Drawing::draw( Drawing cref in_drawing, Color cref in_color )
{
    // C! draws the drawing's polygons all in in_color (a silhouette), then through this drawing's color filter if one is set
    Nodes nodes = in_drawing.nodes( );

    m_opaque = ( nodes.recolor( true, in_color, m_color_filter_set, m_color_filter ) && m_opaque );

    m_bounding_box_dirty = true;

    back_node( !back_node( ).transform.is_identity( ) ||
               !back_node( ).colored_polys.empty( ) ).drawings.insert_back( nodes );

    rethis;
}

Drawing ref Drawing::draw( varray<Color> cref in_colors,
                           Polygon cref in_poly,
                           dec in_thickness,
                           bool in_preserve_thickness,
                           bool in_extend_lines )
{
    rethis_if( !( in_poly.sides( ) ) );

    Assert( !is_neg( in_thickness ) );
    Assert( in_colors.size( ) );

    // C! added to the node first and filled in there, rather than filled in a local and copied in. After a nested drawing it starts the next node, to keep the order
    ColoredPolygon ref colored_poly = back_node( !back_node( ).transform.is_identity( ) ||
                                                    !back_node( ).drawings.empty( ) ).colored_polys.insert_back( );

    colored_poly.poly = in_poly;

    colored_poly.thickness = in_thickness;
    colored_poly.preserve_thickness = in_preserve_thickness;
    colored_poly.extend_lines = in_extend_lines;

    colored_poly.colors = in_colors;

    colored_poly.opaque = true;
    for_range( i, colored_poly.colors.size( ) )
    {
        if( m_color_filter_set )
        {
            m_color_filter( colored_poly.colors[ i ] );
        }

        if( !colored_poly.colors[ i ].is_opaque( ) )
        {
            colored_poly.opaque = false;

            break_if( !m_color_filter_set );
        }
    }

    m_bounding_box_dirty = true;

    rethis;
}

Drawing ref Drawing::draw( Color cref in_color,
                           Polygon cref in_poly,
                           dec in_thickness,
                           bool in_preserve_thickness,
                           bool in_extend_lines )
{
    return( draw( varray<Color>( max<uint>( 1, in_poly.sides( ) ), in_color ), in_poly, in_thickness, in_preserve_thickness, in_extend_lines ) );
}

Drawing ref Drawing::draw( Color cref in_color1,
                           Color cref in_color2,
                           Line cref in_line,
                           dec in_thickness,
                           bool in_preserve_thickness,
                           bool in_extend_lines )
{
    return( draw( { in_color1, in_color2 }, Polygon( varray<Point>( { in_line.pt1( ), in_line.pt2( ) } ) ), in_thickness, in_preserve_thickness, in_extend_lines ) );
}

Drawing ref Drawing::draw( Color cref in_color,
                           Line cref in_line,
                           dec in_thickness,
                           bool in_preserve_thickness,
                           bool in_extend_lines )
{
    return( draw( in_color, in_color, in_line, in_thickness, in_preserve_thickness, in_extend_lines ) );
}

Drawing ref Drawing::draw( Color cref in_color,
                           Path cref in_path,
                           dec in_thickness,
                           bool in_preserve_thickness,
                           bool in_extend_lines )
{
    for_each( line, in_path.lines( ) )
    {
        draw( in_color, line, in_thickness, in_preserve_thickness, in_extend_lines );
    }

    rethis;
}

#if defined( AXN_DEBUG )
Drawing ref Drawing::draw( Color cref in_color,
                           Vector cref in_vector,
                           dec in_arrow_head_length,
                           dec in_thickness,
                           bool in_preserve_thickness )
{
    draw( in_color, Line( in_vector.origin( ), in_vector.destination( ) ), in_thickness, in_preserve_thickness );
    draw( in_color, Line( in_vector.destination( ), in_vector.destination( ) - VectorA( in_vector.angle( ) + ( half( RIGHT ) ), in_arrow_head_length ) ), in_thickness, in_preserve_thickness, true );
    draw( in_color, Line( in_vector.destination( ), in_vector.destination( ) - VectorA( in_vector.angle( ) - ( half( RIGHT ) ), in_arrow_head_length ) ), in_thickness, in_preserve_thickness, true );

    rethis;
}
#endif

Drawing ref Drawing::add_hole( Polygon cref in_poly )
{
    back_node( !back_node( ).transform.is_identity( ) ).hole_polys.insert_back( in_poly );

    rethis;
}

Drawing ref Drawing::add_bounds( Polygon cref in_poly )
{
    m_bounding_box_dirty = true; // C! the crop limits the box to what can still be seen, see bounding_box( )

    back_node( !back_node( ).transform.is_identity( ) ).crop_polys.insert_back( in_poly );

    rethis;
}

bool Drawing::empty( ) const
{
    return( m_nodes.draws_nothing( ) );
}

FixedRectangle cref Drawing::bounding_box( ) const
{
    if( m_bounding_box_dirty )
    {
        bool any = false;
        FixedRectangle bounds;

        m_nodes.add_bounding_box( IDENTITY_TRANSFORM, { }, any, bounds );

        // C! TODO the box is now only what's drawn (it used to always include the local origin), so an object drawn away from its position can be culled sooner: check nothing relied on the origin being inside
        m_bounding_box = ( any ? bounds : FixedRectangle( ) );
        m_bounding_box_dirty = false;

        if( any )
        {
            if( m_max_bounding_box_set )
            {
                m_max_bounding_box.union_with( bounds );
            }
            else
            {
                m_max_bounding_box = bounds;
                m_max_bounding_box_set = true;
            }
        }
    }

    return( m_bounding_box );
}

FixedRectangle cref Drawing::max_bounding_box( ) const
{
    bounding_box( ); // C! brings the current drawing into it first

    return( m_max_bounding_box );
}

void Drawing::clear( bool in_reserve_mem )
{
    Transformable::clear_transform( );

    m_nodes.clear( !in_reserve_mem );

    m_bounding_box.width( 0.0 );
    m_bounding_box.height( 0.0 );
    m_bounding_box.center( ORIGIN );
    m_bounding_box_dirty = false;

    m_opaque = true;

    clear_color_filter( );
}

// C! -- optimize( ) -- //

// C! how the camera renders a node list (render_nodes in Camera.cpp), which optimize( ) must not change the look of:
// C!   - the last node is rendered outermost: its transform, crops and holes also apply to every node before it, then its polygons are drawn, then its nested drawings
// C!   - each node sets the stencil from all the crops and holes so far, drawn under its own matrix, and doesn't restore it afterwards,
// C!     so a crop also clips polygons drawn after it (AspineTree's trunk marks and Waterfall's ripples rely on this)
// C! drawings with no crops or holes anywhere are flattened: every polygon gets its final transform baked into its points where that's exact
// C! drawings with crops or holes only get rewrites that keep exactly the same stencil and drawing steps

void Drawing::optimize( )
{
    return_if( m_nodes.empty( ) );

    // C! baking polygons into an inner drawing can let its outer nodes merge too, so keep going until a pass changes nothing
    // C! capped: no pass adds nodes, but merging and flattening multiply transforms in different orders, so rounding could keep
    // C! changing them a little without ever settling
    while_max( true, 100 )
    {
        Nodes nodes = m_nodes;

        if( m_nodes.has_crops_or_holes( ) && m_nodes.all_identity( ) )
        {
            varray<FixedRectangle> poly_bounds;

            if( m_nodes.add_polygon_bounds( poly_bounds ) )
            {
                bool crops_hold_everything = true;
                m_nodes.remove_unused_crops_and_holes( poly_bounds, crops_hold_everything );

                if( crops_hold_everything ) { m_nodes.remove_crops( ); }
            }
        }

        if( m_nodes.has_crops_or_holes( ) )
        {
            m_nodes.merge( true );
        }
        else
        {
            m_nodes = m_nodes.optimized_crop_free( );
        }

        break_if( m_nodes == nodes );
    }
}

// C! a transform can be baked into a polygon's points when it's affine and doesn't collapse the polygon (mirroring is fine: triangulation works on the raw points),
// C! and for an outline only when it's also rigid (rotation, mirroring, and translation), since a scale would change how thick the outline is drawn
bool Drawing::ColoredPolygon::bakeable( Transform cref in_transform ) const
{
    return_true_if( in_transform.is_identity( ) );
    return_false_if( !is_zero( in_transform.get( 2, 0 ) ) || !is_zero( in_transform.get( 2, 1 ) ) || !is_one( in_transform.get( 2, 2 ) ) );

    dec a = in_transform.get( 0, 0 );
    dec b = in_transform.get( 0, 1 );
    dec c = in_transform.get( 1, 0 );
    dec d = in_transform.get( 1, 1 );

    return_false_if( is_zero( ( a * d ) - ( b * c ) ) );
    return_true_if( thickness == FILLED );

    return( is_one( ( a * a ) + ( c * c ) ) && is_one( ( b * b ) + ( d * d ) ) && is_zero( ( a * b ) + ( c * d ) ) );
}

uint Drawing::Nodes::node_count( ) const
{
    uint count = size( );

    for_each_const( node, *this )
    {
        for_each_const( drawing, node.drawings ) { count += drawing.node_count( ); }
    }

    return( count );
}

bool Drawing::Nodes::draws_nothing( ) const
{
    for_each_const( node, *this )
    {
        return_false_if( !node.colored_polys.empty( ) );

        for_each_const( drawing, node.drawings ) { return_false_if( !drawing.draws_nothing( ) ); }
    }

    return( true );
}

bool Drawing::Nodes::has_crops_or_holes( ) const
{
    for_each_const( node, *this )
    {
        return_true_if( !node.crop_polys.empty( ) || !node.hole_polys.empty( ) );

        for_each_const( drawing, node.drawings ) { return_true_if( drawing.has_crops_or_holes( ) ); }
    }

    return( false );
}

bool Drawing::Nodes::all_identity( ) const
{
    for_each_const( node, *this )
    {
        return_false_if( !node.transform.is_identity( ) );

        for_each_const( drawing, node.drawings ) { return_false_if( !drawing.all_identity( ) ); }
    }

    return( true );
}

bool Drawing::Nodes::recolor( bool in_set_color, Color cref in_color, bool in_use_filter, function<void( Color ref in_color )> cref in_filter )
{
    bool opaque = true;

    for_each( node, *this )
    {
        for_each( colored_poly, node.colored_polys )
        {
            colored_poly.opaque = true;

            for_each( color, colored_poly.colors )
            {
                if( in_set_color )
                {
                    color = in_color;
                }

                if( in_use_filter )
                {
                    in_filter( color );
                }

                colored_poly.opaque = ( colored_poly.opaque && color.is_opaque( ) );
            }

            opaque = ( opaque && colored_poly.opaque );
        }

        for_each( drawing, node.drawings )
        {
            opaque = ( drawing.recolor( in_set_color, in_color, in_use_filter, in_filter ) && opaque );
        }
    }

    return( opaque );
}

// C! the way the camera draws the nodes (see render_nodes in Camera.cpp): walking from the outermost node in, each node's transform applies
// C! to it and every node before it, and so do its crops. Every point is transformed (not a box) so rotating doesn't grow the result,
// C! and each polygon's box is cut down to the box of every crop around it
void Drawing::Nodes::add_bounding_box( Transform cref in_outer, varray<FixedRectangle> in_crops, bool ref in_any, FixedRectangle ref in_bounds ) const
{
    Transform trans = in_outer;

    for_range( n, size( ) )
    {
        Node cref node = at( size( ) - 1 - n );

        trans = ( trans * node.transform );

        if( !node.crop_polys.empty( ) )
        {
            MinMaxPoint crop_bounds;
            for_each_const( crop, node.crop_polys ) { for_each_const( pt, crop.points( ) ) { crop_bounds.update( trans.apply( pt ) ); } }

            in_crops.insert_back( FixedRectangle( crop_bounds.min( ), crop_bounds.max( ) ) );
        }

        // C! the most the transform stretches any length (its largest singular value), to widen outlines by;
        // C! rotating or mirroring doesn't change it, so baking those into the points (optimize( )) keeps the same box
        dec a = dec( trans.get( 0, 0 ) );
        dec b = dec( trans.get( 0, 1 ) );
        dec c = dec( trans.get( 1, 0 ) );
        dec d = dec( trans.get( 1, 1 ) );

        dec half_sum = half( ( a * a ) + ( b * b ) + ( c * c ) + ( d * d ) );
        dec det = ( ( a * d ) - ( b * c ) );
        dec stretch = sqrt( half_sum + sqrt( max( 0.0, ( half_sum * half_sum ) - ( det * det ) ) ) );

        for_each_const( colored_poly, node.colored_polys )
        {
            continue_if( colored_poly.poly.points( ).empty( ) );

            MinMaxPoint poly_bounds;
            for_each_const( pt, colored_poly.poly.points( ) ) { poly_bounds.update( trans.apply( pt ) ); }

            FixedRectangle box = FixedRectangle( poly_bounds.min( ), poly_bounds.max( ) );

            if( !is_zero( colored_poly.thickness ) && !colored_poly.preserve_thickness )
            {
                box.expand( colored_poly.thickness * stretch ); // C! a full thickness each way, enough for extended line ends too
            }

            bool visible = true;

            for_each_const( crop, in_crops )
            {
                // C! worked out here rather than with intersects( ), which doesn't count boxes that only touch (or a line with no width)
                dec lower_x = max( dec( box.lower_bound_x( ) ), dec( crop.lower_bound_x( ) ) );
                dec lower_y = max( dec( box.lower_bound_y( ) ), dec( crop.lower_bound_y( ) ) );
                dec upper_x = min( dec( box.upper_bound_x( ) ), dec( crop.upper_bound_x( ) ) );
                dec upper_y = min( dec( box.upper_bound_y( ) ), dec( crop.upper_bound_y( ) ) );

                if( ( lower_x > upper_x ) || ( lower_y > upper_y ) )
                {
                    visible = false;
                    break;
                }

                box = FixedRectangle( Point( lower_x, lower_y ), Point( upper_x, upper_y ) );
            }

            continue_if( !visible );

            if( in_any )
            {
                in_bounds.union_with( box );
            }
            else
            {
                in_bounds = box;
                in_any = true;
            }
        }

        for_each_const( drawing, node.drawings )
        {
            drawing.add_bounding_box( trans, in_crops, in_any, in_bounds );
        }
    }
}

// C! adds the box around every polygon to in_polygon_bounds (in each polygon's own coordinates, so only right when every transform is the identity), or returns false if it can't be known (an outline that keeps its thickness on screen gets wider when zoomed out)
bool Drawing::Nodes::add_polygon_bounds( varray<FixedRectangle> ref in_poly_bounds ) const
{
    for_each_const( node, *this )
    {
        for_each_const( colored_poly, node.colored_polys )
        {
            return_false_if( ( colored_poly.thickness != FILLED ) && colored_poly.preserve_thickness );

            FixedRectangle bounds = colored_poly.poly.bounds( );
            in_poly_bounds.insert_back( FixedRectangle( bounds.width( ) + x2( colored_poly.thickness ), bounds.height( ) + x2( colored_poly.thickness ), bounds.center( ) ) );
        }

        for_each_const( drawing, node.drawings ) { return_false_if( !drawing.add_polygon_bounds( in_poly_bounds ) ); }
    }

    return( true );
}

// C! with every transform the identity, crops and holes are in the same space as the polygons: a hole that touches no polygon does nothing,
// C! and if every crop holds every polygon the crops do nothing either
void Drawing::Nodes::remove_unused_crops_and_holes( varray<FixedRectangle> cref in_poly_bounds, bool ref in_crops_hold_everything )
{
    for_each( node, *this )
    {
        varray<Polygon> holes;

        for_each_const( hole, node.hole_polys )
        {
            bool used = false;

            for_each_const( bounds, in_poly_bounds ) { if( hole.bounds( ).intersects( bounds ) ) { used = true; break; } }

            if( used ) { holes.insert_back( hole ); }
        }

        node.hole_polys = holes;

        for_each_const( crop, node.crop_polys )
        {
            // C! a convex crop holds an area when it holds all four corners; a concave one can't be told that easily, so it's kept
            for_each_const( bounds, in_poly_bounds )
            {
                bool holds = crop.convex( ) && crop.contains( bounds.top_left( ) ) && crop.contains( bounds.top_right( ) ) && crop.contains( bounds.bottom_left( ) ) && crop.contains( bounds.bottom_right( ) );

                if( !holds ) { in_crops_hold_everything = false; }
            }
        }

        for_each( drawing, node.drawings ) { drawing.remove_unused_crops_and_holes( in_poly_bounds, in_crops_hold_everything ); }
    }
}

void Drawing::Nodes::remove_crops( )
{
    for_each( node, *this )
    {
        node.crop_polys.clear( );

        for_each( drawing, node.drawings ) { drawing.remove_crops( ); }
    }
}

// C! rewrites that keep exactly the same stencil and drawing steps, for drawings with crops or holes
void Drawing::Nodes::merge( bool in_crop_free_context )
{
    // C! with crops or holes in here or around this drawing, the stencil a node or nested drawing sets carries on to what's drawn after it,
    // C! even when it draws nothing itself
    bool stencil_carries = !in_crop_free_context || has_crops_or_holes( );

    for_each( node, *this )
    {
        node.crop_polys.remove_duplicates( );
        node.hole_polys.remove_duplicates( );
    }

    // C! nested drawings: drop the ones without nodes, and optimize the rest (fully when nothing around them crops)
    bool crop_free = in_crop_free_context;
    for_range( j, size( ) ) // C! from the outermost node in, since a node's crops reach every node before it
    {
        Node ref node = at( size( ) - j - 1 );

        crop_free = crop_free && node.crop_polys.empty( ) && node.hole_polys.empty( );

        varray<Nodes> drawings;

        for_each( drawing, node.drawings )
        {
            continue_if( drawing.empty( ) );

            if( crop_free && !drawing.has_crops_or_holes( ) )
            {
                drawing = drawing.optimized_crop_free( );

                // C! it draws nothing, but still turns the stencil off for what comes after it
                if( drawing.empty( ) && stencil_carries ) { drawing.insert_back( ); }
            }
            else
            {
                drawing.merge( crop_free );
            }

            if( !drawing.empty( ) ) { drawings.insert_back( drawing ); }
        }

        node.drawings = drawings;
    }

    bool merged = true;
    while( merged )
    {
        merged = false;

        for_range( i, size( ) )
        {
            // C! an empty node only sets the stencil that the next node drawn (the one below it, or above it for the first node) sets anyway,
            // C! but the last node left has to stay, since the stencil it sets carries on to what's drawn after this drawing
            if( ( size( ) > 1 ) && at( i ).empty( ) )
            {
                remove_at( i );
                merged = true;
                break;
            }

            // C! the first node, with only a transform, transforms nothing, and with no crops or holes in here the stencil it sets doesn't matter either
            if( !i && !stencil_carries && ( size( ) > 1 ) && at( i ).transform_only( ) )
            {
                remove_at( i );
                merged = true;
                break;
            }

            continue_if( !i );

            // C! a node with only a transform sets the stencil, then the node below it sets it again straight away under the same transform
            // C! times its own, so the transform can move into the node below
            if( at( i ).transform_only( ) )
            {
                at( i - 1 ).transform = at( i ).transform * at( i - 1 ).transform;
                remove_at( i );
                merged = true;
                break;
            }

            Node cref below = at( i - 1 );
            Node cref above = at( i );

            // C! a node with no transform, crops or holes sets the same stencil as the node above it, so it can join it,
            // C! as long as its polygons and the node above's polygons still draw in the same order around the nested drawings
            if( below.transform.is_identity( ) && below.crop_polys.empty( ) && below.hole_polys.empty( ) && ( below.drawings.empty( ) || above.colored_polys.empty( ) ) )
            {
                Node joined = above;
                joined.colored_polys = below.colored_polys;
                joined.colored_polys.insert_back( above.colored_polys );
                joined.drawings = below.drawings;
                joined.drawings.insert_back( above.drawings );

                remove_at( i );
                at( i - 1 ) = joined;
                merged = true;
                break;
            }
        }
    }
}

// C! for a drawing with no crops or holes, drawn where nothing around it crops: flattened, unless merging its nodes ends up with fewer
Drawing::Nodes Drawing::Nodes::optimized_crop_free( ) const
{
    varray<TransformedColoredPolygon> polys;
    flatten( size( ), IDENTITY_TRANSFORM, polys );

    Nodes rebuilt_nodes = rebuilt( polys );

    Nodes merged = *this;
    merged.merge( true );

    return( ( rebuilt_nodes.node_count( ) <= merged.node_count( ) ) ? rebuilt_nodes : merged );
}

// C! every polygon in the order the camera draws it, with the transform it's drawn under ( nodes 0 .. in_count - 1, under in_outer )
void Drawing::Nodes::flatten( uint in_count, Transform cref in_outer, varray<TransformedColoredPolygon> ref in_transformed_polys ) const
{
    return_if( !in_count );

    Node cref node = at( in_count - 1 );
    Transform inner = in_outer * node.transform;

    flatten( in_count - 1, inner, in_transformed_polys );

    for_each_const( colored_poly, node.colored_polys ) { in_transformed_polys.insert_back( { colored_poly, inner } ); }

    for_each_const( drawing, node.drawings ) { drawing.flatten( drawing.size( ), inner, in_transformed_polys ); }
}

// C! as few nodes as possible that draw in_polygons in order: baked polygons go straight into a node, and so do polygons that keep their transform
// C! while they're at the start (a node's transform also applies to every node before it, so only the first node can have one of its own),
// C! otherwise a polygon that keeps its transform goes in a nested drawing of one node with that transform (shared with the polygons next to it that have the same one)
Drawing::Nodes Drawing::Nodes::rebuilt( varray<TransformedColoredPolygon> cref in_polys )
{
    Nodes nodes;

    for_each_const( transformed, in_polys )
    {
        bool baked = transformed.colored_poly.bakeable( transformed.transform );
        Transform transform = baked ? IDENTITY_TRANSFORM : transformed.transform;

        ColoredPolygon colored_poly = transformed.colored_poly;

        if( baked && !transformed.transform.is_identity( ) )
        {
            colored_poly.poly.transform( transformed.transform );
            colored_poly.poly.points( true ); // C! work the transformed points out now, not on every render
        }

        if( nodes.empty( ) )
        {
            nodes.insert_back( );
            nodes.back( ).transform = transform;
        }

        // C! a node draws its polygons before its nested drawings, so a polygon after a nested drawing needs the next node
        if( nodes.back( ).drawings.empty( ) && ( nodes.back( ).transform == transform ) )
        {
            nodes.back( ).colored_polys.insert_back( colored_poly );
            continue;
        }

        if( baked || !nodes.back( ).transform.is_identity( ) ) { nodes.insert_back( ); }

        if( baked )
        {
            nodes.back( ).colored_polys.insert_back( colored_poly );
            continue;
        }

        Node ref node = nodes.back( );

        if( !node.drawings.empty( ) && ( node.drawings.back( ).back( ).transform == transform ) )
        {
            node.drawings.back( ).back( ).colored_polys.insert_back( colored_poly );
        }
        else
        {
            Node nested;
            nested.transform = transform;
            nested.colored_polys.insert_back( colored_poly );

            node.drawings.insert_back( Nodes( varray<Node>( { nested } ) ) );
        }
    }

    return( nodes );
}

Drawing::Nodes cref Drawing::nodes( ) cref
{
    return( m_nodes );
}

Drawing::Nodes Drawing::nodes( ) &&
{
    return( m_nodes );
}

Drawing::Node ref Drawing::back_node( bool in_add_new )
{
    if( in_add_new || is_empty( m_nodes ) )
    {
        m_nodes.insert_back( );
    }

    return( m_nodes.back( ) );
}
