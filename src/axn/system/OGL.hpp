#ifndef OGL_hpp
#define OGL_hpp

#include "axn/graphics/Color.hpp"
#include "axn/geometry/Geometry.hpp"

namespace axn
{

class ogl
{

private:

    ogl( ) { }

public:

    static error init( );
    static error close( );

    static error error_check( );

private:

    static error clear( uint in_gl_bits );

public:

    static error clear( );

    static error clear_color( );
    static error clear_color( Color cref in_color, bool in_clear = true );
    static error clear_clear_color( bool in_clear = true ) { return( ogl::clear_color( BLACK, in_clear ) ); }

    static error clear_depth( );

    static error clear_stencil( );

    static error load_identity( );

private:

    static error push_attribute( uint in_attribute );
    static error push_attribute( );
    static error pop_attribute( );

    static error push_matrix( );
    static error pop_matrix( );

public:

    static error transform( Transform cref in_trans );

    static error translate( Planc cref in_dx, Planc cref in_dy );
    static error translate( Point cref in_pt ) { return( translate( in_pt.x( ), in_pt.y( ) ) ); }
    static error translate( Vector cref in_vec ) { return( translate( in_vec.dx( ), in_vec.dy( ) ) ); }

    static error scale( Planc cref in_x, Planc cref in_y );
    static error scale( Planc cref in_x ) { return( scale( in_x, in_x ) ); }

private:

    static error begin( uint in_gl_enum );

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

    static error vertex( dec in_x, dec in_y, dec in_z = 1.0, dec in_w = 1.0 );

    static error faces_counterclockwise( bool in_ccw );
    static bool faces_counterclockwise( );

    static error shade_flat( );
    static bool shaded_flat( );

    static error shade_smooth( );
    static bool shaded_smooth( );

    static error point_radius( dec in_radius );
    static dec point_radius( );

    static error point_diameter( dec in_diameter );
    static dec point_diameter( );

    static error line_width( dec in_width );
    static dec line_width( );

    static error color( Color cref in_color );

    static error blend_normal( );
    static error blend_clear( );

    static error enable_anti_alias( );
    static error disable_anti_alias( );

private:

    static error depth_func( uint in_gl_enum );

public:

    static error depth_always( );
    static error depth_never( );
    static error depth_equal( );
    static error depth_not_equal( );
    static error depth_equal_greater( );
    static error depth_greater( );
    static error depth_equal_less( );
    static error depth_less( );

    static error depth_mask( bool in_mask );
    static bool depth_mask( );

private:

    static error stencil_func( uint in_gl_enum );

public:

    static error stencil_always( );
    static error stencil_never( );
    static error stencil_equal( );
    static error stencil_not_equal( );
    static error stencil_equal_greater( );
    static error stencil_greater( );
    static error stencil_equal_less( );
    static error stencil_less( );

    static error stencil_mask( bool in_mask );
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

    static bool state_bool( uint in_gl_enum );
    static int state_int( uint in_gl_enum );
    static double state_double( uint in_gl_enum );

    static error hint( uint in_gl_enum_target, uint in_gl_enum );

    static error enable( uint in_gl_enum );
    static bool enabled( uint in_gl_enum );

    static error disable( uint in_gl_enum );
    static bool disabled( uint in_gl_enum );

public:

    struct matrix
    {

        matrix( ) { ogl::push_matrix( ); }
        ~matrix( ) { ogl::pop_matrix( ); }

        operator bool( ) const { return( true ); }

    };

    struct attribute
    {

        attribute( uint in_attribute ) { ogl::push_attribute( in_attribute ); }

        attribute( ) { ogl::push_attribute( ); }
        ~attribute( ) { ogl::pop_attribute( ); }

        operator bool( ) const { return( true ); }

    };

};

#define ogl_matrix( ) if( ogl::matrix omatrix = ogl::matrix( ) )
#define ogl_matrix_clear( ) ogl::clear( ); ogl_matrix( )

#define ogl_attribute( ... ) if( ogl::attribute oattribute = ogl::attribute( __VA_ARGS__ ) )
#define ogl_attribute_clear( ... ) ogl::clear( ); ogl_attribute( __VA_ARGS__ )

} // namespace axn

#endif /* OGL_hpp */
