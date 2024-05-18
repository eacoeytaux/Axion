#include "Camera.hpp"

#include "OGL.hpp"

#include "Engine.hpp"
#include "Object.hpp"

namespace
{
const dec MIN_ZOOM = 0.64;
const dec MAX_ZOOM = 2.5;
const uint LIGHTING_LAYERS = 6;
const dec LIGHTING_RADIUS_GROW = 0.333;
const dec LIGHTING_RADIUS_GROW_EXPONENT = 0.88;
} // namespace

Camera::Camera( const Coordinate & _target, const Planc & _width, const Planc & _height, const dec _zoom ) : m_cursor_world_position( COORDINATE_INFINITY_NEGATIVE )
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

    const dec _screen_width = Engine::screen_width( );
    const dec _screen_height = Engine::screen_height( );

    const Coordinate _camera_offset = half( Vector( width( ), height( ) ) );
    const dec _zoom = Camera::zoom( );

    ogl::clear( );
    ogl::depth_always( );
    ogl::push_matrix( );
    {
        ogl::scale( 2.0 / _screen_width, 2.0 / _screen_height );
        ogl::translate( half( _screen_width ), half( _screen_height ) );
        ogl::translate( -half( width( ) ), -half( height( ) ) );

        Engine::anti_alias( ) ? ogl::enable_anti_alias( ) : ogl::disable_anti_alias( );

        Visible cursor_visible( cursor_drawing( ).move( m_cursor_world_position ) );
        capture( &cursor_visible );

        auto draw_convex_coordinates = [ & ]( const varray<Color> & _colors, const varray<Coordinate> & _coordinates, const Transform & _transform = IDENTITY_TRANSFORM, const dec _z = 0.0 )
        {
            if( !_colors.size( ) || !_coordinates.size( ) )
                return;

            ogl::push_matrix( );
            {
                ogl::translate( _transform.get( 0, 2 ), _transform.get( 1, 2 ) );
                ogl::transform( _transform );
                ogl::begin_polygons( );
                {
                    if( _colors.size( ) == 1 )
                    {
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
                }
                ogl::end( );
            }
            ogl::pop_matrix( );
        };

        m_subjects.sort( visible_sort );
        for_each( subject, m_subjects )
        {
            if( !subject || !subject->drawing( ).polygon_count( ) )
                continue;

            ogl::clear_depth( );
            ogl::push_matrix( );
            {
                if( const dec _z = subject->z( ) )
                {
                    ogl::scale( _zoom * _z );
                    ogl::translate( -center( ).x( ),
                                    -center( ).y( ) );
                }

                const Drawing & _drawing = subject->drawing( );
                ogl::translate( _drawing.center( ).x( ), _drawing.center( ).y( ) );

                for_each( _colored_polygon, _drawing.colored_polygons( ) )
                {
                    const Polygon & _polygon = _colored_polygon.polygon;

                    if( _colored_polygon.thickness == FILLED )
                    {
                        if( _drawing.has_border( ) )
                        {
                            // TODO

                            // const dec _border_width = _drawing.border_width( );
                            // const Color _border_color = _drawing.border_color( );

                            // Polygon polygon_border = Polygon::expand( _polygon, _border_width );

                            // for_each( polygon, polygon_border.convex_partitions( ) )
                            // {
                            //     draw_convex_coordinates( { _border_color }, polygon.coordinates_raw( ), polygon.transform( ) );
                            // }
                        }

                        const varray<Polygon> & _convex_polygons = _polygon.convex_partitions( );
                        for_range( _convex_polygons.size( ) )
                        {
                            const Polygon & _convex_polygon = _convex_polygons[ i ];

                            draw_convex_coordinates( _colored_polygon.colors, _convex_polygon.coordinates( true ), _convex_polygon.transform( ) ); // TODO need convex indices
                        }
                    }
                    else
                    {
                        const Transform _transform = _colored_polygon.polygon.transform( );
                        const dec _thickness = _colored_polygon.thickness / ( _colored_polygon.preserve_thickness ? _zoom : 1.0 );

                        const varray<Line> & _lines = _polygon.lines( );
                        for_range( _lines.size( ) )
                        {
                            const Line & _line = _lines[ i ];
                            const Angle _line_angle = _line.angle( );
                            const Vector _line_vector( _line.c1( ), _line.c2( ) );

                            const Line & _next_line = _lines[ ( i + 1 ) % _lines.size( ) ];
                            const Angle _next_line_angle = _next_line.angle( );
                            const Vector _next_line_vector( _next_line.c1( ), _next_line.c2( ) );

                            Polygon line_polygon;
                            if( _colored_polygon.extend_lines )
                            {
                                line_polygon = Rectangle( _line_vector.magnitude( ) + _thickness, _thickness, _line_vector.half( ), _line_angle );
                            }
                            else
                            {
                                line_polygon = Rectangle( _line_vector.magnitude( ), _thickness, _line_vector.half( ), _line_angle );
                            }

                            Polygon corner_polygon;
                            if( ( _line.c1( ) != _line.c2( ) ) && ( _next_line.c1( ) != _next_line.c2( ) ) )
                            {
                                Coordinate c0 = _line.c2( );
                                Coordinate c1 = c0 + VectorA( _line_angle - RIGHT_ANGLE, half( _thickness ) );
                                Coordinate c2 = c0 + VectorA( _next_line_angle - RIGHT_ANGLE, half( _thickness ) );
                                Coordinate c3 = Line( c1, c1 + VectorA( _line_angle ) ).intersection( Line( c2, c2 - VectorA( _next_line_angle ) ) );
                                corner_polygon = Polygon( { c0, c1, c3, c2 } );
                            }

                            if( _drawing.has_border( ) )
                            {
                                // TODO

                                // const dec _border_width = _drawing.border_width( );
                                // const Color _border_color = _drawing.border_color( );

                                // Polygon polygon_border_corner = Polygon::expand( _corner_polygon, _border_width );
                                // for_each( polygon, polygon_border_corner.convex_partitions( ) )
                                //{
                                //     draw_convex_coordinates( { _border_color }, polygon.coordinates( true ), polygon.transform( ) );
                                // }

                                // Polygon polygon_border = Polygon::expand( line_polygon, _border_width );
                                // for_each( polygon, polygon_border.convex_partitions( ) )
                                //{
                                //     draw_convex_coordinates( { _border_color }, polygon.coordinates( true ), polygon.transform( ) );
                                // }
                            }

                            draw_convex_coordinates( { _colored_polygon.colors[ i ] }, corner_polygon.coordinates( true ), corner_polygon.transform( ) );
                            draw_convex_coordinates( { _colored_polygon.colors[ i ] }, line_polygon.coordinates( true ), line_polygon.transform( ) );
                        }
                    }
                }
            }
            ogl::pop_matrix( );
        }

        if( m_lighting )
        {
            ogl::clear_depth( );
            ogl::depth_always( );

            draw_convex_coordinates( { Color::rgba( 0.0, 0.0, 0.5, 0.2 ) }, Rectangle( _screen_width, _screen_height ).coordinates( ) );

            const varray<LightSource> & _light_sources = m_lighting->light_sources( );

            for_range( LIGHTING_LAYERS + 1 )
            {
                ogl::clear_depth( );
                ogl::depth_always( );
                ogl::push_matrix( );
                {
                    ogl::scale( _zoom );
                    ogl::translate( -center( ).x( ),
                                    -center( ).y( ) );

                    for_each( light, _light_sources )
                    {
                        ogl::push_matrix( );
                        {
                            ogl::translate( light.position( ).x( ),
                                            light.position( ).y( ) );

                            draw_convex_coordinates( { TRANSPARENT },
                                                     Circle( light.radius( ) * ( ( i * LIGHTING_RADIUS_GROW * pow( LIGHTING_RADIUS_GROW_EXPONENT, i ) ) + 1 ) ).coordinates( ) );
                        }
                        ogl::pop_matrix( );
                    }
                }
                ogl::pop_matrix( );

                ogl::depth_not_equal( );
                draw_convex_coordinates( { BLACK.a( min( 1.0, ( (dec)( i + 1 ) / (dec)LIGHTING_LAYERS ) ) ) },
                                         Rectangle( _screen_width, _screen_height ).coordinates( ) );
            }
        }
    }
    ogl::pop_matrix( );

    return *this;
}

Drawing Camera::cursor_drawing( ) const
{
    const dec RETICLE_WIDTH = 2.0;
    const dec RETICLE_LENGTH = 8.0;
    const dec RETICLE_BORDER_WIDTH = 1.0;

    Drawing cursor_drawing;

    const dec _zoom = zoom( );
    cursor_drawing.draw( BLACK, Rectangle( ( RETICLE_LENGTH + ( RETICLE_BORDER_WIDTH * 2.0 ) ) / _zoom, ( RETICLE_WIDTH + ( RETICLE_BORDER_WIDTH * 2.0 ) ) / _zoom ), 0 );
    cursor_drawing.draw( BLACK, Rectangle( ( RETICLE_WIDTH + ( RETICLE_BORDER_WIDTH * 2.0 ) ) / _zoom, ( RETICLE_LENGTH + ( RETICLE_BORDER_WIDTH * 2.0 ) ) / _zoom ), 0 );
    cursor_drawing.draw( WHITE, Rectangle( RETICLE_WIDTH / _zoom, RETICLE_LENGTH / _zoom ), 0 );
    cursor_drawing.draw( WHITE, Rectangle( RETICLE_LENGTH / _zoom, RETICLE_WIDTH / _zoom ), 0 );

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

dec Camera::zoom( ) const { return m_zoom; }
Camera & Camera::zoom( const dec _zoom )
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
