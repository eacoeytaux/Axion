#include "Camera.hpp"

#include "OGL.hpp"

#include "Engine.hpp"
#include "Object.hpp"

using axn::graphics::Camera;

namespace
{
const double MIN_ZOOM = 0.64;
const double MAX_ZOOM = 2.5;
} // namespace

Camera::Camera( const Coordinate & _target, const Planc & _width, const Planc & _height, const double _zoom ) : m_cursor_world_position( COORDINATE_INFINITY_NEGATIVE )
{
    center( _target );
    target( _target );
    width( _width );
    height( _height );
    zoom( _zoom );
}

Coordinate Camera::screen_to_world( const Coordinate & _screen_position ) const
{
    Coordinate world_position = _screen_position;
    world_position += center( ) - Vector( width( ), height( ) ).half( );
    world_position.y( -world_position.y( ) + ( center( ).y( ) * 2.0 ) );
    world_position = Vector( center( ), world_position ) / zoom( );
    return world_position;
}

Coordinate Camera::world_to_screen( const Coordinate & _world_position ) const
{
    Coordinate screen_position = _world_position;
    screen_position = Vector( center( ), screen_position ) * zoom( );
    screen_position.y( -screen_position.y( ) + ( center( ).y( ) * 2.0 ) );
    screen_position -= center( ) - Vector( width( ), height( ) ).half( );
    return screen_position;
}

bool Camera::in_view( const Coordinate & _world_position ) const
{
    Coordinate screen_position = world_to_screen( _world_position );
    return in_range( screen_position.x( ), width( ) ) && in_range( screen_position.y( ), height( ) );
}

bool visible_sort( const Visible * const & v1, const Visible * const & v2 )
{
    if( v1->z( ) != v2->z( ) )
        return ( v1->z( ) < v2->z( ) );
    if( v1->layer_position( ) != v2->layer_position( ) )
        return ( v1->layer_position( ) < v2->layer_position( ) );
    return false;
}

Camera & Camera::lighting( const Lighting * lighting )
{
    m_lighting = lighting;
    return *this;
}

Camera & Camera::clear_lighting( )
{
    m_lighting = nullptr;
    return *this;
}

Camera & Camera::clear( )
{
    m_subjects.clear( );
    return *this;
}

Camera & Camera::render( )
{
    if( !m_subjects.size( ) )
        return *this;

    const double _screen_width = Engine::screen_width( );
    const double _screen_height = Engine::screen_height( );

    const Coordinate _camera_offset = half( Vector( width( ), height( ) ) );
    const double _zoom = Camera::zoom( );

    ogl::clear( );
    ogl::depth_always( );

    ogl::push_matrix( );

    ogl::scale( 2.0 / _screen_width, 2.0 / _screen_height );
    ogl::translate( half( _screen_width ), half( _screen_height ) );
    ogl::translate( -half( width( ) ), -half( height( ) ) );

    Engine::anti_alias( ) ? ogl::enable_anti_alias( ) : ogl::disable_anti_alias( );

    Visible cursor_visible( cursor_drawing( ).move( m_cursor_world_position ) );
    capture( &cursor_visible );

    auto draw_convex_polygon = [ & ]( const varray<Color> & _colors, const varray<Coordinate> & _coordinates, const Transform & _transform, const double _z = 0.0 )
    {
        if( !_colors.size( ) || !_coordinates.size( ) )
            return;

        // ogl::push_matrix( );
        // ogl::transform( _transform );

        ogl::begin_polygons( );

        if( _colors.size( ) == 1 )
        { // TODO this is never used?
            ogl::color( _colors[ 0 ].r( ), _colors[ 0 ].g( ), _colors[ 0 ].b( ), _colors[ 0 ].a( ) );
            for_range( _coordinates.size( ) ) ogl::vertex( _coordinates[ i ].x( ), _coordinates[ i ].y( ), -_z );
        }
        else
        {
            for_range( _coordinates.size( ) )
            {
                ogl::color( _colors[ i ].r( ), _colors[ i ].g( ), _colors[ i ].b( ), _colors[ i ].a( ) );
                ogl::vertex( _coordinates[ i ].x( ), _coordinates[ i ].y( ), -_z );
            }
        }

        ogl::end( );

        // ogl::pop_matrix( );
    };

    // m_subjects.sort(visible_sort);
    for_each( subject, m_subjects )
    {
        if( !subject || !subject->drawing( ).polygon_count( ) )
            continue;

        ogl::clear_depth( );
        ogl::push_matrix( );

        if( const double _z = subject->z( ) )
        {
            const double _zzoom = _zoom * _z;

            ogl::translate( -center( ).x( ) * _zzoom,
                            -center( ).y( ) * _zzoom );

            ogl::scale( _zzoom );
        }

        const Drawing & _drawing = subject->drawing( );
        for_each( _colored_shape, _drawing.colored_shapes( ) )
        {
            const Shape & _shape = _colored_shape.shape;
            const double _thickness = _colored_shape.thickness * ( _colored_shape.preserve_thickness ? (double)1.0 : _zoom );

            ogl::push_matrix( );
            ogl::translate( _drawing.center( ).x( ), _drawing.center( ).y( ) );

            if( _thickness == FILLED )
            {
                if( _drawing.has_border( ) )
                {
                    // TODO

                    // const double _border_width = _drawing.border_width( );
                    // const Color _border_color = _drawing.border_color( );

                    // Shape shape_border = Shape::expand( _shape, _border_width );

                    // for_each( shape, shape_border.convex_partitions( ) )
                    // {
                    //     draw_convex_polygon( { _border_color }, shape.coordinates( ), shape.transform( ) );
                    // }
                }

                const varray<Shape> & _convex_shapes = _shape.convex_partitions( );
                for_range( _convex_shapes.size( ) )
                {
                    const Shape & _convex_shape = _convex_shapes[ i ];

                    draw_convex_polygon( _colored_shape.colors, _convex_shape.coordinates( ), _convex_shape.transform( ) ); // TODO need convex indices
                }
            }
            else
            {
                if( _drawing.has_border( ) )
                {
                    // TODO
                }

                const varray<Line> & _lines = _shape.lines( );
                for_range( _lines.size( ) )
                {
                    const Line & _line = _lines[ i ];
                    const Vector _line_vector( _line.c1( ), _line.c2( ) );
                    const Shape _line_shape = Shape::rectangle( _line_vector.magnitude( ), _thickness, _line_vector.half( ).destination( ), _line.angle( ) );

                    draw_convex_polygon( { _colored_shape.colors[ i ] }, _line_shape.coordinates( ), _line_shape.transform( ) );
                }
            }
            ogl::pop_matrix( );
        }
        ogl::pop_matrix( );
    }

    if( m_lighting )
    {
        const varray<LightSource> & light_sources = m_lighting->light_sources( );

        ogl::clear_depth( );
        ogl::depth_always( );
        draw_convex_polygon( { BLUE.a( 0.1 ) },
                             Shape::rectangle( _screen_width, _screen_height ).coordinates( ),
                             IdentityTransform( ) );

        ogl::push_matrix( );

        uint layer_count = 6; // todo
        for_range( layer_count + 1 )
        {
            ogl::clear_depth( );
            ogl::depth_always( );

            ogl::push_matrix( );
            ogl::scale( _zoom );
            ogl::translate( -center( ).x( ),
                            -center( ).y( ) );

            for_each( light, light_sources )
            {
                ogl::push_matrix( );
                ogl::translate( light.position( ).x( ),
                                light.position( ).y( ) );

                draw_convex_polygon( { TRANSPARENT },
                                     Shape::circle( light.radius( ) + ( light.radius( ) * 0.333 ) * i * pow( 0.88, i ) ).coordinates( ),
                                     IdentityTransform( ) ); // todo

                ogl::pop_matrix( );
            }

            ogl::pop_matrix( );

            ogl::depth_not_equal( );
            draw_convex_polygon( { BLACK.a( min( 1.0, ( (double)( i + 1 ) / (double)layer_count ) ) ) },
                                 Shape::rectangle( _screen_width, _screen_height ).coordinates( ),
                                 IdentityTransform( ) );
        }

        ogl::pop_matrix( );
    }

    ogl::pop_matrix( );

    return *this;
}

Drawing Camera::cursor_drawing( ) const
{
    const double RETICLE_WIDTH = 2.0;
    const double RETICLE_LENGTH = 8.0;
    const double RETICLE_BORDER_WIDTH = 1.0;

    Drawing cursor_drawing;

    const double _zoom = zoom( );
    cursor_drawing.draw( BLACK, Shape::rectangle( ( RETICLE_LENGTH + ( RETICLE_BORDER_WIDTH * 2.0 ) ) / _zoom, ( RETICLE_WIDTH + ( RETICLE_BORDER_WIDTH * 2.0 ) ) / _zoom ), 0 );
    cursor_drawing.draw( BLACK, Shape::rectangle( ( RETICLE_WIDTH + ( RETICLE_BORDER_WIDTH * 2.0 ) ) / _zoom, ( RETICLE_LENGTH + ( RETICLE_BORDER_WIDTH * 2.0 ) ) / _zoom ), 0 );
    cursor_drawing.draw( WHITE, Shape::rectangle( RETICLE_WIDTH / _zoom, RETICLE_LENGTH / _zoom ), 0 );
    cursor_drawing.draw( WHITE, Shape::rectangle( RETICLE_LENGTH / _zoom, RETICLE_WIDTH / _zoom ), 0 );

    return cursor_drawing;
}

Camera & Camera::update( const Coordinate & _target, bool _hard_target_set )
{
    if( _hard_target_set )
    {
        center( _target );
    }
    else
    {
        target( _target );

        Vector movement = Vector( m_center, m_target ) * m_movement_ratio;
        m_cursor_world_position += movement;
        m_center += movement;
    }

    ++m_age;
    return *this;
}

Camera & Camera::capture( const Visible * _subject )
{
    if( _subject )
        m_subjects.insert_back( _subject );
    return *this;
}

uint Camera::age( ) const { return m_age; }

Planc Camera::width( ) const { return m_width; }
Camera & Camera::width( const Planc & _width )
{
    m_width = _width;
    return *this;
}

Planc Camera::height( ) const { return m_height; }
Camera & Camera::height( const Planc & _height )
{
    m_height = _height;
    return *this;
}

double Camera::zoom( ) const { return m_zoom; }
Camera & Camera::zoom( const double _zoom )
{
    if( !in_range( _zoom, MIN_ZOOM, MAX_ZOOM ) )
        return *this;

    Coordinate pre_target_offset = target_offset( );

    Coordinate cursor_screen_position = world_to_screen( m_cursor_world_position );

    m_zoom = _zoom;

    Coordinate post_target_offset = target_offset( );
    Coordinate d_target_offset = post_target_offset - pre_target_offset;

    m_target += d_target_offset;
    m_center += d_target_offset;

    m_cursor_world_position = screen_to_world( cursor_screen_position );

    return *this;
}

Coordinate Camera::center( ) const { return m_center; }
Camera & Camera::center( const Coordinate & _center )
{
    m_center = _center;
    return *this;
}

Coordinate Camera::target( ) const { return m_target; }
Camera & Camera::target( const Coordinate & _target, const bool _hard_set )
{
    m_target = _target;
    m_target += target_offset( );

    if( _hard_set )
        center( m_target );
    return *this;
}

Vector Camera::target_offset( ) const
{
    return Vector( 0.0, ( height( ) / 4.0 ) / zoom( ) );
}
