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

    static Error init( );
    static Error close( );

    static Error error_check( );

private:

    static Error clear( uint in_gl_bits );

public:

    static Error clear( );

    static Error clear_color( );
    static Error clear_color( Color cref in_color, bool in_clear = true );
    static Error clear_clear_color( bool in_clear = true ) { return( ogl::clear_color( BLACK, in_clear ) ); }

    static Error clear_depth( );

    static Error clear_stencil( );

    static Error load_identity( );

private:

    static Error push_attribute( uint in_attribute );
    static Error push_attribute( );
    static Error pop_attribute( );

    static Error push_matrix( );
    static Error pop_matrix( );

public:

    static Error transform( Transform cref in_trans );

    static Error translate( Planc cref in_dx, Planc cref in_dy );
    static Error translate( Point cref in_pt ) { return( translate( in_pt.x( ), in_pt.y( ) ) ); }
    static Error translate( Vector cref in_vec ) { return( translate( in_vec.dx( ), in_vec.dy( ) ) ); }

    static Error scale( Planc cref in_x, Planc cref in_y );
    static Error scale( Planc cref in_x ) { return( scale( in_x, in_x ) ); }

private:

    static Error begin( uint in_gl_enum );

public:

    static Error end( );

    static Error begin_points( );
    static Error begin_lines( );
    static Error begin_lines_strip( );
    static Error begin_lines_loop( );
    static Error begin_triangles( );
    static Error begin_triangles_strip( );
    static Error begin_triangles_fan( );
    static Error begin_polygons( );

    static Error vertex( dec in_x, dec in_y, dec in_z = 1.0, dec in_w = 1.0 );

    static Error faces_counterclockwise( bool in_ccw );
    static bool faces_counterclockwise( );

    static Error shade_flat( );
    static bool shaded_flat( );

    static Error shade_smooth( );
    static bool shaded_smooth( );

    static Error point_radius( dec in_radius );
    static dec point_radius( );

    static Error point_diameter( dec in_diameter );
    static dec point_diameter( );

    static Error line_width( dec in_width );
    static dec line_width( );

    static Error color( Color cref in_color );

    static Error blend_normal( );
    static Error blend_clear( );

    static Error enable_anti_alias( );
    static Error disable_anti_alias( );

private:

    static Error depth_func( uint in_gl_enum );

public:

    static Error depth_always( );
    static Error depth_never( );
    static Error depth_equal( );
    static Error depth_not_equal( );
    static Error depth_equal_greater( );
    static Error depth_greater( );
    static Error depth_equal_less( );
    static Error depth_less( );

    static Error depth_mask( bool in_mask );
    static bool depth_mask( );

private:

    static Error stencil_func( uint in_gl_enum );

public:

    static Error stencil_always( );
    static Error stencil_never( );
    static Error stencil_equal( );
    static Error stencil_not_equal( );
    static Error stencil_equal_greater( );
    static Error stencil_greater( );
    static Error stencil_equal_less( );
    static Error stencil_less( );

    static Error stencil_mask( bool in_mask );
    static bool stencil_mask( );

    static Error stencil_add( );
    static Error stencil_remove( );

private:

    static Error m_error;
    static int m_error_code;
    static bool m_disable_error_check;

    static bool m_initialized;
    static bool m_initializing;

    static uint m_attrib_stack_count;
    static uint m_matrix_stack_count;

    static bool m_started_sequence;

    static bool m_depth_mask;
    static bool m_stencil_mask;

    static Error reset( );

    static bool state_bool( uint in_gl_enum );
    static int state_int( uint in_gl_enum );
    static double state_double( uint in_gl_enum );

    static Error hint( uint in_gl_enum_target, uint in_gl_enum );

    static Error enable( uint in_gl_enum );
    static bool enabled( uint in_gl_enum );

    static Error disable( uint in_gl_enum );
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
