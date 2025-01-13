#ifndef OGL_hpp
#define OGL_hpp

#include "Geometry.hpp"

namespace axn
{

class ogl
{
private:
    ogl( ) { }

public:
    static error init( );
    static error close( );

private:
    static error clear( int gl_bits );

public:
    static error clear( );

    static error clear_color( dec r, dec g, dec b );
    static error clear_color( ) { return ogl::clear_color( 0.0, 0.0, 0.0 ); }

    static error clear_depth( );

    static error clear_stencil( );

    static error load_identity( );

    static error push_attribute( );
    static error pop_attribute( );

    static error push_matrix( );
    static error pop_matrix( );

    static error matrix_modelview_mode( bool load_identity = true );
    static error matrix_projection_mode( bool load_identity = true );

    static error transform( Transform cref );

    static error translate( dec x, dec y );
    static error translate( Coordinate cref c ) { return translate( c.x( ), c.y( ) ); }
    static error translate( Vector cref v ) { return translate( v.dx( ), v.dy( ) ); }

    static error scale( dec s ) { return scale( s, s ); }
    static error scale( dec x, dec y );

private:
    static error begin( int gl_enum );

public:
    static error end( );

    static error begin_points( );
    static error begin_lines( );
    static error begin_lines_strip( );
    static error begin_lines_loop( );
    static error begin_triangles( );
    static error begin_triangles_strip( );
    static error begin_triangles_fan( );
    static error begin_polygons( );

    static error vertex( dec x, dec y, dec z = 1.0, dec w = 1.0 );

    static error faces_counterclockwise( bool );
    static bool faces_counterclockwise( );

    static error shade_flat( );
    static bool shaded_flat( );

    static error shade_smooth( );
    static bool shaded_smooth( );

    static error point_radius( dec );
    static dec point_radius( );

    static error point_diameter( dec );
    static dec point_diameter( );

    static error line_width( dec );
    static dec line_width( );

    static error color( dec r, dec g, dec b, dec a = 1.0 );

    static error blend_normal( );
    static error blend_clear( );

    static error enable_anti_alias( );
    static error disable_anti_alias( );

private:
    static error depth_func( int gl_enum );

public:
    static error depth_always( );
    static error depth_never( );
    static error depth_equal( );
    static error depth_not_equal( );
    static error depth_equal_greater( );
    static error depth_greater( );
    static error depth_equal_less( );
    static error depth_less( );

    static error depth_mask( bool );
    static bool depth_mask( );

private:
    static error stencil_func( int gl_enum );

public:
    static error stencil_always( );
    static error stencil_never( );
    static error stencil_equal( );
    static error stencil_not_equal( );
    static error stencil_equal_greater( );
    static error stencil_greater( );
    static error stencil_equal_less( );
    static error stencil_less( );

    static error stencil_mask( bool );
    static bool stencil_mask( );

    static error stencil_add( );
    static error stencil_remove( );

private:
    static error m_error;
    static int m_error_code;
    static bool m_disable_error_check;

    static bool m_initialized;
    static bool m_initializing;

    static uint m_attrib_stack_count;
    static uint m_matrix_stack_count;

    static bool m_started_sequence;

    static bool m_depth_mask;
    static bool m_stencil_mask;

    static error reset( );

    static bool state_bool( int gl_enum );
    static int state_int( int gl_enum );
    static double state_double( int gl_enum );

    static error hint( int gl_enum_target, int gl_enum );

    static error enable( int gl_enum );
    static bool enabled( int gl_enum );

    static error disable( int gl_enum );
    static bool disabled( int gl_enum );

public:
    static error error_check( );
};

} // namespace axn

#endif /* OGL_hpp */
