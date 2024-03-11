#include "Camera.hpp"

#include "Object.hpp"

AXN_NAMESPACES
using axn::graphics::Camera;

const uint Y_PARALAX_DAMPEN_EXPONENT = 32;
const ufloat MIN_ZOOM = 0.64f;
const ufloat MAX_ZOOM = 2.f;

Camera::Camera( const Coordinate & _target, const planc & _width, const planc & _height,
                const ufloat _zoom ) {
    center( _target );
    target( _target );
    width( _width );
    height( _height );
    zoom( _zoom );
}

Coordinate Camera::screen_to_world( const Coordinate & _screen_position ) const {
    Coordinate world_position = _screen_position;
    // Transform t; // TODO
    // t *= MoveTransform(center() - (Vector(width(), height()).half()));
    // t *= MirrorTransformX();
    // t *= MoveTransform(Vector(0.f, center().y() * 2.f));
    // t *= MoveTransform(center());
    // t *= ScaleTransform(1.f / zoom());
    world_position += center( ) - ( Vector( width( ), height( ) ).half( ) );
    world_position.y( -world_position.y( ) + ( center( ).y( ) * 2.f ) );
    world_position = Vector( center( ), world_position ) / zoom( );
    return world_position;
}

Coordinate Camera::world_to_screen( const Coordinate & _world_position ) const {
    Coordinate screen_position = _world_position;
    screen_position = Vector( center( ), screen_position ) * zoom( );
    screen_position.y( -screen_position.y( ) + ( center( ).y( ) * 2.f ) );
    screen_position -= center( ) - ( Vector( width( ), height( ) ).half( ) );
    return screen_position;
}

bool Camera::in_view( const Coordinate & _world_position ) const {
    Coordinate screen_position = world_to_screen( _world_position );
    return in_range( screen_position.x( ), width( ) ) &&
           in_range( screen_position.y( ), height( ) );
}

bool visible_sort( const Visible* const & v1, const Visible* const & v2 ) {
    if( v1->z( ) != v2->z( ) )
        return ( v1->z( ) < v2->z( ) );
    if( v1->layer_position( ) != v2->layer_position( ) )
        return ( v1->layer_position( ) < v2->layer_position( ) );
    return false;
}

Camera & Camera::clear( ) {
    m_colored_triangles.clear( );
    return *this;
}
Camera & Camera::render( ) {
    // m_subjects.sort(visible_sort);

    Visible cursor_visible( cursor_drawing( ).move( m_cursor_world_position ) );
    capture( &cursor_visible );

    for_each( subject, m_subjects ) { render( subject ); }
    m_subjects.clear( );

    return *this;
}

const varray<ColoredTriangle> & Camera::rendering( ) const { return m_colored_triangles; }

Camera & Camera::render( const Visible* _subject ) {
    const Drawing & _drawing = _subject->drawing( );

    if( _drawing.polygon_count( ) ) {
        const ufloat _z = _subject->z( );
        const ufloat _zoom = Camera::zoom( );

        Coordinate camera_offset = Vector( width( ), height( ) ).half( );
        
        varray<ColoredTriangle> colored_triangles;
        varray<ColoredTriangle> colored_triangles_border;
        
        for_each( colored_shape, _drawing.colored_shapes( ) ) {
            ufloat thickness = colored_shape.thickness *
                               ( colored_shape.preserve_thickness ? (ufloat)1.f : _zoom );
            Shape shape = colored_shape.shape;
            Shape shape_border;
            
            if ( _drawing.has_border( ) ) {
                shape_border = shape;
                shape_border.expand( _drawing.border_width( ) );
            }
            
            shape.move( _drawing.center( ) );
            if ( _drawing.has_border( ) )
                shape_border.move( _drawing.center( ) );
            
            if( _z ) {
                shape.scale( _zoom * _z );
                shape.move(
                    camera_offset -
                    Vector( center( ).x( ) * _zoom * _z,
                            center( ).y( ) *
                                ( _zoom - pow<float>( ( 1.f - (float)_z ),
                                                      Y_PARALAX_DAMPEN_EXPONENT ) ) ) );
                if ( _drawing.has_border( ) ) {
                    shape_border.scale( _zoom * _z );
                    shape_border.move(
                        camera_offset -
                        Vector( center( ).x( ) * _zoom * _z,
                                center( ).y( ) *
                                    ( _zoom - pow<float>( ( 1.f - (float)_z ),
                                                          Y_PARALAX_DAMPEN_EXPONENT ) ) ) );
                }
            } else {
                shape.move( camera_offset );
                if ( _drawing.has_border( ) )
                    shape_border.move( camera_offset );
            }

            if( thickness == FILLED ) {
                const varray<Shape> & triangles = shape.triangles( );
                const varray<Shape> & triangles_border = shape_border.triangles( );
                
                uint count = triangles.size( );
                m_colored_triangles.reserve_more( count );
                
                for_range( count ) {
                    ColoredTriangle colored_tri( triangles[ i ],
                                                colored_shape.colors[ 0 ],
                                                colored_shape.colors[ i + 1 ],
                                                colored_shape.colors[ i + 2 ] );
                    
                    if ( _drawing.has_border( ) ) {
                        colored_triangles.push_back( colored_tri );
                        
                        ColoredTriangle colored_tri_border( triangles_border[ i ],
                                                           _drawing.border_color( ) );
                        colored_triangles_border.push_back( colored_tri_border );
                    } else {
                        m_colored_triangles.push_back( colored_tri );
                    }
                }
            } else {
                const varray<Line> & lines = shape.lines( );

                uint count = lines.size( ) * 2; // two rectangles per line
                m_colored_triangles.reserve_more( count );

                for_range( lines.size( ) ) {
                    const Line & line = lines[ i ];
                    const Vector line_vector( line.c1( ), line.c2( ) );       
                    const Shape line_shape = Shape::rectangle(
                        line_vector.magnitude( ), thickness,
                        ( line_vector.half( ) ).destination( ), line.angle( ) );
                    const varray<Shape> & line_shape_triangles = line_shape.triangles( );

                    for_range( line_shape_triangles.size( ) ) {
                        
                        if ( _drawing.has_border( ) ) {
                            // TODO
                        } else {
                            m_colored_triangles.push_back(
                                ColoredTriangle( line_shape_triangles[ i ],
                                    colored_shape.colors[ 0 ],
                                    colored_shape.colors[ i + 1 ],
                                    colored_shape.colors[ i + 2 ] ) );
                        }
                    }
                }
            }
        }
        
        m_colored_triangles.push_back( colored_triangles_border );
        m_colored_triangles.push_back( colored_triangles );
    }

    return *this;
}

Drawing Camera::cursor_drawing( ) const {
    const ufloat RETICLE_WIDTH = 1.f;
    const ufloat RETICLE_LENGTH = 9.f;

    Drawing cursor_drawing;

    cursor_drawing.add(
        BLACK, Shape::rectangle( ( RETICLE_LENGTH + 2 ) / zoom( ), ( RETICLE_WIDTH + 2 ) / zoom( ) ),
        0 );
    cursor_drawing.add(
        BLACK, Shape::rectangle( ( RETICLE_WIDTH + 2 ) / zoom( ), ( RETICLE_LENGTH + 2 ) / zoom( ) ),
        0 );
    cursor_drawing.add( WHITE, Shape::rectangle( RETICLE_WIDTH / zoom( ), RETICLE_LENGTH / zoom( ) ),
                        0 );
    cursor_drawing.add( WHITE, Shape::rectangle( RETICLE_LENGTH / zoom( ), RETICLE_WIDTH / zoom( ) ),
                        0 );

    return cursor_drawing;
}

Camera & Camera::update( const Coordinate & _target, bool _hard_target_set ) {
    if( _hard_target_set )
        center( _target );
    else
        target( _target );

    Vector movement = Vector( m_center, m_target ) * m_movement_ratio;
    m_cursor_world_position += movement;
    m_center += movement;

    ++m_age;
    return *this;
}

Camera & Camera::capture( const Visible* _subject ) {
    if( _subject )
        m_subjects.push_back( _subject );
    return *this;
}

uint Camera::age( ) const { return m_age; }

planc Camera::width( ) const { return m_width; }
Camera & Camera::width( const planc & _width ) {
    m_width = _width;
    return *this;
}

planc Camera::height( ) const { return m_height; }

Camera & Camera::height( const planc & _height ) {
    m_height = _height;
    return *this;
}

ufloat Camera::zoom( ) const { return m_zoom; }

Camera & Camera::zoom( const ufloat _zoom ) {
    if( !in_range( _zoom, MIN_ZOOM, MAX_ZOOM ) )
        return *this;
    Coordinate cursor_screen_position = world_to_screen( m_cursor_world_position );
    m_zoom = _zoom;
    m_cursor_world_position = screen_to_world( cursor_screen_position );
    return *this;
}

Coordinate Camera::center( ) const { return m_center; }
Camera & Camera::center( const Coordinate & _center ) {
    m_center = _center;
    return *this;
}

Coordinate Camera::target( ) const { return m_target; }
Camera & Camera::target( const Coordinate & _target, const bool _hard_set ) {
    m_target = _target;
    if( _hard_set )
        center( _target );
    return *this;
}
