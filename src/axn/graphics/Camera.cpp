#include "axn/graphics/Camera.hpp"

#include "axn/system/OGL.hpp"
#include "axn/system/Engine.hpp"
#include "axn/world/Object.hpp"

var_const( string CLICK_SOUND = ( "click.wav" ) );

var_const( dec MIN_ZOOM = ( 0.64 ) );
var_const( dec MAX_ZOOM = ( 2.5 ) );

var_const( dec DEFAULT_TARGET_OFFSET_Y = ( 0.125 ) );

var_const( uint LIGHTING_LAYERS = ( 6 ) );
var_const( dec LIGHTING_RADIUS_GROW = ( 0.333 ) );
var_const( dec LIGHTING_RADIUS_GROW_EXPONENT = ( 0.88 ) );

var_const( dec DEFAULT_HUD_OFFSET = ( 0.025 ) );

var_const( Planc DEFAULT_RENDER_BUFFER = ( METER * 4.0 ) ); // C! see Camera::render_buffer( )

var_const( dec CINEMATIC_HEIGHT_RATIO = ( 0.75 ) ); // C! how much of the frame's height cinematic mode shows

section( TRANSITION )
{
    var_const( ticks DEFAULT_TRANSITION_DURATION = ( 20.0 ) );
    var_const( ticks DEFAULT_TRANSITION_COMPLETE_DURATION = ( 4.0 ) );
}

section( TRACKING )
{
    #if defined( AXN_DEBUG )
    // #define TRACK_POLYS
    #endif

    #if defined( TRACK_POLYS )
    var_static( uint total_render_count = ( 0 ) );
    var_static( uint total_poly_count = ( 0 ) );
    var_static( uint total_pt_count = ( 0 ) );
    var_static( uint poly_count = ( 0 ) );
    var_static( uint pt_count = ( 0 ) );
    var_static( uint max_poly_count = ( 0 ) );
    #endif
}

section( SHUTTER )
{
    var_const( uint SHUTTER_SIDES = ( 9 ) ); // C!
    var_const( ticks SHUTTER_DURATION = ( 20.0 ) ); // C!
    var_const( Color SHUTTER_COLOR = ( BLACK ) ); // C!
    var_const( Color SHUTTER_EDGE_COLOR = ( GRAY_DARK ) ); // C!
    var_const( dec SHUTTER_EDGE_THICKNESS = ( 1.5 ) ); // C!
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

        virtual void render( ptr<Camera> camera ) override
        {
            HUD::render( camera );

            if( Engine::paused( ) && !camera->shutter( ).paused_engine( ) ) // C! no pause icon for the shutter's own pause, but a pause already on keeps its icon
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

                    draw( WHITE.a( is_pos( m_play_show_countdown.percentage( ) ) ? 1.0 : dec( m_play_fade_countdown.percentage( ) ) ), tri );
                }
            }
        }

    private:

        Countdown m_play_show_countdown;
        Countdown m_play_fade_countdown;

    };

    var_static( PauseHUD pause_hud );
}

#if defined( AXN_DEBUG )
section( VOLUME_HUD )
{
    var_const( dec VOLUME_HUD_WIDTH = ( 0.1 ) ); // C!
    var_const( dec VOLUME_HUD_HEIGHT = ( 0.1 ) ); // C!
    var_const( dec VOLUME_HUD_CENTER_Y = ( 0.5 ) ); // C! both speakers halfway up the screen

    var_const( Planc VOLUME_LINE_THICKNESS = ( 5.0 ) );

    var_const( dec VOLUME_MUTE_X_SIZE = ( 0.5 ) ); // C! how wide the muted x is, as a part of the arcs' space (it's kept inside the HUD's height)
    var_const( dec VOLUME_MUTE_X_DISTANCE = ( 0.5 ) ); // C! how far in front of the cone the muted x's middle is, as a part of the arcs' space

    var_const( uint VOLUME_ARCS = ( 3 ) ); // C! the arcs shown at full volume; below full, 0 to 1 is split evenly into one fewer steps, one arc per step
    var_const( Angle VOLUME_ARC_SPREAD = ( Angle::fraction( 7.5 ) ) ); // C! how far each arc reaches either side of straight ahead

    var_const( dec VOLUME_SPEAKER_BOX_DEPTH = ( 0.125 ) ); // C! the box at the back
    var_const( dec VOLUME_SPEAKER_BOX_HEIGHT = ( 0.35 ) ); // C!
    var_const( dec VOLUME_SPEAKER_BOX_HALF_HEIGHT = ( half( VOLUME_SPEAKER_BOX_HEIGHT ) ) ); // C!

    var_const( dec VOLUME_SPEAKER_CONE_DEPTH = ( 0.33 ) ); // C! where the cone in front of the box opens out to
    var_const( dec VOLUME_SPEAKER_CONE_HEIGHT = ( 0.8 ) ); // C!
    var_const( dec VOLUME_SPEAKER_CONE_HALF_HEIGHT = ( half( VOLUME_SPEAKER_CONE_HEIGHT ) ) ); // C!

    var_const( Color VOLUME_HUD_COLOR = ( WHITE ) ); // C!
    var_const( Color VOLUME_ERROR_COLOR = ( RED ) ); // C! the arcs or x while Engine::sound_error( )

    // C! a speaker at the side of the screen, facing the middle, with arcs in front of it for the loudest sound playing (Engine::loudest_sound( )):
    // C! VOLUME_ARCS at full volume, fewer the quieter it is, none when nothing is playing. Shown only while debug mode is on and the HUD is shown (it's a normal HUD element)
    class VolumeHUD : public Camera::HUD
    {

    public:

        // C! in_left puts it on the left, facing right; otherwise it's on the right, facing left (the same speaker, mirrored)
        VolumeHUD( bool in_facing_right = true ) : HUD( in_facing_right ? half( VOLUME_HUD_WIDTH ) : one_minus( half( VOLUME_HUD_WIDTH ) ), VOLUME_HUD_CENTER_Y, VOLUME_HUD_WIDTH, VOLUME_HUD_HEIGHT ), m_facing_right( in_facing_right ) { }

        virtual void render( ptr<Camera> camera ) override
        {
            HUD::render( camera );

            return_if( !Debug::active || !Settings::get( Settings::DEBUG_AUDIO ) );

            const FixedRectangle c_bounds = bounds( camera );
            const Planc c_width = c_bounds.width( );
            const Planc c_height = c_bounds.height( );
            const Point c_back = Point( c_bounds.center( ).x( ) - ( negated( half( c_width ), !m_facing_right ) ), c_bounds.center( ).y( ) ); // C! the middle of its back edge

            // C! a point on the speaker, in_ahead of its back (towards the middle of the screen) and in_up from its middle
            lambdaf( at )( Planc in_x, Planc in_y ) { return( Point( c_back.x( ) + negated( c_width * in_x, !m_facing_right ), c_back.y( ) + ( c_height * in_y ) ) ); };

            // C! the speaker: a box at the back, and a cone opening forward from it
            draw( VOLUME_HUD_COLOR, Polygon( { at( 0.0, -VOLUME_SPEAKER_BOX_HALF_HEIGHT ), at( VOLUME_SPEAKER_BOX_DEPTH, -VOLUME_SPEAKER_BOX_HALF_HEIGHT ),
                                               at( VOLUME_SPEAKER_BOX_DEPTH, VOLUME_SPEAKER_BOX_HALF_HEIGHT ), at( 0.0, VOLUME_SPEAKER_BOX_HALF_HEIGHT ) } ) );
            draw( VOLUME_HUD_COLOR, Polygon( { at( VOLUME_SPEAKER_BOX_DEPTH, -VOLUME_SPEAKER_BOX_HALF_HEIGHT ), at( VOLUME_SPEAKER_CONE_DEPTH, -VOLUME_SPEAKER_CONE_HALF_HEIGHT ),
                                               at( VOLUME_SPEAKER_CONE_DEPTH, VOLUME_SPEAKER_CONE_HALF_HEIGHT ), at( VOLUME_SPEAKER_BOX_DEPTH, VOLUME_SPEAKER_BOX_HALF_HEIGHT ) } ) );

            const Point c_mouth = at( VOLUME_SPEAKER_CONE_DEPTH, 0.0 );

            // C! the arcs spread evenly out to the furthest one that fits: in front of the cone, and (at its widest, VOLUME_ARC_SPREAD either
            // C! side) inside half the HUD's height, less half a line so the line itself fits too
            const Planc c_room_ahead = c_width * one_minus( VOLUME_SPEAKER_CONE_DEPTH );
            const Planc c_room_across = half( c_height ) / max( dec( VectorA( VOLUME_ARC_SPREAD, 1.0 ).dy( ) ), dec( DEFAULT_EPSILON ) );
            const Planc c_furthest = min( c_room_ahead, c_room_across ) - half( VOLUME_LINE_THICKNESS );
            const Angle c_ahead = ( m_facing_right ? A0 : PI ); // C! straight ahead of it

            const Color c_color = ( Engine::sound_error( ) ? VOLUME_ERROR_COLOR : VOLUME_HUD_COLOR );

            if( Engine::muted( ) || Engine::sfx_muted( ) ) // C! an x where the arcs go: the sound effects can't be heard
            {
                const Planc c_half = min( half( c_furthest * VOLUME_MUTE_X_SIZE ), half( c_height ) - half( VOLUME_LINE_THICKNESS ) );
                const Point c_center = c_mouth + VectorA( c_ahead, c_furthest * VOLUME_MUTE_X_DISTANCE );

                draw( c_color, Line( c_center + Vector( -c_half, -c_half ), c_center + Vector( c_half, c_half ) ), VOLUME_LINE_THICKNESS );
                draw( c_color, Line( c_center + Vector( -c_half, c_half ), c_center + Vector( c_half, -c_half ) ), VOLUME_LINE_THICKNESS );
            }
            else
            {
                const declmp c_loudest = Engine::loudest_sound( );
                return_if( is_zero( c_loudest ) ); // C! nothing playing

                // C! all of them at full volume; below that, which of the VOLUME_ARCS - 1 even steps from 0 to 1 it's in (with 3: 1 arc below half, 2 from half)
                uint arcs = VOLUME_ARCS;
                if( !is_one( c_loudest ) && ( VOLUME_ARCS > 1 ) )
                {
                    arcs = min( VOLUME_ARCS - 1, uint( dec( c_loudest ) * dec( VOLUME_ARCS - 1 ) ) + 1 ); // C! min: just under full still counts as the last step below it
                }

                for_range( i, arcs )
                {
                    const Planc c_radius = c_furthest * ( dec( i + 1 ) / dec( VOLUME_ARCS ) );
                    draw( c_color, Arc::ccw( c_radius, c_mouth, c_ahead - VOLUME_ARC_SPREAD, c_ahead + VOLUME_ARC_SPREAD ).path( ), VOLUME_LINE_THICKNESS );
                }
            }
        }

    private:

        bool m_facing_right = true;

    };

    var_static( VolumeHUD volume_hud_left = ( VolumeHUD( true ) ) );
    var_static( VolumeHUD volume_hud_right = ( VolumeHUD( false ) ) );
}
#endif

// C! fading a drawing as one (Drawing::opacity( )) takes two passes over it, each polygon given its own depth by the order it's drawn in,
// C! later ones nearer: the depth pass writes the opaque polygons' depths, then the color pass draws each polygon only where nothing
// C! opaque was drawn above it, its colors' alphas times the opacity. What an opaque polygon covers stays hidden, what a see-through one
// C! covers still shows
var_static( bool render_grayscale = ( false ) ); // C! the grayscale setting, read once at the start of Camera::render( ) for every polygon it draws

section( FADE )
{
    enum fade_pass_type
    {
        NO_FADE_PASS = 0,
        FADE_DEPTH_PASS,
        FADE_COLOR_PASS,
    };

    var_const( dec FADE_Z_RANGE = ( 0.9 ) ); // C! the depths used go from -FADE_Z_RANGE to +FADE_Z_RANGE, inside OpenGL's -1 to 1

    var_static( fade_pass_type fade_pass = ( NO_FADE_PASS ) );
    var_static( uint fade_poly_count = ( 0 ) ); // C! the colored polygons in the drawing being faded
    var_static( uint fade_poly_index = ( 0 ) ); // C! the next one's place in the drawing order
    var_static( declmp fade_opacity = ( 1.0 ) );
    var_static( dec vertex_z = ( 0.0 ) ); // C! every vertex's z, 0 except while fading
}

void render_convex_polygon( varray<Color> cref in_colors, varray<Point> cref in_pts )
{
    return_if( is_empty( in_colors ) || ( !is_one( in_colors.size( ) ) && ( in_colors.size( ) != in_pts.size( ) ) ) );

    #if defined( TRACK_POLYS )
    poly_count += 1;
    pt_count += in_pts.size( );
    #endif

    ogl::begin_polygons( );
    {
        bool single_color = is_one( in_colors.size( ) );

        for_range( i, in_pts.size( ) )
        {
            if( !single_color || is_zero( i ) )
            {
                Color color = ( render_grayscale ? Color::grayscale( in_colors[ i ] ) : in_colors[ i ] );
                if( fade_pass == FADE_COLOR_PASS ) { color.a( color.a( ) * fade_opacity ); }
                ogl::color( color );
            }

            ogl::vertex( in_pts[ i ].x( ), in_pts[ i ].y( ), vertex_z );
        }

    }
    ogl::end( );
}

// todo I want to review this
// C! reused by every call, so the triangles, edges, and corners drawn every frame don't each allocate (rendering is single threaded)
section( RENDER_BUFFERS )
{
    var_static( varray<Color> one_color = ( varray<Color>( 1 ) ) );
    var_static( varray<Color> three_colors = ( varray<Color>( 3 ) ) );
    var_static( varray<Point> three_pts = ( varray<Point>( 3 ) ) );
    var_static( varray<Point> four_pts = ( varray<Point>( 4 ) ) );
}

// C! the points of Polygon::circle( in_radius, in_center ), from a cached unit circle into a reused buffer, rather than building a polygon:
// C! lighting draws one for every light on every layer, every frame
varray<Point> cref circle_points( Planc cref in_radius, Point cref in_center )
{
    static umap<uint, varray<Point>> unit_circles; // C! one per point count, so it stays small however the radii change
    static varray<Point> pts;

    uint side_count = curve_point_count( in_radius );

    if( !unit_circles.contains( side_count ) )
    {
        unit_circles.insert( side_count, Polygon::equilateral( side_count, 1.0, ORIGIN, Angle::fraction( x2( side_count ) ) ).points( ) ); // C! the same polygon Polygon::circle( ) makes
    }

    varray<Point> cref unit = unit_circles.at( side_count );

    pts.resize( unit.size( ) );

    for_range( i, unit.size( ) )
    {
        pts[ i ] = Point( in_center.x( ) + ( unit[ i ].x( ) * in_radius ), in_center.y( ) + ( unit[ i ].y( ) * in_radius ) );
    }

    return( pts );
}

void render_filled_polygon( Polygon cref in_poly, varray<Color> cref in_colors )
{
    varray<Point> cref pts = in_poly.points( true );

    if( in_poly.convex( ) )
    {
        render_convex_polygon( in_colors, pts );
    }
    else
    {
        for_each_const( t, in_poly.triangle_indices( ) )
        {
            for_range( k, 3 ) { three_pts[ k ] = pts[ t[ k ] ]; }

            if( is_one( in_colors.size( ) ) )
            {
                render_convex_polygon( in_colors, three_pts );
            }
            else
            {
                for_range( k, 3 ) { three_colors[ k ] = in_colors[ t[ k ] ]; }

                render_convex_polygon( three_colors, three_pts );
            }
        }
    }
}

void render_colored_polygon( Drawing::ColoredPolygon cref in_colored_poly, dec in_zoom )
{
    // todo? Assert( in_colored_polygon.polygon.sides( ) >= 3 );

    varray<Point> cref pts = in_colored_poly.poly.points( true );
    varray<Color> cref color = in_colored_poly.colors;

    if( in_colored_poly.thickness == FILLED )
    {
        render_filled_polygon( in_colored_poly.poly, color );
    }
    else
    {
        const dec c_thickness = ( in_colored_poly.thickness / ( in_colored_poly.preserve_thickness ? ( in_zoom ) : ( 1.0 ) ) ); // C! lines (2 points) keep their thickness too now
        // C! TODO preserve_thickness now applies to 2 point lines too, so debug lines no longer grow and shrink with the zoom: keep, or go back to lines scaling

        for_range( i, pts.size( ) )
        {
            const Line c_line = Line( pts[ i ? ( i - 1 ) : ( pts.size( ) - 1 ) ], pts[ i ] );
            const Angle c_line_angle( c_line.angle( ) );
            const Vector c_line_vector( c_line.pt1( ), c_line.pt2( ) );

            const Line c_next_line = Line( pts[ i ], pts[ ( i == pts.size( ) - 1 ) ? 0 : ( i + 1 ) ] );
            const Angle c_next_line_angle( c_next_line.angle( ) );
            const Vector c_next_line_vector( c_next_line.pt1( ), c_next_line.pt2( ) );

            // C! the rectangle along the edge, the same corners Polygon::rectangle( length, thickness, middle, angle ) has, without building a polygon
            const Point c_middle = Point( half( c_line_vector ) );
            const Vector c_along = VectorA( c_line_angle, half( c_line_vector.magnitude( ) + ( in_colored_poly.extend_lines ? c_thickness : 0.0 ) ) );
            const Vector c_across = VectorA( c_line_angle + RIGHT, half( c_thickness ) );

            four_pts[ 0 ] = c_middle + c_along + c_across;
            four_pts[ 1 ] = c_middle - c_along + c_across;
            four_pts[ 2 ] = c_middle - c_along - c_across;
            four_pts[ 3 ] = c_middle + c_along - c_across;

            one_color[ 0 ] = color[ i ];

            render_convex_polygon( one_color, four_pts );

            // C! fill the notch the two side rectangles leave at the corner, on the outside of the turn (a 2 point polygon is a line, no corners)
            // C! TODO outline corners used to be left notched, now they're mitred (bevelled when sharp): keep, or switch to round or plain bevel
            if( ( pts.size( ) > 2 ) && ( c_line.pt1( ) != c_line.pt2( ) ) && ( c_next_line.pt1( ) != c_next_line.pt2( ) ) )
            {
                const dec c_turn = ( dec( c_line_vector.dx( ) * c_next_line_vector.dy( ) ) - dec( c_line_vector.dy( ) * c_next_line_vector.dx( ) ) );

                if( !is_zero( c_turn ) )
                {
                    const Angle c_outside = ( is_pos( c_turn ) ? -RIGHT : +RIGHT ); // C! turning left leaves the notch on the right

                    Point pt1 = c_line.pt2( );
                    Point pt2 = pt1 + VectorA( c_line_angle + c_outside, half( c_thickness ) );
                    Point pt3 = pt1 + VectorA( c_next_line_angle + c_outside, half( c_thickness ) );
                    Point pt4 = Line( pt2, pt2 + VectorA( c_line_angle ) ).intersection( Line( pt3, pt3 - VectorA( c_next_line_angle ) ) );

                    if( ( pt4 != INVALID_PT ) && ( pt1.distance_to( pt4 ) <= c_thickness ) )
                    {
                        four_pts[ 0 ] = pt1; four_pts[ 1 ] = pt2; four_pts[ 2 ] = pt4; four_pts[ 3 ] = pt3;
                        render_convex_polygon( one_color, four_pts ); // C! mitred corner
                    }
                    else
                    {
                        three_pts[ 0 ] = pt1; three_pts[ 1 ] = pt2; three_pts[ 2 ] = pt3;
                        render_convex_polygon( one_color, three_pts ); // C! a sharp corner's mitre would stick out too far, cut it off
                    }
                }
            }
        }
    }
}

// C! fills a crop or hole polygon into the stencil (the color doesn't matter there)
void render_stencil_polygon( Polygon cref in_poly )
{
    one_color[ 0 ] = CLEAR;

    render_filled_polygon( in_poly, one_color );
}

// C! renders nodes 0 .. in_count - 1 of in_drawing_nodes, the last one outermost, without copying any of them
void render_nodes( uint in_count,
                   Drawing::Nodes cref in_drawing_nodes,
                   varray<Polygon> cref in_crop_polys,
                   varray<Polygon> cref in_hole_polys,
                   dec in_zoom )
{
    return_if( !in_count );

    Drawing::Node cref drawing_node = in_drawing_nodes[ in_count - 1 ];

    ogl::matrix mat;
    {
        ogl::translate( drawing_node.transform.translation_x( ), drawing_node.transform.translation_y( ) );
        ogl::transform( drawing_node.transform );

        // C! the crops and holes so far, only copied when this node adds some of its own
        varray<Polygon> combined_crop_polys;
        varray<Polygon> combined_hole_polys;

        if( !drawing_node.crop_polys.empty( ) ) { combined_crop_polys = in_crop_polys; combined_crop_polys += drawing_node.crop_polys; }
        if( !drawing_node.hole_polys.empty( ) ) { combined_hole_polys = in_hole_polys; combined_hole_polys += drawing_node.hole_polys; }

        varray<Polygon> cref crop_polys = ( drawing_node.crop_polys.empty( ) ? in_crop_polys : combined_crop_polys );
        varray<Polygon> cref hole_polys = ( drawing_node.hole_polys.empty( ) ? in_hole_polys : combined_hole_polys );

        render_nodes( in_count - 1, in_drawing_nodes, crop_polys, hole_polys, in_zoom );

        // C! set after the nodes before it, which set their own
        if( !is_empty( crop_polys ) || !is_empty( hole_polys ) )
        {
            ogl::clear_stencil( );

            ogl::stencil_mask( true );

            ogl::stencil_always( );

            ogl::stencil_add( );

            for_each_const( poly, crop_polys )
            {
                render_stencil_polygon( poly );
            }

            if( !is_empty( crop_polys ) )
            {
                ogl::stencil_remove( );
            }

            for_each_const( poly, hole_polys )
            {
                render_stencil_polygon( poly );
            }

            if( !is_empty( crop_polys ) )
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

        for_each_const( colored_poly, drawing_node.colored_polys )
        {
            if( fade_pass != NO_FADE_PASS )
            {
                vertex_z = -FADE_Z_RANGE + ( 2.0 * FADE_Z_RANGE * dec( ++fade_poly_index ) / dec( fade_poly_count + 1 ) );
                continue_if( ( fade_pass == FADE_DEPTH_PASS ) && !colored_poly.opaque ); // C! a see-through polygon doesn't hide what's under it
            }

            render_colored_polygon( colored_poly, in_zoom );
        }

        for_each_const( drawing, drawing_node.drawings )
        {
            render_nodes( drawing.size( ), drawing, crop_polys, hole_polys, in_zoom );
        }
    }
}

void render_drawing( Drawing cref in_drawing, dec in_zoom )
{
    render_nodes( in_drawing.nodes( ).size( ), in_drawing.nodes( ), varray<Polygon>( ), varray<Polygon>( ), in_zoom );
}

// C! how many colored polygons rendering in_drawing_nodes draws, nested drawings included
uint colored_polygon_count( Drawing::Nodes cref in_drawing_nodes )
{
    uint count = 0;

    for_range( i, in_drawing_nodes.size( ) )
    {
        count += in_drawing_nodes[ i ].colored_polys.size( );

        for_each_const( drawing, in_drawing_nodes[ i ].drawings ) { count += colored_polygon_count( drawing ); }
    }

    return( count );
}

// C! renders in_drawing at its opacity( ) in the two passes described at FADE
void render_faded_drawing( Drawing cref in_drawing, dec in_zoom )
{
    return_if( !is_pos( in_drawing.opacity( ) ) );

    fade_poly_count = colored_polygon_count( in_drawing.nodes( ) );
    fade_opacity = in_drawing.opacity( );

    ogl::clear_depth( ); // C! to 0, the far end

    ogl::color_mask( false );
    ogl::depth_mask( true );
    ogl::depth_equal_greater( ); // C! keeps the nearest opaque polygon's depth
    fade_pass = FADE_DEPTH_PASS;
    fade_poly_index = 0;
    render_drawing( in_drawing, in_zoom );

    ogl::color_mask( true );
    ogl::depth_mask( false );
    fade_pass = FADE_COLOR_PASS; // C! still depth_equal_greater( ): only at or above that polygon
    fade_poly_index = 0;
    render_drawing( in_drawing, in_zoom );

    fade_pass = NO_FADE_PASS;
    fade_opacity = 1.0;
    vertex_z = 0.0;

    ogl::depth_mask( true );
    ogl::depth_always( );
}

Camera::Camera( ptr<World> in_world, Planc cref in_width, Planc cref in_height, dec in_zoom ) : m_world( in_world ), m_shutter( SHUTTER_SIDES, SHUTTER_DURATION )
{
    Assert( !is_null( in_world ) );

    cursor_world_position_reset( );
    width( in_width );
    height( in_height );
    zoom( in_zoom );

    target_offset_ratio_y( DEFAULT_TARGET_OFFSET_Y );

    hud_offset_percentage( DEFAULT_HUD_OFFSET );

    render_buffer( DEFAULT_RENDER_BUFFER );

    #if defined( AXN_DEBUG )
    add_always_hud_element( &pause_hud );
    add_hud_element( &volume_hud_left );
    add_hud_element( &volume_hud_right );
    #endif

    add_screen_effect( &m_shutter );

    #if defined( AXN_DEBUG )
    show_hud( false );
    #endif

    transition_duration( DEFAULT_TRANSITION_DURATION, DEFAULT_TRANSITION_COMPLETE_DURATION );
}

#if defined( AXN_DEBUG )
Drawing Camera::debug_overlay_drawing( ) const
{
    static Angle delta;

    static Angle last_delta;
    static Angle last_target_delta;

    delta -= DELTA;

    const Planc c_thickness = FPS_LINE_THICKNESS; // C! was times zoom( ), to undo outlines in this fixed overlay being divided by the zoom

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

    return( overlay_drawing );
}
#endif

void Camera::update( )
{
    Point old_center = center( );
    Vector movement = Vector( old_center, target( ) ) * m_movement_ratio;
    Point new_center = old_center + movement;

    center( new_center );
    cursor_world_position( cursor_world_position( ) + ( center( ) - old_center ) );

    if( m_transition_timer.tick( ) && m_transition_complete_timer.tick( ) )
    {
        m_transitioned = m_transitioning_out;
    }

    for_each( screen_effect, m_screen_effects )
    {
        screen_effect->update( this );
    }

    m_age++;
}

void Camera::render( )
{
    #if defined( TRACK_POLYS )
    poly_count = 0;
    pt_count = 0;
    #endif

    const dec c_screen_width = Engine::screen_width( );
    const dec c_screen_height = Engine::screen_height( );

    const FixedRectangle c_screen = Engine::screen_bounds( );

    const Point c_camera_center = center( );

    const dec c_zoom = zoom( );

    // C! every setting the render uses, read once here, so the whole frame is drawn with the same settings
    render_grayscale = Settings::get( Settings::GRAPHICS_GRAYSCALE );

    #if defined( AXN_DEBUG )
    const bool c_debug_camera_display = Settings::get( Settings::DEBUG_CAMERA_DISPLAY );
    const bool c_debug_show_background = Settings::get( Settings::DEBUG_SHOW_BACKGROUND );
    const bool c_debug_background_color = Settings::get( Settings::DEBUG_BACKGROUND_COLOR );
    const bool c_debug_show_visual_bbox = Settings::get( Settings::DEBUG_SHOW_VISUAL_BBOX );
    #endif

    Color lens_bounds_color = BLACK;
    Color world_bounds_color = BLACK;

    #if defined( AXN_DEBUG )
    world_bounds_color = RED;
    if( Debug::active && c_debug_camera_display )
    {
        world_bounds_color.a( 0.5 );
        lens_bounds_color.a( 0.5 );
    }
    #endif

    #if defined( AXN_DEBUG )
    if( c_debug_show_background )
    {
        #endif
        if( world( ) && world( )->current_room( ) )
        {
            ogl::clear_color( world( )->current_room( )->fill_color( ) );
        }
        #if defined( AXN_DEBUG )
    }
    else
    {
        if( c_debug_background_color )
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

            dec scale = 1.0; // C! how much this visible is scaled on screen, which outlines that keep their thickness undo

            ogl_matrix( )
            {
                if( !in_fixed )
                {
                    if( !is_zero( in_visible.z( ) ) )
                    {
                        scale = ( c_zoom * in_visible.z( ) );

                        ogl::scale( scale );
                        ogl::translate( -( c_camera_center.x( ) ),
                                        -( c_camera_center.y( ) ) );
                    }
                }

                ogl::translate( in_visible.center( ).x( ), in_visible.center( ).y( ) );

                #if defined( AXN_DEBUG )
                if( Debug::active && c_debug_show_visual_bbox )
                {
                    render_convex_polygon( { WHITE.a( 0.15 ) }, in_visible.bounding_box( ).points( ) );
                }
                #endif

                if( in_visible.opacity( ) < 1.0 )
                {
                    render_faded_drawing( in_visible, scale );
                }
                else
                {
                    render_drawing( in_visible, scale ); // C! was always c_zoom, wrong for fixed visibles and ones not at z = 1
                }
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

            if( world( ) && world( )->current_room( ) && world( )->current_room( )->lighting_active( ) )
            {
                if( ptr<const Lighting> lighting = world( )->current_room( )->lighting( ) )
                {
                    if( lighting->ambient_color( ).is_visible( ) )
                    {
                        ogl::depth_always( );
                        render_convex_polygon( { lighting->ambient_color( ) }, c_screen.points( ) );
                    }

                    varray<LightSource> cref light_sources = lighting->light_sources( );

                    if( !is_empty( light_sources ) )
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

                                one_color[ 0 ] = CLEAR;

                                for_each_const( light, light_sources )
                                {
                                    render_convex_polygon( one_color,
                                                           circle_points( light.radius( ) * ( ( i * LIGHTING_RADIUS_GROW * pow( LIGHTING_RADIUS_GROW_EXPONENT, i ) ) + 1 ), light.position( ) ) );
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
                        render_convex_polygon( { lens_bounds_color.a( percent ) }, lens_bounds( ).points( ) );
                    }

                    b_case( SWIPE ) :
                    {
                        Point start, end;
                        Angle a = m_transition_vec.angle( ).truncated( );

                        if( a.in_quadrant( Q1, true ) )
                        {
                            start = lens_bounds( ).bottom_left( );
                            end = lens_bounds( ).top_right( );
                        }
                        else if( a.in_quadrant( Q2, true ) )
                        {
                            start = lens_bounds( ).bottom_right( );
                            end = lens_bounds( ).top_left( );
                        }
                        else if( a.in_quadrant( Q3, true ) )
                        {
                            start = lens_bounds( ).top_right( );
                            end = lens_bounds( ).bottom_left( );
                        }
                        else // if( a.in_quadrant( Q4, true ) )
                        {
                            start = lens_bounds( ).top_left( );
                            end = lens_bounds( ).bottom_right( );
                        }

                        Vector distance_total = end - start;
                        Vector distance = distance_total * percent;

                        Planc d = x2( max( width( ), height( ) ) );

                        Vector vec = start + distance;

                        Vector side1 = vec + VectorA( a + RIGHT, d );
                        Vector back1 = side1 - VectorA( a, d );

                        Vector side2 = vec + VectorA( a - RIGHT, d );
                        Vector back2 = side2 - VectorA( a, d );

                        // C! the swipe cropped to the lens
                        Drawing swipe;
                        swipe.draw( lens_bounds_color, Polygon( { side1, back1, back2, side2 } ) );
                        swipe.add_bounds( Polygon( lens_bounds( ).points( ) ) );

                        render_drawing( swipe, 1.0 );
                    }

                    b_case( CIRCLE ) :
                    {
                        Max<Planc> max_distance;

                        Vector vec = m_transition_vec - center( );

                        max_distance.update( ( vec - lens_bounds( ).top_left( ) ).magnitude( ) );
                        max_distance.update( ( vec - lens_bounds( ).top_right( ) ).magnitude( ) );
                        max_distance.update( ( vec - lens_bounds( ).bottom_right( ) ).magnitude( ) );
                        max_distance.update( ( vec - lens_bounds( ).bottom_left( ) ).magnitude( ) );

                        Planc radius = max_distance.value( ) * one_minus( percent );

                        ogl::stencil_mask( true );
                        ogl::stencil_not_equal( );
                        ogl::stencil_add( );

                        render_convex_polygon( { CLEAR }, Polygon::circle( radius, vec ).points( ) );
                        render_convex_polygon( { lens_bounds_color }, lens_bounds( ).points( ) );
                    }
                }
            }
            else if( transitioned( ) )
            {
                render_convex_polygon( { lens_bounds_color }, lens_bounds( ).points( ) );
            }
        };

        lambdaf( render_lens_bounds )( )
        {
            ogl::stencil_mask( true );
            ogl::stencil_not_equal( );
            ogl::stencil_add( );

            ogl::clear_stencil( );

            if( world( ) && world( )->current_room( ) )
            {
                render_convex_polygon( { CLEAR }, ( ( world( )->current_room( )->bounds( ) * c_zoom ) - ( center( ) * c_zoom ) ).points( ) );
                render_convex_polygon( { world_bounds_color }, c_screen.points( ) );

                ogl::clear_stencil( );
            }

            render_convex_polygon( { CLEAR }, lens_bounds( ).points( ) );
            render_convex_polygon( { lens_bounds_color }, c_screen.points( ) );

            ogl::clear_stencil( );
        };

        // C! a screen effect or HUD element drawn, then cropped to its own bounds( ), so nothing it draws spills outside them
        // C! (its drawing is cleared every render, so the crop is added fresh each time)
        lambdaf( render_screen_visual )( ScreenVisual ref in_visual )
        {
            in_visual.render( this );
            in_visual.add_bounds( Polygon( in_visual.bounds( this ).points( ) ) );
            render_visible( in_visual, true );
        };

        lambdaf( render_screen_effects )( )
        {
            for_each( screen_effect, m_screen_effects ) { render_screen_visual( *screen_effect ); }
        };

        lambdaf( render_hud_elements )( )
        {
            if( show_hud( ) )
            {
                for_each( hud_element, m_hud_elements ) { render_screen_visual( *hud_element ); }
            }

            for_each( always_hud_element, m_always_hud_elements ) { render_screen_visual( *always_hud_element ); }
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

                if( c_debug_camera_display )
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

    #if defined( TRACK_POLYS )

    if( poly_count )
    {
        total_render_count++;
        total_poly_count += poly_count;
        total_pt_count += pt_count;
        max_poly_count = max( poly_count, max_poly_count );
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

FixedRectangle Camera::bounds( bool in_positioned ) const { return( FixedRectangle( width( ), height( ), ( in_positioned ? center( ) : ORIGIN ) ) ); }
FixedRectangle Camera::lens_bounds( ) const { return( FixedRectangle( width( ), ( cinematic( ) ? ( height( ) * CINEMATIC_HEIGHT_RATIO ) : height( ) ), ORIGIN ) ); }

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
    m_target += d_target_offset; // C! m_target already holds the offset, so only the change in it

    cursor_world_position( screen_to_world( cursor_screen_position ) );
}

dec Camera::min_zoom( ) const { return( MIN_ZOOM ); }
dec Camera::max_zoom( ) const { return( MAX_ZOOM ); }

bool Camera::show_hud( ) const { return( m_show_hud ); }
void Camera::show_hud( bool in_show_hud ) { m_show_hud = in_show_hud; }

bool Camera::cinematic( ) const { return( m_cinematic ); }
void Camera::cinematic( bool in_cinematic ) { m_cinematic = in_cinematic; }

dec Camera::hud_offset_percentage( ) const { return( m_hud_offset_percentage ); }
void Camera::hud_offset_percentage( dec in_hud_offset_percentage ) { m_hud_offset_percentage = in_hud_offset_percentage; }

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

bool Camera::in_render_range( ptr<const Object> in_object ) const
{
    return_false_if( is_null( in_object ) ); // C! nothing to render

    FixedRectangle grown = in_object->max_bounding_box( ) + in_object->position( );
    grown.expand( x2( render_buffer( ) ) ); // C! expand( ) grows the whole width and height, so twice the buffer is the buffer on each side

    return( in_view( grown, in_object->z( ) ) );
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

void Camera::cursor_world_position_reset( ) { m_cursor_world_position = INVALID_PT; }

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

Camera::Shutter ref Camera::shutter( ) { return( m_shutter ); }

Camera::HUD::HUD( dec in_center_x_percent, dec in_center_y_percent, dec in_width_percent, dec in_height_percent )
{
    width( in_width_percent );
    height( in_height_percent );
    center( in_center_x_percent, in_center_y_percent );
}

FixedRectangle Camera::HUD::bounds( ptr<Camera> camera ) const
{
    // C! sized and placed by the whole frame, so cinematic mode doesn't shrink it
    Planc width = camera->width( );
    Planc height = camera->height( );

    Planc offset = min( width, height ) * camera->hud_offset_percentage( );

    width -= x2( offset );
    height -= x2( offset );

    Planc display_width = width * m_width_percent;
    Planc display_height = height * m_height_percent;

    Point center = Point( width * ( m_center_x_percent - 0.5 ), height * ( m_center_y_percent - 0.5 ) );

    // C! then moved in by the cinematic bars on the side it's nearer, so it keeps the same distance from the edge of what's shown
    // C! (the lens is only ever shorter than the frame, and something centered on the screen doesn't move)
    Planc bar_width = half( camera->width( ) - camera->lens_bounds( ).width( ) );
    Planc bar_height = half( camera->height( ) - camera->lens_bounds( ).height( ) );

    if( m_center_x_percent > 0.5 ) { center.x( center.x( ) - bar_width ); }
    else if( m_center_x_percent < 0.5 ) { center.x( center.x( ) + bar_width ); }

    if( m_center_y_percent > 0.5 ) { center.y( center.y( ) - bar_height ); }
    else if( m_center_y_percent < 0.5 ) { center.y( center.y( ) + bar_height ); }

    return( FixedRectangle( display_width, display_height, center ) );
}

void Camera::HUD::width( dec in_width_percent ) { m_width_percent = in_width_percent; }
void Camera::HUD::height( dec in_height_percent ) { m_height_percent = in_height_percent; }
void Camera::HUD::center( dec in_center_x_percent, dec in_center_y_percent ) { m_center_x_percent = in_center_x_percent; m_center_y_percent = in_center_y_percent; }

void Camera::Shutter::render( ptr<Camera> in_camera )
{
    ScreenVisual::render( in_camera );

    return_if( m_countdown.completed( ) || ( m_sides < 3 ) );

    const dec c_progress = one_minus( m_countdown.percentage( ) ); // C! 0 on the first frame (update( ) skips its first tick), so a screenshot taken then doesn't show it
    const dec c_closing = one_minus( abs( x2( c_progress ) - 1.0 ) ); // C! 0 open, 1 shut halfway through, then back to 0
    const dec c_shut = square( c_closing ) * ( 3.0 - x2( c_closing ) ); // C! eased, so the blades slow down at both ends

    const FixedRectangle c_screen = in_camera->bounds( false );
    const dec c_half_diagonal = half( pythagorean( dec( c_screen.width( ) ), dec( c_screen.height( ) ) ) );

    const Angle c_blade_angle = Angle::fraction( dec( m_sides ) );
    const dec c_open_radius = c_half_diagonal / half( c_blade_angle ).cos( ); // C! open wide enough that the opening's flat sides clear the screen's corners
    const dec c_radius = c_open_radius * one_minus( c_shut );
    const dec c_reach = x2( c_open_radius + c_half_diagonal ); // C! how far the blades run out past the opening, off the screen
    const Angle c_twist = c_blade_angle * c_shut; // C! iris blades turn as they close

    for_range( i, m_sides )
    {
        const Angle c_corner_angle = RIGHT + c_twist + ( c_blade_angle * dec( i ) );
        const Point c_corner = ORIGIN + VectorA( c_corner_angle, c_radius );

        // C! each blade fills the wedge outside one corner of the opening, between the side ending there (carried on past it) and the side starting there
        const Point c_back = c_corner + VectorA( c_corner_angle + RIGHT - half( c_blade_angle ), c_reach );
        const Point c_front = c_corner + VectorA( c_corner_angle + RIGHT + half( c_blade_angle ), c_reach );

        draw( SHUTTER_COLOR, Polygon( { c_corner, c_back, c_front } ) );
        draw( SHUTTER_EDGE_COLOR, Line( c_corner, c_back ), SHUTTER_EDGE_THICKNESS );
    }
}

void Camera::Shutter::update( ptr<Camera> in_camera )
{
    ScreenVisual::update( in_camera );

    if( m_starting ) // C! the update runs before the render, so without this the screenshot's frame would already be partly shut
    {
        m_starting = false;
        return;
    }

    if( m_countdown.tick( ) )
    {
        if( Engine::paused( ) && m_paused_engine ) // C! done, so undo start( )'s pause, unless something unpaused it already
        {
            Engine::pause( false );
        }

        m_paused_engine = false;
    }
}

void Camera::Shutter::start( )
{
    m_countdown.reset( m_duration );
    m_starting = true;

    if( !Engine::paused( ) ) // C! a pause that was already on is left alone, and stays on after
    {
        Engine::pause( true );

        m_paused_engine = true;
    }

    Engine::play_sound( SoundFX( CLICK_SOUND ).global( true ) ); // C! the player taking it hears it, wherever they are
}
