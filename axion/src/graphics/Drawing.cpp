#include "Drawing.hpp"

AXN_NAMESPACES
using axn::graphics::Drawing;

Drawing::Drawing( const Coordinate & _center ) { center( _center ); }

const varray<Drawing::ColoredShape> & Drawing::colored_shapes( ) const {
    return m_colored_shapes;
}

varray<Drawing::ColoredShape> Drawing::colored_shapes_border( ) const {
    varray<ColoredShape> colored_shapes_border;
    if ( has_border( ) ) {
        for_each( colored_shape, m_colored_shapes ) {
            ColoredShape border = colored_shape;
            border.shape.expand( border_width( ) );
            
            border.colors.clear();
            border.colors.reserve( border.shape.coordinates( ).size( ) );
            for_range( border.shape.coordinates( ).size( ) )
                border.colors.push_back( border_color( ) );
            
            colored_shapes_border.push_back( border );
        }
    }
    colored_shapes_border.push_back( m_colored_shapes );
    return colored_shapes_border;
}

uint Drawing::polygon_count( ) const { return colored_shapes( ).size( ); }

Drawing & Drawing::reserve( const uint _reserve_size ) {
    m_colored_shapes.reserve( _reserve_size );
    return *this;
}

Drawing & Drawing::reserve_more( const uint _reserve_size ) {
    m_colored_shapes.reserve_more( _reserve_size );
    return *this;
}

Drawing & Drawing::clear( const bool _reserve_mem ) {
    uint mem_planc = ( _reserve_mem ? m_colored_shapes.size( ) : 0 );
    m_colored_shapes.clear( );
    if( _reserve_mem && mem_planc )
        m_colored_shapes.reserve( mem_planc * 2.f );
    return *this;
}

Coordinate Drawing::center( ) const { return m_center; }

Drawing & Drawing::center( const Coordinate & _center ) {
    m_center = _center;
    return *this;
}

Drawing & Drawing::move( const Vector & _vector ) {
    for_each( colored_shape, m_colored_shapes )
        colored_shape.shape.move( _vector );
    return *this;
}

Drawing & Drawing::scale( const ufloat _scale, const Coordinate & _origin ) {
    for_each( colored_shape, m_colored_shapes )
        colored_shape.shape.scale( _scale, _origin );
    return *this;
}

Drawing & Drawing::rotate( const Angle & _angle, const Coordinate & _origin ) {
    for_each( colored_shape, m_colored_shapes )
        colored_shape.shape.rotate( _angle, _origin );
    return *this;
}

Drawing & Drawing::mirror( const Vector & _axis ) {
    for_each( colored_shape, m_colored_shapes )
        colored_shape.shape.mirror( _axis );
    return *this;
}

Drawing & Drawing::add( const Drawing & _drawing ) {
    if (_drawing.has_border( ) )
        m_colored_shapes.push_back( _drawing.colored_shapes_border( ) );
    m_colored_shapes.push_back( _drawing.colored_shapes( ) );
    return *this;
}

Drawing & Drawing::add( const varray<Color> & _colors, const Shape & _shape,
                        const ufloat _thickness, const bool _preserve_thickness ) {
    ColoredShape colored_shape;
    colored_shape.shape = _shape;
    colored_shape.colors = _colors;
    colored_shape.thickness = _thickness;
    colored_shape.preserve_thickness = _preserve_thickness && ( _thickness != FILLED );
    m_colored_shapes.push_back( colored_shape );

    //    bool translucent = false;
    //    for_each (color, colored_polygon.colors) {
    //        if (color.a() != 255) {
    //            translucent = true;
    //            break;
    //        }
    //    }
    //
    //    if (translucent && !m_translucent) { // first translucent color
    //        m_translucent = true;
    //        m_colored_shapes_opaque.push_back(m_colored_shapes);
    //    }
    //
    //    if (m_translucent) {
    //        if (translucent)
    //        m_colored_shapes_translucent.push_back(m_colored_shapes.last());
    //        else
    //        m_colored_shapes_opaque.push_back(m_colored_shapes.last());
    //    }

    return *this;
}

Drawing & Drawing::add( const Color & _color, const Shape & _shape, const ufloat _thickness,
                        const bool _preserve_thickness ) {
    add( varray<Color>( _shape.sides( ), _color ), _shape, _thickness,
         _preserve_thickness );
    return *this;
}

Drawing & Drawing::add( const Color & _color1, const Color & _color2, const Line & _line,
                        const ufloat _thickness, const bool _preserve_thickness ) {
    add( { _color1, _color1, _color2, _color2 },
         Shape::rectangle( _thickness, _line.length( ),
            ( Vector( _line.c1( ), _line.c2( ) ) / 2.f ).destination( ),
            _line.angle( ) - Angle( PI_1D2 ) ),
         FILLED, _preserve_thickness );
    return *this;
}

Drawing & Drawing::add( const Color & _color, const Line & _line, const ufloat _thickness,
                        const bool _preserve_thickness ) {
    add( _color, _color, _line, _thickness, _preserve_thickness );
    return *this;
}

#ifdef AXN_DEBUG
Drawing & Drawing::add( const Color & _color, const Vector & _vector,
                        const ufloat _arrow_head_length, const ufloat _thickness,
                        const bool _preserve_thickness ) {
    add( _color, Line( _vector.origin( ), _vector.destination( ) ), _thickness, _preserve_thickness );
    add( _color, Shape::circle( _thickness, _vector.destination( ) ), _preserve_thickness );
    add( _color,
         Line( _vector.destination( ),
               _vector.destination( ) -
                   VectorA( _vector.angle( ) + PI_1D4, _arrow_head_length ) ),
         _thickness, _preserve_thickness );
    add( _color,
         Line( _vector.destination( ),
               _vector.destination( ) -
                   VectorA( _vector.angle( ) - PI_1D4, _arrow_head_length ) ),
         _thickness, _preserve_thickness );
    return *this;
}
#endif

bool Drawing::translucent( ) const { return m_translucent; }
