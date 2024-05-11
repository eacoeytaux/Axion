#include "Drawing.hpp"

#include "Color.hpp"

using axn::graphics::Drawing;

Drawing::Drawing( const Coordinate & _center ) { center( _center ); }

const varray<Drawing::ColoredShape> & Drawing::colored_shapes( ) const
{
    return m_colored_shapes;
}

varray<Drawing::ColoredShape> Drawing::colored_shapes_border( ) const
{
    varray<ColoredShape> colored_shapes_border;
    if( has_border( ) )
    {
        for_each( colored_shape, m_colored_shapes )
        {
            ColoredShape border = colored_shape;
            border.shape = Shape::expand( border.shape, border_width( ) );

            border.colors.clear( );
            border.colors.reserve( border.shape.sides( ) );
            for_range( border.shape.sides( ) ) border.colors.insert_back( border_color( ) );

            colored_shapes_border.insert_back( border );
        }
        colored_shapes_border.insert_back( m_colored_shapes );
    }
    return colored_shapes_border;
}

uint Drawing::polygon_count( ) const { return colored_shapes( ).size( ); }

Drawing & Drawing::reserve( const uint _reserve_size )
{
    m_colored_shapes.reserve( _reserve_size );
    return *this;
}

Drawing & Drawing::reserve_more( const uint _reserve_size )
{
    m_colored_shapes.reserve_more( _reserve_size );
    return *this;
}

Drawing & Drawing::clear( const bool _reserve_mem )
{
    uint mem_Planc = ( _reserve_mem ? m_colored_shapes.size( ) : 0 );
    m_colored_shapes.clear( );
    if( _reserve_mem && mem_Planc )
        m_colored_shapes.reserve( mem_Planc * 2.0 );
    return *this;
}

Coordinate Drawing::center( ) const { return m_center; }

Drawing & Drawing::center( const Coordinate & _center )
{
    Vector d = _center - m_center;
    if( d.has_magnitude( ) )
    {
        m_center = _center;
        for_each( colored_shape, m_colored_shapes ) colored_shape.shape.move( d );
    }
    return *this;
}

Drawing & Drawing::move( const Vector & _vector )
{
    if( _vector.has_magnitude( ) )
    {
        for_each( colored_shape, m_colored_shapes ) colored_shape.shape.move( _vector );
    }
    return *this;
}

Drawing & Drawing::scale( const double _scale, const Coordinate & _origin )
{
    for_each( colored_shape, m_colored_shapes ) colored_shape.shape.scale( _scale, _origin );
    return *this;
}

Drawing & Drawing::rotate( const Angle & _angle, const Coordinate & _origin )
{
    for_each( colored_shape, m_colored_shapes ) colored_shape.shape.rotate( _angle, _origin );
    return *this;
}

Drawing & Drawing::mirror( const Vector & _axis )
{
    for_each( colored_shape, m_colored_shapes ) colored_shape.shape.mirror( _axis );
    return *this;
}

Drawing & Drawing::draw( const Drawing & _drawing )
{
    // if (_drawing.has_border( ) )
    //     m_colored_shapes.insert_back( _drawing.colored_shapes_border( ) );
    m_colored_shapes.insert_back( _drawing.colored_shapes( ) );
    return *this;
}

Drawing & Drawing::draw( const varray<Color> & _colors, const Shape & _shape, const double _thickness, const bool _preserve_thickness )
{
    Assert( _thickness >= 0.0 );

    ColoredShape colored_shape;
    colored_shape.shape = _shape;
    colored_shape.colors = _colors;
    colored_shape.thickness = _thickness;
    colored_shape.preserve_thickness = _preserve_thickness && ( _thickness != FILLED );
    colored_shape.opaque = true;
    for_each( color, _colors )
    {
        if( color.a( ) != 1.0 )
        {
            colored_shape.opaque = false;
            break;
        }
    }
    m_colored_shapes.insert_back( colored_shape );

    return *this;
}

Drawing & Drawing::draw( const Color & _color, const Shape & _shape, const double _thickness, const bool _preserve_thickness )
{
    draw( varray<Color>( _shape.sides( ), _color ), _shape, _thickness, _preserve_thickness );
    return *this;
}

Drawing & Drawing::draw( const Color & _color1, const Color & _color2, const Line & _line, const double _thickness, const bool _preserve_thickness )
{
    draw( { _color1, _color1, _color2, _color2 }, Shape::rectangle( _thickness, _line.length( ), ( Vector( _line.c1( ), _line.c2( ) ) / 2.0 ).destination( ), _line.angle( ) - Angle( RIGHT_ANGLE ) ), FILLED, _preserve_thickness );
    return *this;
}

Drawing & Drawing::draw( const Color & _color, const Line & _line, const double _thickness, const bool _preserve_thickness )
{
    draw( _color, _color, _line, _thickness, _preserve_thickness );
    return *this;
}

#ifdef AXN_DEBUG
Drawing & Drawing::draw( const Color & _color, const Vector & _vector, const double _arrow_head_length, const double _thickness, const bool _preserve_thickness )
{
    draw( _color, Line( _vector.origin( ), _vector.destination( ) ), _thickness, _preserve_thickness );
    draw( _color, Shape::circle( _thickness, _vector.destination( ) ), _preserve_thickness );
    draw( _color, Line( _vector.destination( ), _vector.destination( ) - VectorA( _vector.angle( ) + ( half( RIGHT_ANGLE ) ), _arrow_head_length ) ), _thickness, _preserve_thickness );
    draw( _color, Line( _vector.destination( ), _vector.destination( ) - VectorA( _vector.angle( ) - ( half( RIGHT_ANGLE ) ), _arrow_head_length ) ), _thickness, _preserve_thickness );
    return *this;
}
#endif

bool Drawing::translucent( ) const { return m_translucent; }
