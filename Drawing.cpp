#include "Drawing.hpp"
#include "Color.hpp"

Drawing::Drawing( const Coordinate & _center ) { center( _center ); }

const varray<Drawing::ColoredPolygon> & Drawing::colored_polygons( const bool _transformed ) const
{
    if( _transformed && !transform( ).identity( ) )
    {
        FixedRectangle transformed_bounding_box;
        
        for_each( colored_polygon, m_colored_polygons )
        {
            colored_polygon.polygon.transform( transform( ) );
            
            transformed_bounding_box.union_with( FixedRectangle( colored_polygon.polygon ) );
        }
        
        m_bounding_box = transformed_bounding_box;

        const_clear_transform( );
    }

    return m_colored_polygons;
}

varray<Drawing::ColoredPolygon> Drawing::colored_polygons_border( const bool _transformed ) const
{
    varray<ColoredPolygon> colored_polygons_border;

    if( has_border( ) )
    {
        for_each( colored_polygon, colored_polygons( _transformed ) )
        {
            ColoredPolygon border = colored_polygon;
            border.polygon = Polygon::expand( border.polygon, border_width( ) );

            border.colors.clear( );
            border.colors.reserve( border.polygon.sides( ) );
            for_range( i, border.polygon.sides( ) ) border.colors.insert_back( border_color( ) );

            colored_polygons_border.insert_back( border );
        }
    }

    return colored_polygons_border;
}

uint Drawing::polygon_count( ) const { return colored_polygons( false ).size( ); }

void Drawing::reserve( const uint _reserve_size ) { m_colored_polygons.reserve( _reserve_size ); }

void Drawing::reserve_more( const uint _reserve_size ) { m_colored_polygons.reserve_more( _reserve_size ); }

void Drawing::clear( const bool _reserve_mem )
{
    Transformable::clear_transform( );
    
    uint mem_size = ( _reserve_mem ? m_colored_polygons.size( ) : ZERO );
    
    clear_override_color( );
    clear_filter_function( );
    
    m_colored_polygons.clear( );
    if( _reserve_mem && mem_size )
    {
        m_colored_polygons.reserve( mem_size * TWO );
    }
    
    m_bounding_box.width( ZERO );
    m_bounding_box.height( ZERO );
    m_bounding_box.center( ORIGIN );
}

Coordinate Drawing::center( ) const { return m_center; }

Drawing & Drawing::center( const Coordinate & _center )
{
    Vector d = _center - m_center;
    if( d.has_magnitude( ) )
    {
        m_center = _center;
    }

    return *this;
}

Drawing & Drawing::override_color( const Color & _color )
{
    m_override_color = _color;
    
    if ( !m_override_color_set )
    {
        for_each( colored_polygon, m_colored_polygons )
        {
            colored_polygon.colors = { m_override_color };
        }
    }
    
    m_override_color_set = true;
    
    return *this;
}

Drawing & Drawing::clear_override_color( )
{
    m_override_color_set = false;
    
    return *this;
}

Drawing & Drawing::filter_function( const function<void ( Color & )> & _filter_function )
{
    m_filter_function = _filter_function;

    m_filter_function_set = true;
    
    return *this;
}

Drawing & Drawing::clear_filter_function( )
{
    m_filter_function_set = false;
    
    return *this;
}

Drawing & Drawing::draw( const Drawing & _drawing )
{
    // if (_drawing.has_border( ) )
    // {
    //     m_colored_polygons.insert_back( _drawing.colored_polygons_border( ) );
    // }
    
    if( m_override_color_set )
    {
        draw( _drawing, m_override_color );
    }
    else if( m_filter_function_set )
    {
        for_each( colored_polygon, _drawing.colored_polygons( false ) )
        {
            ColoredPolygon filtered_colored_polygon = colored_polygon;
            for_range( i, filtered_colored_polygon.colors.size( ) )
            {
                m_filter_function( filtered_colored_polygon.colors[ i ] );
            }
            
            m_colored_polygons.insert_back( filtered_colored_polygon );
            m_bounding_box.union_with( FixedRectangle( filtered_colored_polygon.polygon ) );
        }
    }
    else
    {
        m_colored_polygons.insert_back( _drawing.colored_polygons( ) );
        m_bounding_box.union_with( _drawing.bounding_box( ) );
    }
    
    return *this;
}

Drawing & Drawing::draw( const Drawing & _drawing, const Color & _color )
{
    for_each( colored_polygon, _drawing.colored_polygons(  ) )
    {
        m_colored_polygons.insert_back( colored_polygon ).colors = { _color };
    }
    
    if( _color.a( ) != ONE )
    {
        m_translucent = true;
    }
    
    m_bounding_box.union_with( _drawing.bounding_box( ) );

    return *this;
}

Drawing & Drawing::draw( const varray<Color> & _colors,
                         const Polygon & _polygon,
                         const dec _thickness,
                         const bool _preserve_thickness,
                         const bool _extend_lines )
{
    Assert( _thickness >= ZERO );
    Assert( _colors.size( ) );

    ColoredPolygon & colored_polygon = m_colored_polygons.insert_back( );
    colored_polygon.polygon = _polygon;
    colored_polygon.thickness = _thickness;
    colored_polygon.preserve_thickness = _preserve_thickness;
    colored_polygon.extend_lines = _extend_lines;
    
    if( m_override_color_set )
    {
        colored_polygon.colors = { m_override_color };
        colored_polygon.opaque = m_override_color.opaque( );
    }
    else
    {
        colored_polygon.colors = _colors;
        
        colored_polygon.opaque = true;
        for_range( i, colored_polygon.colors.size( ) )
        {
            if( m_filter_function_set )
            {
                m_filter_function( colored_polygon.colors[ i ] );
            }
            
            if( colored_polygon.colors[ i ].a( ) != ONE )
            {
                colored_polygon.opaque = false;
                break;
            }
        }
    }
    
    m_bounding_box.union_with( FixedRectangle( colored_polygon.polygon ) );

    return *this;
}

Drawing & Drawing::draw( const Color & _color,
                         const Polygon & _polygon,
                         const dec _thickness,
                         const bool _preserve_thickness,
                         const bool _extend_lines )
{
    return draw( varray<Color>( _polygon.sides( ), _color ), _polygon, _thickness, _preserve_thickness, _extend_lines );
}

Drawing & Drawing::draw( const Color & _color1,
                         const Color & _color2,
                         const Line & _line,
                         const dec _thickness,
                         const bool _preserve_thickness,
                         const bool _extend_lines )
{
    return draw( { _color1, _color2 }, Polygon( { _line.c1( ), _line.c2( ) } ), _thickness, _preserve_thickness, _extend_lines );
}

Drawing & Drawing::draw( const Color & _color,
                         const Line & _line,
                         const dec _thickness,
                         const bool _preserve_thickness,
                         const bool _extend_lines )
{
    return draw( _color, _color, _line, _thickness, _preserve_thickness, _extend_lines );
}

#ifdef AXN_DEBUG
Drawing & Drawing::draw( const Color & _color,
                         const Vector & _vector,
                         const dec _arrow_head_length,
                         const dec _thickness,
                         const bool _preserve_thickness )
{
    draw( _color, Line( _vector.origin( ), _vector.destination( ) ), _thickness, _preserve_thickness );
    draw( _color, Line( _vector.destination( ), _vector.destination( ) - VectorA( _vector.angle( ) + ( half( RIGHT_ANGLE ) ), _arrow_head_length ) ), _thickness, _preserve_thickness, true );
    draw( _color, Line( _vector.destination( ), _vector.destination( ) - VectorA( _vector.angle( ) - ( half( RIGHT_ANGLE ) ), _arrow_head_length ) ), _thickness, _preserve_thickness, true );
    return *this;
}
#endif

bool Drawing::translucent( ) const { return m_translucent; }
