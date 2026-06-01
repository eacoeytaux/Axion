#include "axn/graphics/Camera.hpp"

#include "axn/system/OGL.hpp"
#include "axn/system/Engine.hpp"
#include "axn/world/Object.hpp"

var_const( dec MIN_ZOOM = ( 0.64 ) );
var_const( dec MAX_ZOOM = ( 2.5 ) );

var_const( dec DEFAULT_TARGET_OFFSET_Y = ( 0.125 ) );

var_const( uint LIGHTING_LAYERS = ( 6 ) );
var_const( dec LIGHTING_RADIUS_GROW = ( 0.333 ) );
var_const( dec LIGHTING_RADIUS_GROW_EXPONENT = ( 0.88 ) );

var_const( dec DEFAULT_HUD_OFFSET = ( 0.025 ) );

section( TRANSITION )
{
    var_const( ticks DEFAULT_TRANSITION_DURATION = ( 20.0 ) );
    var_const( ticks DEFAULT_TRANSITION_COMPLETE_DURATION = ( 4.0 ) );
}

section( TRACKING )
{
    #if defined( AXN_DEBUG )
    // #define TRACK_POLYGONS
    #endif

    #if defined( TRACK_POLYGONS )
    var_static( uint total_render_count = ( 0 ) );
    var_static( uint total_polygon_count = ( 0 ) );
    var_static( uint total_point_count = ( 0 ) );
    var_static( uint polygon_count = ( 0 ) );
    var_static( uint point_count = ( 0 ) );
    var_static( uint max_polygon_count = ( 0 ) );
    #endif
}

section( RETICLE )
{
    var_const( dec RETICLE_WIDTH = ( 1.5 ) );
    var_const( dec RETICLE_LENGTH = ( 8.0 ) );
    var_const( dec RETICLE_BORDER_WIDTH = ( 1.0 ) );
}

#if defined( AXN_DEBUG )
section( DEBUG_OVERLAY )
{
    var_const( Planc BORDER_LINE_THICKNESS = ( 1.5 ) );
    var_const( Planc CROSSHAIR_LINE_THICKNESS = ( 1.0 ) );
    var_const( Planc FPS_LINE_THICKNESS = ( 1.5 ) );
    var_const( Angle DELTA = ( Angle::fraction( Engine::FPS ) ) );
    var_const( Planc TARGET_RADIUS = ( 2.0 ) );
    var_const( Planc FPS_RADIUS = ( 32.0 ) );
    var_const( Color MAIN_COLOR = ( WHITE ) );
    var_const( Color TARGET_COLOR = ( RED ) );
    var_const( dec COLOR_OPACITY = ( 1.0 ) );
    var_const( dec LAG_COLOR_OPACITY = ( 0.3 ) );
}
#endif

section( PAUSE_HUD )
{
    var_const( dec PAUSE_WIDTH = ( 0.06 ) );
    var_const( dec PAUSE_HEIGHT = ( 0.1 ) );
    var_const( dec PAUSE_GAP = ( 0.1111 ) );
    var_const( ticks PLAY_SHOW = ( 30.0 ) );
    var_const( ticks PLAY_FADE = ( 120.0 ) );

    class PauseHUD : public Camera::HUD
    {

    public:

        PauseHUD( ) : HUD( half( PAUSE_WIDTH ), one_minus( half( PAUSE_HEIGHT ) ), PAUSE_WIDTH, PAUSE_HEIGHT ) { }

        void render( ptr<Camera> camera ) override
        {
            HUD::render( camera );

            if( Engine::paused( ) )
            {
                m_play_show_countdown.reset( PLAY_SHOW );
                m_play_fade_countdown.reset( PLAY_FADE );

                FixedRectangle rect = FixedRectangle( bounds( camera ).width( ) * half( one_minus( PAUSE_GAP ) ),
                                                      bounds( camera ).height( ),
                                                      bounds( camera ).center( ) );

                Vector offset = VectorX( half( rect.width( ) ) + half( bounds( camera ).width( ) * PAUSE_GAP ) );

                draw( WHITE, rect + offset );
                draw( WHITE, rect - offset );
            }
            else
            {
                if( !m_play_show_countdown.tick( ) || !m_play_fade_countdown.tick( ) )
                {
                    Polygon tri = Polygon( { bounds( camera ).top_left( ),
                                             bounds( camera ).bottom_left( ),
                                             midpoint( bounds( camera ).top_right( ),
                                                       bounds( camera ).bottom_right( ) ) } );

                    draw( WHITE.a( is_pos( m_play_show_countdown.percentage( ) ) ? 1.0 : m_play_fade_countdown.percentage( ) ), tri );
                }
            }
        }

    private:

        Countdown m_play_show_countdown;
        Countdown m_play_fade_countdown;

    };

    var_static( PauseHUD pause_hud );
}

Camera::Camera( ptr<World> in_world, Planc cref in_width, Planc cref in_height, dec in_zoom ) : m_world( in_world )
{
    Assert( !is_null( in_world ) );

    cursor_world_position_reset( );
    width( in_width );
    height( in_height );
    zoom( in_zoom );

    target_offset_ratio_y( DEFAULT_TARGET_OFFSET_Y );

    hud_offset_percentage( DEFAULT_HUD_OFFSET );

    add_always_hud_element( &pause_hud );

    #if defined( AXN_DEBUG )
    show_hud( false );
    #endif

    transition_duration( DEFAULT_TRANSITION_DURATION, DEFAULT_TRANSITION_COMPLETE_DURATION );
}

void Camera::clear_all( )
{
    clear_subjects( );
    clear_screen_effects( );
    clear_hud_elements( );
}

void Camera::clear_subjects( )
{
    m_subjects.clear( );
    m_background_subjects.clear( );
    m_foreground_subjects.clear( );

    safe_delete( m_owned_subjects );
    safe_delete( m_background_owned_subjects );
    safe_delete( m_foreground_owned_subjects );

    #if defined( AXN_DEBUG )
    m_debug_subjects.clear( );
    safe_delete( m_owned_debug_subjects );
    #endif
}

void Camera::clear_screen_effects( )
{
    m_screen_effects.clear( );
    safe_delete( m_owned_screen_effects );
}

void Camera::clear_hud_elements( )
{
    m_hud_elements.clear( );
    safe_delete( m_hud_elements );
}

void render_convex_polygon( varray<Color> cref in_colors, varray<Point> cref in_points )
{
    return_if( !in_colors.size( ) || !in_points.size( ) );

    #if defined( TRACK_POLYGONS )
    polygon_count += 1;
    point_count += in_points.size( );
    #endif

    ogl::begin_polygons( );
    {
        bool single_color = is_one( in_colors.size( ) );

        for_range( i, in_points.size( ) )
        {
            if( !single_color || is_zero( i ) )
            {
                ogl::color( Settings::get( Settings::GRAPHICS_GRAYSCALE ) ? Color::grayscale( in_colors[ i ] ) : in_colors[ i ] );
            }

            ogl::vertex( in_points[ i ].x( ), in_points[ i ].y( ), 0.0 );
        }

    }
    ogl::end( );
}

void render_colored_polygon( Drawing::ColoredPolygon cref in_colored_polygon, dec in_zoom )
{
    // todo? Assert( in_colored_polygon.polygon.sides( ) >= 3 );

    Polygon cref p = in_colored_polygon.polygon;
    varray<Point> cref pts = in_colored_polygon.polygon.points( true );
    varray<Color> cref color = in_colored_polygon.colors;

    if( in_colored_polygon.thickness == FILLED )
    {
        if( p.convex( ) )
        {
            render_convex_polygon( color, pts );
        }
        else
        {
            for_each( t, p.triangle_indices( ) )
            {
                varray<Color> colors;

                if( is_one( color.size( ) ) )
                {
                    colors = { color[ 0 ] };
                }
                else
                {
                    colors = { color[ t[ 0 ] ],
                               color[ t[ 1 ] ],
                               color[ t[ 2 ] ] };
                }

                render_convex_polygon( colors,
                                       { pts[ t[ 0 ] ],
                                         pts[ t[ 1 ] ],
                                         pts[ t[ 2 ] ] } );
            }
        }
    }
    else
    {
        const dec c_thickness = ( in_colored_polygon.thickness / ( ( in_colored_polygon.preserve_thickness && ( pts.size( ) > 2 ) ) ? ( in_zoom ) : ( 1.0 ) ) );

        for_range( i, pts.size( ) )
        {
            const Line c_line = Line( pts[ i ? ( i - 1 ) : ( pts.size( ) - 1 ) ], pts[ i ] );
            const Angle c_line_angle( c_line.angle( ) );
            const Vector c_line_vector( c_line.pt1( ), c_line.pt2( ) );

            const Line c_next_line = Line( pts[ i ], pts[ ( i == pts.size( ) - 1 ) ? 0 : ( i + 1 ) ] );
            const Angle c_next_line_angle( c_next_line.angle( ) );
            const Vector c_next_line_vector( c_next_line.pt1( ), c_next_line.pt2( ) );

            const Polygon c_line_polygon = Polygon::rectangle( ( c_line_vector.magnitude( ) + ( in_colored_polygon.extend_lines ? c_thickness : 0.0 ) ), c_thickness, half( c_line_vector ), c_line_angle );

            render_convex_polygon( { color[ i ] }, c_line_polygon.points( ) );

            if( ( c_line.pt1( ) != c_line.pt2( ) ) && ( c_next_line.pt1( ) != c_next_line.pt2( ) ) )
            {
                Point pt1 = c_line.pt2( );
                Point pt2 = pt1 + VectorA( c_line_angle - RIGHT, half( c_thickness ) );
                Point pt3 = pt1 + VectorA( c_next_line_angle - RIGHT, half( c_thickness ) );
                Point pt4 = Line( pt2, pt2 + VectorA( c_line_angle ) ).intersection( Line( pt3, pt3 - VectorA( c_next_line_angle ) ) );

                // todo render_convex_polygon( { color[ i ] }, { pt1, pt2, pt4, pt3 } );
            }
        }
    }
}

void render_nodes( varray<Drawing::Node> cref in_drawing_nodes,
                   varray<Drawing::ColoredPolygon> cref in_crop_polygons,
                   varray<Drawing::ColoredPolygon> cref in_hole_polygons,
                   dec in_zoom )
{
    return_if( in_drawing_nodes.empty( ) );

    varray<Drawing::Node> drawing_nodes = in_drawing_nodes;
    Drawing::Node drawing_node = drawing_nodes.back( );
    drawing_nodes.remove_back( );

    ogl::matrix m;
    {
        ogl::translate( drawing_node.transform.translation_x( ), drawing_node.transform.translation_y( ) );
        ogl::transform( drawing_node.transform );

        // todo should be normal Polygons
        varray<Drawing::ColoredPolygon> crop_polygons = in_crop_polygons;
        varray<Drawing::ColoredPolygon> hole_polygons = in_hole_polygons;

        crop_polygons += drawing_node.crop_shapes;
        hole_polygons += drawing_node.hole_shapes;

        if( crop_polygons.size( ) || hole_polygons.size( ) )
        {
            ogl::clear_stencil( );

            ogl::stencil_mask( true );

            ogl::stencil_always( );

            ogl::stencil_add( );

            for_each( polygon, crop_polygons )
            {
                render_colored_polygon( polygon, in_zoom );
            }

            if( crop_polygons.size( ) )
            {
                ogl::stencil_remove( );
            }

            for_each( polygon, hole_polygons )
            {
                render_colored_polygon( polygon, in_zoom );
            }

            if( crop_polygons.size( ) )
            {
                ogl::stencil_equal( );
            }
            else
            {
                ogl::stencil_not_equal( );
            }

            ogl::stencil_mask( false );
        }
        else
        {
            ogl::stencil_always( );
        }

        render_nodes( drawing_nodes, crop_polygons, hole_polygons, in_zoom );

        for_each( drawing, drawing_node.drawings )
        {
            render_nodes( drawing, crop_polygons, hole_polygons, in_zoom );
        }

        for_each( colored_polygon, drawing_node.colored_shapes )
        {
            render_colored_polygon( colored_polygon, in_zoom );
        }
    }
}

void render_drawing( Drawing cref in_drawing, dec in_zoom )
{
    render_nodes( in_drawing.nodes( ), varray<Drawing::ColoredPolygon>( ), varray<Drawing::ColoredPolygon>( ), in_zoom );
}

void Camera::render( )
{
    #if defined( TRACK_POLYGONS )
    polygon_count = 0;
    point_count = 0;
    #endif

    const dec c_screen_width = Engine::screen_width( );
    const dec c_screen_height = Engine::screen_height( );

    const FixedRectangle c_screen = Engine::screen_bounds( );

    const Point c_camera_center = center( );

    const dec c_zoom = zoom( );

    Color lens_bounds_color = BLACK;
    Color world_bounds_color = BLACK;

    #if defined( AXN_DEBUG )
    world_bounds_color = RED;
    if( Debug::active && Settings::get( Settings::DEBUG_CAMERA_DISPLAY ) )
    {
        world_bounds_color.a( 0.5 );
        lens_bounds_color.a( 0.5 );
    }
    #endif

    #if defined( AXN_DEBUG )
    if( Settings::get( Settings::DEBUG_SHOW_BACKGROUND ) )
    {
        #endif
        ogl::clear_color( world( )->current_room( )->fill_color( ) );
        #if defined( AXN_DEBUG )
    }
    else
    {
        if( Settings::get( Settings::DEBUG_BACKGROUND_COLOR ) )
        {
            ogl::clear_color( WHITE );
        }
        else
        {
            ogl::clear_clear_color( );
        }
    }
    #endif

    ogl_matrix_clear( )
    {
        ogl::scale( inverse( half( c_screen_width ) ), inverse( half( c_screen_height ) ) );

        Engine::anti_alias( ) ? ogl::enable_anti_alias( ) : ogl::disable_anti_alias( );

        lambdaf( render_visible )( Visible cref in_visible, bool in_fixed )
        {
            // ogl::clear_stencil( );
            ogl::stencil_always( );

            // ogl::clear_depth( );
            ogl::depth_always( );

            ogl_matrix( )
            {
                if( !in_fixed )
                {
                    if( !is_zero( in_visible.z( ) ) )
                    {
                        ogl::scale( c_zoom * in_visible.z( ) );
                        ogl::translate( -( c_camera_center.x( ) ),
                                        -( c_camera_center.y( ) ) );
                    }
                }

                ogl::translate( in_visible.center( ).x( ), in_visible.center( ).y( ) );

                #if defined( AXN_DEBUG )
                if( Debug::active && Settings::get( Settings::DEBUG_SHOW_VISUAL_BBOX ) )
                {
                    render_convex_polygon( { WHITE.a( 0.15 ) }, in_visible.bounding_box( ).points( ) );
                }
                #endif

                render_drawing( in_visible, c_zoom );
            }
        };

        lambdaf( render_subjects )( )
        {
            m_background_subjects.sort( Visible::sort, true );
            for_each( visible, m_background_subjects ) { render_visible( *visible, false ); }

            m_subjects.sort( Visible::sort, true );
            for_each( visible, m_subjects ) { render_visible( *visible, false ); }

            m_foreground_subjects.sort( Visible::sort, true );
            for_each( visible, m_foreground_subjects ) { render_visible( *visible, false ); }

            ogl::clear_stencil( );
        };

        lambdaf( render_lighting )( )
        {
            ogl::stencil_always( );

            if( world( )->current_room( )->lighting_active( ) )
            {
                if( ptr<const Lighting> lighting = world( )->current_room( )->lighting( ) )
                {
                    if( lighting->ambient_color( ).is_visible( ) )
                    {
                        ogl::depth_always( );
                        render_convex_polygon( { lighting->ambient_color( ) }, c_screen.points( ) );
                    }

                    varray<LightSource> cref light_sources = lighting->light_sources( );

                    if( light_sources.size( ) )
                    {
                        ogl_matrix( )
                        {
                            ogl::scale( c_zoom );
                            ogl::translate( -c_camera_center.x( ),
                                            -c_camera_center.y( ) );

                            Polygon cover = Polygon::rectangle( ( c_screen_width / c_zoom ), ( c_screen_height / c_zoom ), c_camera_center );

                            for_range( i, LIGHTING_LAYERS + 1 )
                            {
                                ogl::clear_depth( );
                                ogl::depth_always( );

                                for_each( light, light_sources )
                                {
                                    render_convex_polygon( { TRANSPARENT },
                                                           Polygon::circle( light.radius( ) * ( ( i * LIGHTING_RADIUS_GROW * pow( LIGHTING_RADIUS_GROW_EXPONENT, i ) ) + 1 ), light.position( ) ).points( ) );
                                }

                                ogl::depth_not_equal( );

                                render_convex_polygon( { BLACK.a( min( 1.0, ( dec( i + 1 ) / dec( LIGHTING_LAYERS ) ) ) * lighting->darkness_intensity( ) ) }, cover.points( ) );
                            }
                        }
                    }
                }
            }
        };

        lambdaf( render_transition )( )
        {
            ogl::clear_stencil( );
            ogl::stencil_always( );

            if( transitioning( ) )
            {
                dec percent = m_transitioning_out ? one_minus( m_transition_timer.percentage( ) ) : m_transition_timer.percentage( );

                switch( m_transition_type )
                {
                    d_case( NO_TRANSITION ) : { }

                    b_case( FADE ) :
                    {
                        render_convex_polygon( { lens_bounds_color.a( percent ) }, bounds( false ).points( ) );
                    }

                    b_case( SWIPE ) :
                    {
                        Point start, end;
                        Angle a = m_transition_vec.angle( ).truncated( );

                        if( a.in_quadrant( Q1, true ) )
                        {
                            start = bounds( false ).bottom_left( );
                            end = bounds( false ).top_right( );
                        }
                        else if( a.in_quadrant( Q2, true ) )
                        {
                            start = bounds( false ).bottom_right( );
                            end = bounds( false ).top_left( );
                        }
                        else if( a.in_quadrant( Q3, true ) )
                        {
                            start = bounds( false ).top_right( );
                            end = bounds( false ).bottom_left( );
                        }
                        else // if( a.in_quadrant( Q4, true ) )
                        {
                            start = bounds( false ).top_left( );
                            end = bounds( false ).bottom_right( );
                        }

                        Vector distance_total = end - start;
                        Vector distance = distance_total * percent;

                        Planc d = x2( max( width( ), height( ) ) );

                        Vector vec = start + distance;

                        Vector side1 = vec + VectorA( a + RIGHT, d );
                        Vector back1 = side1 - VectorA( a, d );

                        Vector side2 = vec + VectorA( a - RIGHT, d );
                        Vector back2 = side2 - VectorA( a, d );

                        // todo ideally this would just be a crop

                        ogl::stencil_mask( true );
                        ogl::stencil_not_equal( );
                        ogl::stencil_add( );

                        render_convex_polygon( { TRANSPARENT }, bounds( false ).points( ) );

                        ogl::stencil_equal( );

                        render_convex_polygon( { lens_bounds_color }, { side1, back1, back2, side2 } );

                    }

                    b_case( CIRCLE ) :
                    {
                        Max<Planc> max_distance;

                        Vector vec = m_transition_vec - center( );

                        max_distance.update( ( vec - bounds( false ).top_left( ) ).magnitude( ) );
                        max_distance.update( ( vec - bounds( false ).top_right( ) ).magnitude( ) );
                        max_distance.update( ( vec - bounds( false ).bottom_right( ) ).magnitude( ) );
                        max_distance.update( ( vec - bounds( false ).bottom_left( ) ).magnitude( ) );

                        Planc radius = max_distance.value( ) * one_minus( percent );

                        ogl::stencil_mask( true );
                        ogl::stencil_not_equal( );
                        ogl::stencil_add( );

                        render_convex_polygon( { TRANSPARENT }, Polygon::circle( radius, vec ).points( ) );
                        render_convex_polygon( { lens_bounds_color }, bounds( false ).points( ) );
                    }
                }
            }
            else if( transitioned( ) )
            {
                render_convex_polygon( { lens_bounds_color }, bounds( false ).points( ) );
            }
        };

        lambdaf( render_lens_bounds )( )
        {
            ogl::stencil_mask( true );
            ogl::stencil_not_equal( );
            ogl::stencil_add( );

            ogl::clear_stencil( );

            render_convex_polygon( { TRANSPARENT }, ( ( world( )->current_room( )->bounds( ) * c_zoom ) - ( center( ) * c_zoom ) ).points( ) );
            render_convex_polygon( { world_bounds_color }, c_screen.points( ) );

            ogl::clear_stencil( );

            render_convex_polygon( { TRANSPARENT }, bounds( false ).points( ) );
            render_convex_polygon( { lens_bounds_color }, c_screen.points( ) );

            ogl::clear_stencil( );
        };

        lambdaf( render_screen_effects )( )
        {
            for_each( screen_effect, m_screen_effects )
            {
                screen_effect->render( this );
                render_visible( *screen_effect, true );
            }
        };

        lambdaf( render_hud_elements )( )
        {
            if( show_hud( ) )
            {
                for_each( hud_element, m_hud_elements )
                {
                    hud_element->render( this );
                    render_visible( *hud_element, true );
                }
            }

            for_each( always_hud_element, m_always_hud_elements )
            {
                always_hud_element->render( this );
                render_visible( *always_hud_element, true );
            }
        };

        lambdaf( render_cursor )( )
        {
            if( is_num( cursor_world_position( ).x( ) ) && is_num( cursor_world_position( ).y( ) ) )
            {
                render_visible( Visible( cursor_drawing( ).scale( inverse( c_zoom ) ).move( Vector( cursor_world_position( ) ) ) ), false );
            }
        };

        lambdaf( render_debug_elements )( )
        {
            #if defined( AXN_DEBUG )
            if( Debug::active )
            {
                for_each( visible, m_debug_subjects )
                {
                    render_visible( *visible, false );
                }

                if( Settings::get( Settings::DEBUG_CAMERA_DISPLAY ) )
                {
                    render_visible( Visible( debug_overlay_drawing( ) ), true );
                }
            }
            #endif
        };

        render_subjects( );
        render_lighting( );
        render_screen_effects( );
        render_debug_elements( );
        render_transition( );
        render_lens_bounds( );
        render_hud_elements( );
        render_cursor( );
    }

    #if defined( TRACK_POLYGONS )

    if( polygon_count )
    {
        total_render_count++;
        total_polygon_count += polygon_count;
        total_point_count += point_count;
        max_polygon_count = max( polygon_count, max_polygon_count );
    }

    // cout
    //    << "polygons: " << ( polygon_count )
    //    << " ( points: " << ( point_count )
    //    << " ) ( convex: " << ( convex_polygon_count )
    //    << " -> " << ( convex_polygon_triangle_count )
    //    << " ) / average: " << ( total_polygon_count / total_render_count )
    //    << " / max: " << ( max_polygon_count )
    //    << endl;

    // Log( DEBUG_LOG, "polygons: %u ( points: %u ) ( convex: %u -> %u ) / average: %u / max: %u",
    //     polygon_count,
    //     point_count,
    //     convex_polygon_count,
    //     convex_polygon_triangle_count,
    //     total_polygon_count / total_render_count,
    //     max_polygon_count );

    #endif
}

Drawing Camera::cursor_drawing( ) const
{
    static_setup( Drawing cursor_drawing )
    {
        cursor_drawing.draw( BLACK, Polygon::rectangle( RETICLE_LENGTH + x2( RETICLE_BORDER_WIDTH ),
                                                        RETICLE_WIDTH + x2( RETICLE_BORDER_WIDTH ) ) );
        cursor_drawing.draw( BLACK, Polygon::rectangle( RETICLE_WIDTH + x2( RETICLE_BORDER_WIDTH ),
                                                        RETICLE_LENGTH + x2( RETICLE_BORDER_WIDTH ) ) );
        cursor_drawing.draw( WHITE, Polygon::rectangle( RETICLE_WIDTH, RETICLE_LENGTH ) );
        cursor_drawing.draw( WHITE, Polygon::rectangle( RETICLE_LENGTH, RETICLE_WIDTH ) );
    }

    return( cursor_drawing );
}

#if defined( AXN_DEBUG )
Drawing Camera::debug_overlay_drawing( ) const
{
    static Angle delta;

    static Angle last_delta;
    static Angle last_target_delta;

    delta -= DELTA;

    const Planc c_thickness = FPS_LINE_THICKNESS * zoom( );

    const Planc c_width = width( );
    const Planc c_height = height( );

    const Planc c_half_width = half( c_width );
    const Planc c_half_height = half( c_height );

    const Vector c_target_offset = target( ) - center( );

    const Polygon c_target_outer = Polygon::circle( TARGET_RADIUS + c_thickness, c_target_offset );
    const Polygon c_target_inner = Polygon::circle( TARGET_RADIUS, c_target_offset );
    const Polygon c_target_cover = Polygon::circle( TARGET_RADIUS );

    const Polygon c_fps_circle = Polygon::circle( FPS_RADIUS );
    const Polygon c_fps_dot = Polygon::circle( TARGET_RADIUS );

    const Angle c_target_delta( ( ( dec( milliseconds( ) ) / 1000.0 ) * -TAU ) * DELTA );

    const Line c_fps_target_line( ORIGIN, PointY( FPS_RADIUS ).rotate( c_target_delta ) );
    const Line c_fps_line( ORIGIN, PointY( FPS_RADIUS ).rotate( delta ) );

    const Angle c_d_delta( delta - last_delta );
    const Angle c_d_target_delta( c_target_delta - last_target_delta );
    const Angle c_d_d_delta( ( c_d_delta - c_d_target_delta ) * TAU );

    const Polygon c_lag( Arc::ccw( FPS_RADIUS, delta + RIGHT, delta + c_d_d_delta + RIGHT ).points( ) + ORIGIN );

    last_delta = delta;
    last_target_delta = c_target_delta;

    Drawing overlay_drawing;

    overlay_drawing.draw( TARGET_COLOR.a( COLOR_OPACITY ), c_fps_target_line, c_thickness, true );

    // draw lag
    // if( is_pos( c_d_d_delta ) ) { overlay_drawing.draw( TARGET_COLOR.a( LAG_COLOR_OPACITY ), c_lag, FILLED ); }

    overlay_drawing.draw( MAIN_COLOR,
                          Line( Point( -c_half_width, -c_half_height ),
                                Point( +c_half_width, -c_half_height ) ),
                          BORDER_LINE_THICKNESS, true, true );
    overlay_drawing.draw( MAIN_COLOR,
                          Line( Point( -c_half_width, +c_half_height ),
                                Point( +c_half_width, +c_half_height ) ),
                          BORDER_LINE_THICKNESS, true, true );
    overlay_drawing.draw( MAIN_COLOR,
                          Line( Point( -c_half_width, -c_half_height ),
                                Point( -c_half_width, +c_half_height ) ),
                          BORDER_LINE_THICKNESS, true, true );
    overlay_drawing.draw( MAIN_COLOR,
                          Line( Point( +c_half_width, -c_half_height ),
                                Point( +c_half_width, +c_half_height ) ),
                          BORDER_LINE_THICKNESS, true, true );

    overlay_drawing.draw( MAIN_COLOR.a( COLOR_OPACITY ),
                          Line( PointX( -c_half_width ),
                                PointX( +c_half_width ) ),
                          CROSSHAIR_LINE_THICKNESS, true );
    overlay_drawing.draw( MAIN_COLOR.a( COLOR_OPACITY ),
                          Line( PointY( -c_half_height ),
                                PointY( +c_half_height ) ),
                          CROSSHAIR_LINE_THICKNESS, true );

    overlay_drawing.draw( MAIN_COLOR, c_target_outer, FILLED );
    overlay_drawing.draw( TARGET_COLOR, c_target_inner, FILLED );
    overlay_drawing.draw( MAIN_COLOR, c_target_cover, FILLED );

    overlay_drawing.draw( MAIN_COLOR.a( COLOR_OPACITY ), c_fps_line, c_thickness, true );
    overlay_drawing.draw( MAIN_COLOR.a( COLOR_OPACITY ), c_fps_circle, c_thickness, true );
    overlay_drawing.draw( MAIN_COLOR, c_fps_dot, FILLED );
    // TODO c_thickness shouldn't be necessary, preserve thickness isn't working

    return( overlay_drawing );
}
#endif

void Camera::update( )
{
    m_age++;

    Point old_center = center( );
    Vector movement = Vector( old_center, target( ) ) * m_movement_ratio;
    Point new_center = old_center + movement;

    center( new_center );
    cursor_world_position( cursor_world_position( ) + ( center( ) - old_center ) );

    if( m_transition_timer.tick( ) && m_transition_complete_timer.tick( ) )
    {
        m_transitioned = m_transitioning_out;
    }
}

ptr<Visible> Camera::capture( ptr<Visible> in_subject, bool in_should_delete )
{
    Assert( !is_null( in_subject ) );

    m_subjects.insert_back( in_subject );

    if( in_should_delete ) { m_owned_subjects.insert_back( in_subject ); }

    return( in_subject );
}

ptr<Visible> Camera::capture_background( ptr<Visible> in_subject, bool in_should_delete )
{
    Assert( !is_null( in_subject ) );

    m_background_subjects.insert_back( in_subject );

    if( in_should_delete ) { m_background_owned_subjects.insert_back( in_subject ); }

    return( in_subject );
}

ptr<Visible> Camera::capture_foreground( ptr<Visible> in_subject, bool in_should_delete )
{
    Assert( !is_null( in_subject ) );

    m_foreground_subjects.insert_back( in_subject );

    if( in_should_delete ) { m_foreground_owned_subjects.insert_back( in_subject ); }

    return( in_subject );
}

ptr<Camera::ScreenVisual> Camera::add_screen_effect( ptr<ScreenVisual> in_effect, bool in_should_delete )
{
    Assert( !is_null( in_effect ) );

    m_screen_effects.insert_back( in_effect );

    if( in_should_delete ) { m_owned_screen_effects.insert_back( in_effect ); }

    return( in_effect );
}

void Camera::remove_screen_effect( ptr<ScreenVisual> in_effect )
{
    Assert( !is_null( in_effect ) );

    m_screen_effects.remove( in_effect );

    if( m_owned_screen_effects.contains( in_effect ) ) { m_owned_screen_effects.remove( in_effect ); }
}

void Camera::add_hud_element( ptr<HUD> in_hud_element, bool in_should_delete )
{
    Assert( !is_null( in_hud_element ) );

    m_hud_elements.insert_back( in_hud_element );

    if( in_should_delete ) { m_owned_hud_elements.insert_back( in_hud_element ); }
}

void Camera::remove_hud_element( ptr<HUD> in_hud_element )
{
    Assert( !is_null( in_hud_element ) );

    m_hud_elements.remove( in_hud_element );

    if( m_owned_hud_elements.contains( in_hud_element ) ) { m_owned_hud_elements.remove( in_hud_element ); }
}

void Camera::add_always_hud_element( ptr<HUD> in_always_hud_element, bool in_should_delete )
{
    Assert( !is_null( in_always_hud_element ) );

    m_always_hud_elements.insert_back( in_always_hud_element );

    if( in_should_delete ) { m_owned_always_hud_elements.insert_back( in_always_hud_element ); }
}

void Camera::remove_always_hud_element( ptr<HUD> in_always_hud_element )
{
    Assert( !is_null( in_always_hud_element ) );

    m_always_hud_elements.remove( in_always_hud_element );

    if( m_owned_always_hud_elements.contains( in_always_hud_element ) ) { m_owned_always_hud_elements.remove( in_always_hud_element ); }
}

#if defined( AXN_DEBUG )
void Camera::capture_debug( ptr<Visible> in_subject, bool in_should_delete )
{
    Assert( !is_null( in_subject ) );

    m_debug_subjects.insert_back( in_subject );

    if( in_should_delete ) { m_owned_debug_subjects.insert_back( in_subject ); }
}
#endif

Planc Camera::width( ) const { return( m_width ); }
void Camera::width( Planc cref in_width ) { m_width = in_width; }

Planc Camera::height( ) const { return( m_height ); }
void Camera::height( Planc cref in_height ) { m_height = in_height; }

Point Camera::center( ) const { return( m_center ); }
void Camera::center( Point cref in_center )
{
    m_center = in_center;

    if( world( ) && world( )->current_room( ) )
    {
        FixedRectangle bb = world( )->current_room( )->bounds( );

        bb.shrink_width( width( ) / zoom( ) );
        bb.shrink_height( height( ) / zoom( ) );

        if( bb.width( ) )
        {
            m_center.x( min( m_center.x( ), bb.upper_bound_x( ) ) );
            m_center.x( max( m_center.x( ), bb.lower_bound_x( ) ) );
        }
        else
        {
            m_center.x( bb.center( ).x( ) );
        }

        if( bb.height( ) )
        {
            m_center.y( min( m_center.y( ), bb.upper_bound_y( ) ) );
            m_center.y( max( m_center.y( ), bb.lower_bound_y( ) ) );
        }
        else
        {
            m_center.y( bb.center( ).y( ) );
        }

        if( bb.area( ) )
        {
            if( !bb.contains( m_center ) )
            {
                m_center.x( min( m_center.x( ), bb.upper_bound_x( ) ) );
                m_center.x( max( m_center.x( ), bb.lower_bound_x( ) ) );
                m_center.y( min( m_center.y( ), bb.upper_bound_y( ) ) );
                m_center.y( max( m_center.y( ), bb.lower_bound_y( ) ) );
            }
        }
        else
        {
            if( !bb.width( ) )
            {
                m_center.x( bb.center( ).x( ) );
            }

            if( !bb.height( ) )
            {
                m_center.y( bb.center( ).y( ) );
            }
        }
    }
}

dec Camera::zoom( ) const { return( m_zoom ); }
void Camera::zoom( dec in_zoom )
{
    Assert( is_pos( in_zoom ) );

    Point pre_target_offset = target_offset( );
    Point cursor_screen_position = world_to_screen( cursor_world_position( ) );

    m_zoom = in_zoom;

    #if defined( AXN_DEBUG )
    if( Settings::get( Settings::DEBUG_CAMERA_ZOOM_LOCK ) )
        #endif
        m_zoom = minmax( m_zoom, min_zoom( ), max_zoom( ) );

    Point post_target_offset = target_offset( );
    Point d_target_offset = post_target_offset - pre_target_offset;

    center( center( ) + d_target_offset );
    target( target( ) + d_target_offset );

    cursor_world_position( screen_to_world( cursor_screen_position ) );
}

dec Camera::min_zoom( ) const { return( MIN_ZOOM ); }
dec Camera::max_zoom( ) const { return( MAX_ZOOM ); }

bool Camera::show_hud( ) const { return( m_show_hud ); }
void Camera::show_hud( bool in_show_hud ) { m_show_hud = in_show_hud; }

dec Camera::hud_offset_percentage( ) const { return( m_hud_offset_percentage ); }
void Camera::hud_offset_percentage( dec in_hud_offset_percentage ) { m_hud_offset_percentage = in_hud_offset_percentage; }

Point Camera::target( bool in_include_offset ) const { return( m_target - ( in_include_offset ? target_offset( ) : V0 ) ); }
void Camera::target( Point cref in_target, bool in_hard_set )
{
    m_target = in_target;
    m_target += target_offset( );

    if( in_hard_set ) { center( m_target ); }
}

Vector Camera::target_offset( ) const { return( VectorY( ( height( ) * target_offset_ratio_y( ) ) / zoom( ) ) ); }

dec Camera::target_offset_ratio_y( ) const { return( m_offset_ratio_y ); }
void Camera::target_offset_ratio_y( dec in_offset_y ) { m_offset_ratio_y = in_offset_y; }

FixedRectangle Camera::bounds( bool in_positioned ) const { return( FixedRectangle( width( ), height( ), ( in_positioned ? center( ) : ORIGIN ) ) ); }

bool Camera::in_view( Point cref in_world_position, dec in_z ) const
{
    return_true_if( !in_z );

    Point screen_position = world_to_screen( in_world_position );

    return( in_range( screen_position.x( ), ( width( ) / in_z ) ) && in_range( screen_position.y( ), ( height( ) / in_z ) ) );
}

bool Camera::in_view( FixedRectangle cref in_world_bounding_box, dec in_z ) const
{
    return_true_if( !in_z );

    FixedRectangle bounds( world_to_screen( in_world_bounding_box.lower( ) ), world_to_screen( in_world_bounding_box.upper( ) ) );
    FixedRectangle screen( ( width( ) / in_z ), ( height( ) / in_z ), world_to_screen( center( ) ) );

    return( screen.intersects( bounds ) );
}

Point Camera::screen_to_world( Point cref in_screen_position ) const
{
    Point world_position = in_screen_position;

    world_position += center( ) - half( Vector( Engine::screen_width( ), Engine::screen_height( ) ) );
    world_position.y( -world_position.y( ) + x2( center( ).y( ) ) );
    world_position = Vector( center( ), world_position ) / zoom( );

    return( world_position );
}

Point Camera::world_to_screen( Point cref in_world_position ) const
{
    Point screen_position = in_world_position;

    screen_position = Vector( center( ), screen_position ) * zoom( );
    screen_position.y( -screen_position.y( ) + x2( center( ).y( ) ) );
    screen_position -= center( ) - half( Vector( Engine::screen_width( ), Engine::screen_height( ) ) );

    return( screen_position );
}

Point Camera::cursor_world_position( ) { return( m_cursor_world_position ); }
void Camera::cursor_world_position( Point cref in_cursor_world_position ) { m_cursor_world_position = in_cursor_world_position; }

void Camera::cursor_world_position_reset( ) { m_cursor_world_position = INVALID_POINT; }

Camera::HUD::HUD( dec in_center_x_percent, dec in_center_y_percent, dec in_width_percent, dec in_height_percent )
{
    width( in_width_percent );
    height( in_height_percent );
    center( in_center_x_percent, in_center_y_percent );
}

void Camera::HUD::width( dec in_width_percent ) { m_width_percent = in_width_percent; }
void Camera::HUD::height( dec in_height_percent ) { m_height_percent = in_height_percent; }
void Camera::HUD::center( dec in_center_x_percent, dec in_center_y_percent ) { m_center_x_percent = in_center_x_percent; m_center_y_percent = in_center_y_percent; }

FixedRectangle Camera::HUD::bounds( ptr<Camera> camera ) const
{
    Planc width = camera->width( );
    Planc height = camera->height( );

    Planc offset = min( width, height ) * camera->hud_offset_percentage( );

    width -= x2( offset );
    height -= x2( offset );

    Planc display_width = width * m_width_percent;
    Planc display_height = height * m_height_percent;

    Point center = Point( width * ( m_center_x_percent - 0.5 ), height * ( m_center_y_percent - 0.5 ) );

    return( FixedRectangle( display_width, display_height, center ) );
}

bool Camera::transitioned( ) const { return( m_transitioned ); }
bool Camera::transitioning( ) const { return( m_transition_timer.remaining( ) || m_transition_complete_timer.remaining( ) ); }

void Camera::transition( transition_type in_transition, Vector cref in_vec )
{
    Assert( !transitioning( ) );

    m_transition_type = in_transition;
    m_transition_vec = in_vec;

    m_transition_timer.reset( );
    m_transition_complete_timer.reset( );

    m_transitioning_out = !transitioned( );
}

void Camera::transition_duration( ticks in_duration, ticks in_completion_duration )
{
    Assert( !transitioning( ) );

    m_transition_timer.duration( in_duration );
    m_transition_complete_timer.duration( in_completion_duration );
}
