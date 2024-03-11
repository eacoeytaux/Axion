#ifndef OGL_hpp
#define OGL_hpp

#include "Primitives.hpp"

namespace axn {

class ogl {
public:
    static err init( );
    static err close( );

private:
    static err clear( int _gl_bits );

public:
    static err clear_screen( ) { return ogl::clear_screen( 0.f, 0.f, 0.f ); }
    static err clear_screen( float r, float g, float b );

    static err clear_all( ); // todo
    static err clear( ) { return no_error; }

    static err push_attribute( );
    static err pop_attribute( );

    static err push_matrix( );
    static err pop_matrix( );

    static err matrix_modelview_mode( bool load_identity = true );
    static err matrix_projection_mode( bool load_identity = true );

private:
    static err begin( int gl_enum );

public:
    static err begin_points( );
    static err begin_lines( );
    static err begin_lines_strip( );
    static err begin_lines_loop( );
    static err begin_triangles( );
    static err begin_triangles_strip( );
    static err begin_triangles_fan( );
    static err begin_polygons( );
    static err end( );

    static err vertex( float x, float y, float z = 0.0, float w = 1.0 );

    static err faces_counterclockwise( bool );
    static bool faces_counterclockwise( );

    static err shade_flat( );
    static bool shaded_flat( );

    static err shade_smooth( );
    static bool shaded_smooth( );

    static err point_radius( float );
    static float point_radius( );
    static err point_diameter( float );
    static float point_diameter( );

    static err line_width( float );
    static float line_width( );

    static err color( float r, float g, float b, float a = 1.0 );

    static err blend_normal( );

    static err depth_always( );
    static err depth_never( );
    static err depth_equal( );
    static err depth_not_equal( );
    static err depth_greater( bool equal );
    static err depth_less( bool equal );

private:
    static err m_error;
    static bool m_initialized;
    static bool m_initializing;

    static uint m_attrib_stack_count;
    static uint m_matrix_stack_count;

    static bool m_started_sequence;

    static err reset( );

    static bool state_bool( int gl_enum );
    static int state_int( int gl_enum );
    static float state_float( int gl_enum );

    static err enable( int gl_enum );
    static bool enabled( int gl_enum );

    static err disable( int gl_enum );
    static bool disabled( int gl_enum );

    static err check_errors( );
    static err set_error( err );
};

} // namespace axn

#endif /* OGL_hpp */
