#include "Camera.hpp"
#include "Engine.hpp"
#include "Object.hpp"
#include "OGL.hpp"

namespace
{
cdec MIN_ZOOM = 0.64;
cdec MAX_ZOOM = 2.5;
cdec TARGET_OFFSET_Y = 0.125;
cuint LIGHTING_LAYERS = 6;
cdec LIGHTING_RADIUS_GROW = 0.333;
cdec LIGHTING_RADIUS_GROW_EXPONENT = 0.88;
cdec DEFAULT_HUD_OFFSET = 0.025;
} // namespace

Camera::Camera( World * world, Planc cref _width, Planc cref _height, cdec _zoom ) : m_world( world )
{
    Assert( (bool)world );

    cursor_world_position_reset( );

    width( _width );
    height( _height );
    zoom( _zoom );

    hud_offset_percentage( DEFAULT_HUD_OFFSET );
    show_hud( false );
}

void Camera::clear_all( )
{
    clear_subjects( );
    clear_screen_effects( );
    clear_hud_elements( );
}

void Camera::clear_subjects( )
{
    for_each( subject, m_owned_subjects )
    {
        safe_delete( subject );
    }

    m_owned_subjects.clear( );
    m_subjects.clear( );

    #ifdef AXN_DEBUG
    for_each( subject, m_owned_debug_subjects )
    {
        safe_delete( subject );
    }

    m_owned_debug_subjects.clear( );
    m_debug_subjects.clear( );
    #endif
}

void Camera::clear_screen_effects( )
{
    for_each( effect, m_owned_screen_effects )
    {
        // safe_delete( effect );
    }

    m_owned_screen_effects.clear( );
    m_screen_effects.clear( );
}

void Camera::clear_hud_elements( )
{
    for_each( hud_element, m_owned_hud_elements )
    {
        // safe_delete( hud_element );
    }

    m_owned_hud_elements.clear( );
    m_hud_elements.clear( );
}

void Camera::render( )
{
    #ifdef AXN_DEBUG
    static uint total_render_count = 0;
    static uint total_polygon_count = 0;
    static uint max_polygon_count = 0;
    uint polygon_count = 0;
    uint convex_polygon_count = 0;
    uint convex_polygon_triangle_count = 0;
    #endif

    cdec screen_width = Engine::screen_width( );
    cdec screen_height = Engine::screen_height( );

    const Coordinate _camera_center = center( );

    cdec _zoom = Camera::zoom( );

    ogl::clear( );
    ogl::push_matrix( );
    {
        ogl::scale( 2.0 / screen_width, 2.0 / screen_height );

        Engine::anti_alias( ) ? ogl::enable_anti_alias( ) : ogl::disable_anti_alias( );

        auto render_convex_polygon = [ & ] ( const varray<Color> & _colors, varray<Coordinate> cref _coordinates )
        {
            if( !_colors.size( ) || !_coordinates.size( ) )
            {
                return;
            }

            #ifdef AXN_DEBUG
            ++polygon_count;
            #endif

            ogl::begin_polygons( );
            {
                if( _colors.size( ) == 1 )
                {
                    ogl::color( _colors[ 0 ].r( ), _colors[ 0 ].g( ), _colors[ 0 ].b( ), _colors[ 0 ].a( ) );
                    for_range( i, _coordinates.size( ) )
                    {
                        ogl::vertex( _coordinates[ i ].x( ), _coordinates[ i ].y( ), 0.0 );
                    }
                }
                else
                {
                    for_range( i, _coordinates.size( ) )
                    {
                        ogl::color( _colors[ i ].r( ), _colors[ i ].g( ), _colors[ i ].b( ), _colors[ i ].a( ) );
                        ogl::vertex( _coordinates[ i ].x( ), _coordinates[ i ].y( ), 0.0 );
                    }
                }
            }
            ogl::end( );
        };

        auto render_visible = [ & ] ( Visible * visible, bool fixed )
        {
            if( !visible || !visible->polygon_count( ) )
            {
                return;
            }

            Drawing cref _drawing = *visible;

            // ogl::clear_depth( );
            ogl::depth_always( );

            // ogl::clear_stencil( );
            ogl::stencil_always( );

            ogl::push_matrix( );
            {
                if( !fixed )
                {
                    if( cdec _z = visible->z( ) )
                    {
                        ogl::scale( _zoom * _z );
                        ogl::translate( -_camera_center.x( ),
                                        -_camera_center.y( ) );
                    }
                }

                Transform cumulative_transform = _drawing.cumulative_transform( );

                ogl::translate( cumulative_transform.translation_x( ), cumulative_transform.translation_y( ) );
                ogl::transform( cumulative_transform );

                ogl::translate( _drawing.center( ).x( ), _drawing.center( ).y( ) );

                for_each( _colored_polygon, _drawing.colored_polygons( false ) )
                {
                    if( _colored_polygon.reset )
                    {
                        ogl::clear_stencil( );
                        ogl::stencil_always( );
                        ogl::stencil_mask( false );
                    }
                    else
                    {
                        if( _colored_polygon.fill )
                        {
                            ogl::stencil_add( );
                            ogl::stencil_always( );
                            ogl::stencil_mask( true );
                        }
                        else if( _colored_polygon.hole )
                        {
                            ogl::stencil_remove( );
                            ogl::stencil_always( );
                            ogl::stencil_mask( true );
                        }
                        else
                        {
                            ogl::stencil_mask( false );
                        }

                        if( _colored_polygon.thickness == FILLED )
                        {
                            ogl::push_matrix( );

                            Transform cumulative_transform = _colored_polygon.polygon.cumulative_transform( );

                            ogl::translate( cumulative_transform.translation_x( ), cumulative_transform.translation_y( ) );
                            ogl::transform( cumulative_transform );

                            if( _colored_polygon.polygon.convex( ) )
                            {
                                render_convex_polygon( _colored_polygon.colors, _colored_polygon.polygon.coordinates( false ) );
                            }
                            else
                            {
                                varray<Coordinate> cref cs = _colored_polygon.polygon.coordinates( false );
                                const varray<varray<uint>> & t = _colored_polygon.polygon.triangle_indices( );

                                for_range( i, t.size( ) )
                                {
                                    render_convex_polygon( { _colored_polygon.colors[ t[ i ][ 0 ] ],
                                                             _colored_polygon.colors[ t[ i ][ 1 ] ],
                                                             _colored_polygon.colors[ t[ i ][ 2 ] ] },
                                                           { cs[ t[ i ][ 0 ] ],
                                                             cs[ t[ i ][ 1 ] ],
                                                             cs[ t[ i ][ 2 ] ] } );
                                }

                                #ifdef AXN_DEBUG
                                ++convex_polygon_count;
                                convex_polygon_triangle_count += t.size( );
                                #endif
                            }

                            ogl::pop_matrix( );
                        }
                        else
                        {
                            cdec _thickness = _colored_polygon.preserve_thickness ? ( _colored_polygon.thickness / _zoom ) : ( _colored_polygon.thickness );

                            // TODO not raw!
                            varray<Coordinate> cref cs = _colored_polygon.polygon.coordinates( true );
                            for_range( i, cs.size( ) )
                            {
                                const Line _line = Line( cs[ i ? ( i - 1 ) : ( cs.size( ) - 1 ) ], cs[ i ] );
                                const Angle _line_angle = _line.angle( );
                                const Vector _line_vector( _line.c1( ), _line.c2( ) );

                                const Line _next_line = Line( cs[ i ], cs[ ( i == cs.size( ) - 1 ) ? 0 : ( i + 1 ) ] );
                                const Angle _next_line_angle = _next_line.angle( );
                                const Vector _next_line_vector( _next_line.c1( ), _next_line.c2( ) );

                                Polygon line_polygon;
                                if( _colored_polygon.extend_lines )
                                {
                                    line_polygon = Polygon::rectangle( _line_vector.magnitude( ) + _thickness, _thickness, half( _line_vector ), _line_angle );
                                }
                                else
                                {
                                    line_polygon = Polygon::rectangle( _line_vector.magnitude( ), _thickness, half( _line_vector ), _line_angle );
                                }

                                Polygon corner_polygon;
                                if( ( _line.c1( ) != _line.c2( ) ) && ( _next_line.c1( ) != _next_line.c2( ) ) )
                                {
                                    Coordinate c0 = _line.c2( );
                                    Coordinate c1 = c0 + Vector::A( _line_angle - RIGHT_ANGLE, half( _thickness ) );
                                    Coordinate c2 = c0 + Vector::A( _next_line_angle - RIGHT_ANGLE, half( _thickness ) );
                                    Coordinate c3 = Line( c1, c1 + Vector::A( _line_angle ) ).intersection( Line( c2, c2 - Vector::A( _next_line_angle ) ) );
                                    corner_polygon = Polygon( { c0, c1, c3, c2 } );
                                }

                                render_convex_polygon( { _colored_polygon.colors[ i ] }, corner_polygon.coordinates( ) );
                                render_convex_polygon( { _colored_polygon.colors[ i ] }, line_polygon.coordinates( ) );
                            }
                        }

                        if( _colored_polygon.fill || _colored_polygon.hole )
                        {
                            ogl::stencil_equal( );
                        }
                    }
                }
            }
            ogl::pop_matrix( );
        };

        auto render_subjects = [ & ] ( )
        {
            m_subjects.sort( Visible::sort, true );
            for_each( visible, m_subjects )
            {
                render_visible( visible, false );
            }
        };

        auto render_lighting = [ & ] ( )
        {
            if( m_world->current_room( )->lighting_active( ) )
            {
                if( const Lighting * lighting = m_world->current_room( )->lighting( ) )
                {
                    if( lighting->ambient_color( ).a( ) )
                    {
                        ogl::depth_always( );
                        render_convex_polygon( { lighting->ambient_color( ) }, Polygon::rectangle( screen_width, screen_height ).coordinates( ) );
                    }

                    const varray<LightSource> & _light_sources = lighting->light_sources( );

                    if( _light_sources.size( ) )
                    {
                        ogl::push_matrix( );
                        {
                            ogl::scale( _zoom );
                            ogl::translate( -_camera_center.x( ),
                                            -_camera_center.y( ) );

                            Polygon cover = Polygon::rectangle( screen_width / _zoom, screen_height / _zoom, _camera_center );

                            for_range( i, LIGHTING_LAYERS + 1 )
                            {
                                ogl::clear_depth( );
                                ogl::depth_always( );

                                for_each( light, _light_sources )
                                {
                                    render_convex_polygon( { TRANSPARENT },
                                                           Polygon::circle( light.radius( ) * ( ( i * LIGHTING_RADIUS_GROW * pow( LIGHTING_RADIUS_GROW_EXPONENT, i ) ) + 1 ), light.position( ) ).coordinates( ) );
                                }

                                ogl::depth_not_equal( );

                                render_convex_polygon( { BLACK.a( min( 1.0, ( (dec)( i + 1 ) / (dec)LIGHTING_LAYERS ) ) * lighting->darkness_intensity( ) ) }, cover.coordinates( ) );
                            }
                        }
                        ogl::pop_matrix( );
                    }
                }
            }
        };

        auto render_world_bounds = [ & ] ( )
        {
            #ifdef AXN_DEBUG
            const Color COLOR = BLACK.a( 0.5 );
            #else
            const Color COLOR = WHITE;
            #endif

            ogl::clear_depth( );
            ogl::depth_not_equal( );

            Polygon world_bounds_polygon = m_world->current_room( )->bounds( ) - Vector( _camera_center );
            world_bounds_polygon.scale( _zoom );

            render_convex_polygon( { TRANSPARENT }, world_bounds_polygon.coordinates( ) );
            render_convex_polygon( { COLOR }, Polygon::rectangle( screen_width, screen_height ).coordinates( ) );
        };

        auto render_camera_bounds = [ & ] ( )
        {
            #ifdef AXN_DEBUG
            const Color COLOR = ( Debug::active && m_draw_debug ) ? BLACK.a( 0.5 ) : BLACK;
            #else
            const Color COLOR = BLACK;
            #endif

            ogl::clear_depth( );
            ogl::depth_not_equal( );

            Polygon camera_bounds_polygon = FixedRectangle( width( ), height( ) );

            render_convex_polygon( { TRANSPARENT }, camera_bounds_polygon.coordinates( ) );
            render_convex_polygon( { COLOR }, Polygon::rectangle( screen_width, screen_height ).coordinates( ) );
        };

        auto render_screen_effects = [ & ] ( )
        {
            for_each( screen_effect, m_screen_effects )
            {
                screen_effect->render( this );
                render_visible( screen_effect, true );
            }
        };

        auto render_hud_elements = [ & ] ( )
        {
            if( show_hud( ) )
            {
                for_each( hud_element, m_hud_elements )
                {
                    hud_element->render( this );
                    render_visible( hud_element, true );
                }
            }
        };

        auto render_cursor = [ & ] ( )
        {
            if( is_num( cursor_world_position( ).x( ) ) && is_num( cursor_world_position( ).y( ) ) )
            {
                Drawing cursor = cursor_drawing( );
                cursor.scale( inverse( _zoom ) );
                cursor.move( Vector( cursor_world_position( ) ) );

                render_visible( new Visible( cursor ), false );
            }
        };

        auto render_debug_elements = [ & ] ( )
        {
            #ifdef AXN_DEBUG
            if( Debug::active )
            {
                for_each( visible, m_debug_subjects )
                {
                    render_visible( visible, false );
                }

                if( m_draw_debug )
                {
                    render_visible( new Visible( debug_overlay_drawing( ) ), true );
                }
            }
            #endif
        };

        render_subjects( );
        render_lighting( );
        render_world_bounds( );
        render_screen_effects( );
        render_debug_elements( );
        render_camera_bounds( );
        render_hud_elements( );
        render_cursor( );
    }
    ogl::pop_matrix( );

    #ifdef AXN_DEBUG
    ++total_render_count;
    total_polygon_count += polygon_count;
    max_polygon_count = max( polygon_count, max_polygon_count );
    Log( DEBUG_LOG, "polygons: %u ( convex: %u -> %u ) / average: %u / max: %u",
         polygon_count,
         convex_polygon_count,
         convex_polygon_triangle_count,
         ( total_polygon_count / total_render_count ),
         max_polygon_count );
    #endif
}

Drawing Camera::cursor_drawing( ) const
{
    static_setup( Drawing, cursor_drawing )
    {
        cdec RETICLE_WIDTH = 1.5;
        cdec RETICLE_LENGTH = 8.0;
        cdec RETICLE_BORDER_WIDTH = 1.0;

        cursor_drawing.draw( BLACK, Polygon::rectangle( RETICLE_LENGTH + ( RETICLE_BORDER_WIDTH * 2.0 ),
                                                        RETICLE_WIDTH + ( RETICLE_BORDER_WIDTH * 2.0 ) ) );
        cursor_drawing.draw( BLACK, Polygon::rectangle( RETICLE_WIDTH + ( RETICLE_BORDER_WIDTH * 2.0 ),
                                                        RETICLE_LENGTH + ( RETICLE_BORDER_WIDTH * 2.0 ) ) );
        cursor_drawing.draw( WHITE, Polygon::rectangle( RETICLE_WIDTH, RETICLE_LENGTH ) );
        cursor_drawing.draw( WHITE, Polygon::rectangle( RETICLE_LENGTH, RETICLE_WIDTH ) );
    }

    return cursor_drawing;
}

#ifdef AXN_DEBUG
Drawing Camera::debug_overlay_drawing( ) const
{
    cPlanc BORDER_LINE_THICKNESS = 1.5;
    cPlanc CROSSHAIR_LINE_THICKNESS = 1.0;
    cPlanc FPS_LINE_THICKNESS = 1.5;
    const Angle DELTA = TAU / (dec)Engine::FPS;
    cPlanc TARGET_RADIUS = 2.0;
    cPlanc FPS_RADIUS = 32.0;
    const Color MAIN_COLOR = WHITE;
    const Color TARGET_COLOR = RED;
    cdec COLOR_OPACITY = 1.0;

    cPlanc FPS_LINE_THICKNESS_ZOOM = FPS_LINE_THICKNESS * zoom( );
    cPlanc CROSSHAIR_LINE_THICKNESS_ZOOM = CROSSHAIR_LINE_THICKNESS * zoom( );

    static Angle delta;
    delta -= DELTA;

    cPlanc _width = width( );
    cPlanc _height = height( );

    cPlanc _half_width = half( _width );
    cPlanc _half_height = half( _height );

    const Vector _target_offset = target( ) - center( );

    const Polygon _target_outer = Polygon::circle( TARGET_RADIUS + FPS_LINE_THICKNESS, _target_offset );
    const Polygon _target_inner = Polygon::circle( TARGET_RADIUS, _target_offset );
    const Polygon _target_cover = Polygon::circle( TARGET_RADIUS );

    const Line _fps_line = Line( ORIGIN, Coordinate( 0.0, FPS_RADIUS ).rotate( delta ) );
    const Polygon _fps_circle = Polygon::circle( FPS_RADIUS );
    const Polygon _fps_dot = Polygon::circle( TARGET_RADIUS );

    Drawing overlay_drawing;

    overlay_drawing.draw( MAIN_COLOR,
                          Line( Coordinate( -_half_width, -_half_height ),
                                Coordinate( _half_width, -_half_height ) ),
                          BORDER_LINE_THICKNESS, true, true );
    overlay_drawing.draw( MAIN_COLOR,
                          Line( Coordinate( -_half_width, _half_height ),
                                Coordinate( _half_width, _half_height ) ),
                          BORDER_LINE_THICKNESS, true, true );
    overlay_drawing.draw( MAIN_COLOR,
                          Line( Coordinate( -_half_width, -_half_height ),
                                Coordinate( -_half_width, _half_height ) ),
                          BORDER_LINE_THICKNESS, true, true );
    overlay_drawing.draw( MAIN_COLOR,
                          Line( Coordinate( _half_width, -_half_height ),
                                Coordinate( _half_width, _half_height ) ),
                          BORDER_LINE_THICKNESS, true, true );

    overlay_drawing.draw( MAIN_COLOR.a( COLOR_OPACITY ),
                          Line( Coordinate( -_half_width, 0.0 ),
                                Coordinate( _half_width, 0.0 ) ),
                          CROSSHAIR_LINE_THICKNESS_ZOOM, true );
    overlay_drawing.draw( MAIN_COLOR.a( COLOR_OPACITY ),
                          Line( Coordinate( 0.0, -_half_height ),
                                Coordinate( 0.0, _half_height ) ),
                          CROSSHAIR_LINE_THICKNESS_ZOOM, true );

    overlay_drawing.draw( MAIN_COLOR, _target_outer, FILLED );
    overlay_drawing.draw( TARGET_COLOR, _target_inner, FILLED );
    overlay_drawing.draw( MAIN_COLOR, _target_cover, FILLED );

    overlay_drawing.draw( MAIN_COLOR, _fps_dot, FILLED );
    overlay_drawing.draw( MAIN_COLOR.a( COLOR_OPACITY ), _fps_circle, FPS_LINE_THICKNESS_ZOOM, true );
    overlay_drawing.draw( MAIN_COLOR.a( COLOR_OPACITY ), _fps_line, FPS_LINE_THICKNESS_ZOOM, true );

    return overlay_drawing;
}
#endif

void Camera::update( )
{
    ++m_age;

    if( m_center != m_target )
    {
        Vector movement = Vector( m_center, m_target ) * m_movement_ratio;

        center( center( ) + movement );

        cursor_world_position( cursor_world_position( ) + movement );
    }
}

void Camera::capture( Visible * _subject, cbool _should_delete )
{
    Assert( (bool)_subject );

    m_subjects.insert_back( _subject );

    if( _should_delete ) { m_owned_subjects.insert_back( _subject ); }
}

void Camera::add_hud_element( HeadUpDisplay * _hud_element, cbool _should_delete )
{
    Assert( (bool)_hud_element );

    m_hud_elements.insert_back( _hud_element );

    if( _should_delete ) { m_owned_hud_elements.insert_back( _hud_element ); }
}

void Camera::remove_hud_element( HeadUpDisplay * _hud_element )
{
    Assert( (bool)_hud_element );

    m_hud_elements.remove( _hud_element );

    if( m_owned_hud_elements.contains( _hud_element ) ) { m_owned_hud_elements.remove( _hud_element ); }
}

void Camera::add_screen_effect( ScreenEffect * _effect, cbool _should_delete )
{
    Assert( (bool)_effect );

    m_screen_effects.insert_back( _effect );

    if( _should_delete ) { m_owned_screen_effects.insert_back( _effect ); }
}

void Camera::remove_screen_effect( ScreenEffect * _effect )
{
    Assert( (bool)_effect );

    m_screen_effects.remove( _effect );

    if( m_owned_screen_effects.contains( _effect ) ) { m_owned_screen_effects.remove( _effect ); }
}

#ifdef AXN_DEBUG
void Camera::capture_debug( Visible * _subject, cbool _should_delete )
{
    Assert( (bool)_subject );

    m_debug_subjects.insert_back( _subject );

    if( _should_delete ) { m_owned_debug_subjects.insert_back( _subject ); }
}
#endif

Planc Camera::width( ) const { return m_width; }
void Camera::width( Planc cref _width ) { m_width = _width; }

Planc Camera::height( ) const { return m_height; }
void Camera::height( Planc cref _height ) { m_height = _height; }

dec Camera::zoom( ) const { return m_zoom; }
void Camera::zoom( cdec _zoom )
{
    Assert( _zoom, "zoom cannot be zero" );

    dec zoom = max( MIN_ZOOM, min( MAX_ZOOM, _zoom ) );

    Coordinate pre_target_offset = target_offset( );
    Coordinate cursor_screen_position = world_to_screen( cursor_world_position( ) );

    m_zoom = zoom;

    Coordinate post_target_offset = target_offset( );
    Coordinate d_target_offset = post_target_offset - pre_target_offset;

    m_target += d_target_offset;
    m_center += d_target_offset;

    cursor_world_position( screen_to_world( cursor_screen_position ) );
}

dec Camera::min_zoom( ) const { return MIN_ZOOM; }
dec Camera::max_zoom( ) const { return MAX_ZOOM; }

bool Camera::show_hud( ) const { return m_show_hud; }
void Camera::show_hud( cbool _show_hud ) { m_show_hud = _show_hud; }

dec Camera::hud_offset_percentage( ) const { return m_hud_offset_percentage; }
void Camera::hud_offset_percentage( cdec _hud_offset_percentage ) { m_hud_offset_percentage = _hud_offset_percentage; }

Coordinate Camera::center( ) const { return m_center; }
void Camera::center( Coordinate cref _center ) { m_center = _center; }

FixedRectangle Camera::bounds( ) const { return FixedRectangle( width( ), height( ), center( ) ); }

Coordinate Camera::target( ) const { return m_target; }
void Camera::target( Coordinate cref _target, cbool _hard_set )
{
    m_target = _target;
    m_target += target_offset( );

    if( _hard_set ) { center( m_target ); }
}

Vector Camera::target_offset( ) const { return Vector( 0.0, ( height( ) * TARGET_OFFSET_Y ) / zoom( ) ); }

bool Camera::in_view( Coordinate cref _world_position, cdec _z ) const
{
    return_true_if( !_z );

    Coordinate screen_position = world_to_screen( _world_position );
    return in_range( screen_position.x( ), width( ) / _z ) && in_range( screen_position.y( ), height( ) / _z );
}

bool Camera::in_view( FixedRectangle cref _world_bounding_box, cdec _z ) const
{
    return_true_if( !_z );

    FixedRectangle bounds( world_to_screen( _world_bounding_box.bottom( ) ), world_to_screen( _world_bounding_box.top( ) ) );
    FixedRectangle screen( width( ) / _z, height( ) / _z, world_to_screen( center( ) ) );

    return screen.has_intersection_with( bounds );
}

Coordinate Camera::screen_to_world( Coordinate cref _screen_position ) const
{
    Coordinate world_position = _screen_position;

    world_position += center( ) - half( Vector( Engine::screen_width( ), Engine::screen_height( ) ) );
    world_position.y( -world_position.y( ) + ( center( ).y( ) * 2.0 ) );
    world_position = Vector( center( ), world_position ) / zoom( );

    return world_position;
}

Coordinate Camera::world_to_screen( Coordinate cref _world_position ) const
{
    Coordinate screen_position = _world_position;

    screen_position = Vector( center( ), screen_position ) * zoom( );
    screen_position.y( -screen_position.y( ) + ( center( ).y( ) * 2.0 ) );
    screen_position -= center( ) - half( Vector( Engine::screen_width( ), Engine::screen_height( ) ) );

    return screen_position;
}

Coordinate Camera::cursor_world_position( ) { return m_cursor_world_position; }
void Camera::cursor_world_position( Coordinate cref _cursor_world_position ) { m_cursor_world_position = _cursor_world_position; }

void Camera::cursor_world_position_reset( ) { m_cursor_world_position = INVALID_COORDINATE; }

Camera::HeadUpDisplay::HeadUpDisplay( cdec _center_x_percent, cdec _center_y_percent, cdec _width_percent, cdec _height_percent )
    : m_center_x_percent( _center_x_percent ), m_center_y_percent( _center_y_percent ), m_width_percent( _width_percent ), m_height_percent( _height_percent )
{
    persist_render( false );
    needs_render_always( true );
}

FixedRectangle Camera::HeadUpDisplay::bounds( Camera * camera ) const
{
    Planc width = camera->width( );
    Planc height = camera->height( );

    Planc offset = min( width, height ) * camera->hud_offset_percentage( );

    width -= ( offset * 2.0 );
    height -= ( offset * 2.0 );

    Planc display_width = width * m_width_percent;
    Planc display_height = height * m_height_percent;
    Coordinate center( width * ( m_center_x_percent - 0.5 ), height * ( m_center_y_percent - 0.5 ) );

    return FixedRectangle( display_width, display_height, center );
}

Camera::ScreenEffect::ScreenEffect( )
{
    persist_render( false );
    needs_render_always( true );
}
