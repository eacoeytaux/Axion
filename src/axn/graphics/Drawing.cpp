#include "axn/graphics/Drawing.hpp"

varray<Drawing::Node> Drawing::nodes( ) const
{
    return( m_nodes );
}

Drawing::Node ref Drawing::back_node( bool in_add_new )
{
    if( in_add_new || m_nodes.empty( ) )
    {
        m_nodes.insert_back( );
    }

    return( m_nodes.back( ) );
}

Drawing ref Drawing::transform( Transform cref in_trans )
{
    back_node( ).transform.chain( in_trans );

    rethis;
}

Drawing ref Drawing::draw( Drawing cref in_drawing )
{
    if( m_color_filter_set )
    {
        // TODO these needs to apply to all nodes!
    }
    else
    {
        m_opaque = ( m_opaque && in_drawing.opaque( ) );
    }

    m_bounding_box.union_with( in_drawing.bounding_box( ) );

    back_node( !back_node( ).transform.is_identity( ) ||
               back_node( ).colored_shapes.size( ) ||
               back_node( ).crop_shapes.size( ) ||
               back_node( ).hole_shapes.size( ) ).drawings.insert_back( in_drawing.nodes( ) );

    rethis;
}

Drawing ref Drawing::draw( Drawing cref in_drawing, Color cref in_color )
{
    // TODO

    m_opaque = m_opaque && in_color.is_opaque( );

    m_bounding_box.union_with( in_drawing.bounding_box( ) );

    back_node( !back_node( ).transform.is_identity( ) ||
               back_node( ).colored_shapes.size( ) ||
               back_node( ).crop_shapes.size( ) ||
               back_node( ).hole_shapes.size( ) ).drawings.insert_back( in_drawing.nodes( ) );

    rethis;
}

Drawing ref Drawing::add_hole( Polygon cref in_polygon )
{
    ColoredPolygon colored_polygon;
    colored_polygon.polygon = in_polygon;
    colored_polygon.colors = { TRANSPARENT };
    colored_polygon.hole = true;

    back_node( !back_node( ).transform.is_identity( ) ).hole_shapes.insert_back( colored_polygon );

    rethis;
}

Drawing ref Drawing::add_bounds( Polygon cref in_polygon )
{
    ColoredPolygon colored_polygon;
    colored_polygon.polygon = in_polygon;
    colored_polygon.colors = { TRANSPARENT };
    colored_polygon.fill = true;

    m_bounding_box.union_with( in_polygon.bounds( ) ); // TODO not just union, but make bounding box only bounds

    back_node( !back_node( ).transform.is_identity( ) ).crop_shapes.insert_back( colored_polygon );

    rethis;
}

Drawing ref Drawing::clear_bounds( )
{
    ColoredPolygon colored_polygon;
    colored_polygon.reset = true;

    rethis;
}

Drawing ref Drawing::draw( varray<Color> cref in_colors,
                           Polygon cref in_polygon,
                           dec in_thickness,
                           bool in_preserve_thickness,
                           bool in_extend_lines )
{
    rethis_if( !( in_polygon.sides( ) ) );

    Assert( !is_neg( in_thickness ) );
    Assert( in_colors.size( ) );

    ColoredPolygon colored_polygon = ColoredPolygon( );

    colored_polygon.polygon = in_polygon;

    colored_polygon.thickness = in_thickness;
    colored_polygon.preserve_thickness = in_preserve_thickness;
    colored_polygon.extend_lines = in_extend_lines;

    colored_polygon.colors = in_colors;

    colored_polygon.opaque = true;
    for_range( i, colored_polygon.colors.size( ) )
    {
        if( m_color_filter_set )
        {
            m_color_filter( colored_polygon.colors[ i ] );
        }

        if( !colored_polygon.colors[ i ].is_opaque( ) )
        {
            colored_polygon.opaque = false;

            break_if( !m_color_filter_set );
        }
    }

    m_bounding_box.union_with( colored_polygon.polygon.bounds( false ) );

    back_node( !back_node( ).transform.is_identity( ) ||
               back_node( ).crop_shapes.size( ) ||
               back_node( ).hole_shapes.size( ) ).colored_shapes.insert_back( colored_polygon );

    rethis;
}

Drawing ref Drawing::draw( Color cref in_color,
                           Polygon cref in_polygon,
                           dec in_thickness,
                           bool in_preserve_thickness,
                           bool in_extend_lines )
{
    return( draw( varray<Color>( max<uint>( 1, in_polygon.sides( ) ), in_color ), in_polygon, in_thickness, in_preserve_thickness, in_extend_lines ) );
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

void Drawing::optimize( )
{
    // TODO
}
