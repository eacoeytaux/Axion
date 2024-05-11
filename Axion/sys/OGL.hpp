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

    static error clear_color( );
    static error clear_depth( );
    static error clear_stencil( );

    static error clear_screen( ) { return ogl::clear_screen( 0.0, 0.0, 0.0 ); }
    static error clear_screen( double r, double g, double b );

    static error load_identity( );

    static error push_attribute( );
    static error pop_attribute( );

    static error push_matrix( );
    static error pop_matrix( );

    static error matrix_modelview_mode( bool load_identity = true );
    static error matrix_projection_mode( bool load_identity = true );

    static error scale( double s ) { return scale( s, s ); }
    static error scale( double x, double y );
    static error translate( double x, double y );
    static error translate( const Coordinate & c ) { return translate( c.x( ), c.y( ) ); }
    static error transform( const Transform & ); // TODO

private:
    static error begin( int gl_enum );

public:
    static error begin_points( );
    static error begin_lines( );
    static error begin_lines_strip( );
    static error begin_lines_loop( );
    static error begin_triangles( );
    static error begin_triangles_strip( );
    static error begin_triangles_fan( );
    static error begin_polygons( );
    static error end( );

    static error vertex( double x, double y, double z = 1.0, double w = 1.0 );

    static error faces_counterclockwise( bool );
    static bool faces_counterclockwise( );

    static error shade_flat( );
    static bool shaded_flat( );

    static error shade_smooth( );
    static bool shaded_smooth( );

    static error point_radius( double );
    static double point_radius( );

    static error point_diameter( double );
    static double point_diameter( );

    static error line_width( double );
    static double line_width( );

    static error color( double r, double g, double b, double a = 1.0 );

    static error blend_normal( );
    static error blend_func( );

    static error enable_anti_alias( );
    static error disable_anti_alias( );

private:
    static error depth_func( int gl_enum );

public:
    static error depth_always( );
    static error depth_never( );
    static error depth_equal( );
    static error depth_not_equal( );
    static error depth_greater( bool equal );
    static error depth_less( bool equal );

private:
    static error m_error;
    static bool m_initialized;
    static bool m_initializing;

    static uint m_attrib_stack_count;
    static uint m_matrix_stack_count;

    static bool m_started_sequence;

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
    static error check_errors( );
    static error set_error( error );
};

} // namespace axn

#endif /* OGL_hpp */
