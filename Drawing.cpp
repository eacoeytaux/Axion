#include "Drawing.hpp"

const varray<Drawing::ColoredPolygon> & Drawing::colored_polygons( cbool _transformed ) const
{
    if( _transformed && !transform( ).is_identity( ) )
    {
        const Transform _t = transform( );

        FixedRectangle transformed_bounding_box;

        for_each( colored_polygon, m_colored_polygons )
        {
            colored_polygon.polygon.transform( _t );

            transformed_bounding_box.union_with( FixedRectangle::bounds( colored_polygon.polygon, false ) );
        }

        m_bounding_box = transformed_bounding_box;

        const_clear_transform( );
    }

    return m_colored_polygons;
}

Drawing & Drawing::erase( Polygon cref _polygon )
{
    ColoredPolygon & colored_polygon = m_colored_polygons.insert_back( );
    colored_polygon.polygon = _polygon;
    colored_polygon.colors = { TRANSPARENT };
    colored_polygon.hole = true;

    rethis;
}

Drawing & Drawing::bind( Polygon cref _polygon )
{
    ColoredPolygon & colored_polygon = m_colored_polygons.insert_back( );
    colored_polygon.polygon = _polygon;
    colored_polygon.colors = { TRANSPARENT };
    colored_polygon.fill = true;

    rethis;
}

Drawing & Drawing::clear_bounds( )
{
    ColoredPolygon & colored_polygon = m_colored_polygons.insert_back( );
    colored_polygon.reset = true;

    rethis;
}

Drawing & Drawing::draw( Drawing cref _drawing )
{
    if( m_filter_function_set )
    {
        for_each( colored_polygon, _drawing.colored_polygons( ) )
        {
            ColoredPolygon filtered_colored_polygon = colored_polygon;
            for_range( i, filtered_colored_polygon.colors.size( ) )
            {
                m_filter_function( filtered_colored_polygon.colors[ i ] );

                if( !filtered_colored_polygon.colors[ i ].opaque( ) )
                {
                    filtered_colored_polygon.opaque = false;
                    m_opaque = false;
                }
            }

            m_colored_polygons.insert_back( filtered_colored_polygon );

            if( !filtered_colored_polygon.reset )
            {
                m_bounding_box.union_with( FixedRectangle::bounds( filtered_colored_polygon.polygon, false ) );
            }
        }
    }
    else
    {
        m_colored_polygons.insert_back( _drawing.colored_polygons( ) );
        m_bounding_box.union_with( _drawing.bounding_box( ) );

        m_opaque = m_opaque && _drawing.opaque( );
    }

    rethis;
}

Drawing & Drawing::draw( Drawing cref _drawing, Color cref _color )
{
    for_each( colored_polygon, _drawing.colored_polygons( ) )
    {
        m_colored_polygons.insert_back( colored_polygon );
        m_colored_polygons.back( ).colors = { _color };
        m_colored_polygons.back( ).opaque = _color.opaque( );
    }

    m_bounding_box.union_with( _drawing.bounding_box( ) );

    m_opaque = m_opaque && _color.opaque( );

    rethis;
}

Drawing & Drawing::draw( const varray<Color> & _colors,
                         Polygon cref _polygon,
                         cdec _thickness,
                         cbool _preserve_thickness,
                         cbool _extend_lines )
{
    Assert( _thickness >= 0.0 );
    Assert( _colors.size( ) );

    ColoredPolygon & colored_polygon = m_colored_polygons.insert_back( );
    colored_polygon.polygon = _polygon;
    colored_polygon.thickness = _thickness;
    colored_polygon.preserve_thickness = _preserve_thickness;
    colored_polygon.extend_lines = _extend_lines;

    colored_polygon.colors = _colors;

    colored_polygon.opaque = true;
    for_range( i, colored_polygon.colors.size( ) )
    {
        if( m_filter_function_set )
        {
            m_filter_function( colored_polygon.colors[ i ] );
        }

        if( !colored_polygon.colors[ i ].opaque( ) )
        {
            colored_polygon.opaque = false;
        }
    }

    m_bounding_box.union_with( FixedRectangle::bounds( colored_polygon.polygon, false ) );

    rethis;
}

Drawing & Drawing::draw( Color cref _color,
                         Polygon cref _polygon,
                         cdec _thickness,
                         cbool _preserve_thickness,
                         cbool _extend_lines )
{
    return draw( varray<Color>( max<uint>( 1, _polygon.sides( ) ), _color ), _polygon, _thickness, _preserve_thickness, _extend_lines );
}

Drawing & Drawing::draw( Color cref _color1,
                         Color cref _color2,
                         Line cref _line,
                         cdec _thickness,
                         cbool _preserve_thickness,
                         cbool _extend_lines )
{
    return draw( { _color1, _color2 }, Polygon( { _line.c1( ), _line.c2( ) } ), _thickness, _preserve_thickness, _extend_lines );
}

Drawing & Drawing::draw( Color cref _color,
                         Line cref _line,
                         cdec _thickness,
                         cbool _preserve_thickness,
                         cbool _extend_lines )
{
    return draw( _color, _color, _line, _thickness, _preserve_thickness, _extend_lines );
}

Drawing & Drawing::draw( Color cref _color,
                         Path cref _path,
                         cdec const _thickness,
                         cbool _preserve_thickness,
                         cbool _extend_lines )
{
    for_each( line, _path.lines( ) )
    {
        draw( _color, line, _thickness, _preserve_thickness, _extend_lines );
    }

    rethis;
}

#if defined ( AXN_DEBUG )
Drawing & Drawing::draw( Color cref _color,
                         Vector cref _vector,
                         cdec _arrow_head_length,
                         cdec _thickness,
                         cbool _preserve_thickness )
{
    draw( _color, Line( _vector.origin( ), _vector.destination( ) ), _thickness, _preserve_thickness );
    draw( _color, Line( _vector.destination( ), _vector.destination( ) - VectorA( _vector.angle( ) + ( half( RIGHT ) ), _arrow_head_length ) ), _thickness, _preserve_thickness, true );
    draw( _color, Line( _vector.destination( ), _vector.destination( ) - VectorA( _vector.angle( ) - ( half( RIGHT ) ), _arrow_head_length ) ), _thickness, _preserve_thickness, true );
    rethis;
}
#endif
