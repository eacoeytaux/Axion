#include "OGL.hpp"

#include "OS.hpp"
#if defined( OS_WINDOWS )
// ---------------- //
#include <GLFW/glfw3.h>
// ---------------- //
#elif defined( OS_APPLE )
// ---------------- //
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
// ---------------- //
#elif defined( OS_LINUX )
// ---------------- //
//                  //
// ---------------- //
#endif

namespace axn
{

error ogl::m_error = no_error;
int ogl::m_error_code = GL_NO_ERROR;
bool ogl::m_disable_error_check = false;
bool ogl::m_initialized = false;
bool ogl::m_initializing = false;
uint ogl::m_attrib_stack_count = 0;
uint ogl::m_matrix_stack_count = 0;
bool ogl::m_started_sequence = false;
bool ogl::m_depth_mask = false;
bool ogl::m_stencil_mask = false;

int error_check_gl( )
{
    return glGetError( );
}

error ogl::error_check( )
{
    if( !m_disable_error_check )
    {
        m_error_code = error_check_gl( );
        if( m_error_code )
        {
            m_error = error_system;
            Assert( );
        }
    }

    return m_error;
}

error ogl::reset( )
{
    error_check( );

    while( m_matrix_stack_count )
    {
        pop_matrix( );
        --m_matrix_stack_count;
    }
    while( m_attrib_stack_count )
    {
        pop_attribute( );
        --m_attrib_stack_count;
    }

    m_initializing = true;

    clear( );

    faces_counterclockwise( true );
    blend_normal( );

    enable( GL_DEPTH_TEST );
    depth_mask( true );
    depth_always( );

    enable( GL_STENCIL_TEST );
    stencil_add( );
    stencil_mask( false );
    stencil_always( );

    {
        // GLchar * shader_vertex_text[] = {
        //    (GLchar *)"#version 110 \n",
        //    (GLchar *)"layout(location = 0) in vec3 position_in; \n",
        //    (GLchar *)"layout(location = 1) in vec3 color_in; \n",
        //    (GLchar *)"out vec3 color_out; \n",
        //    (GLchar *)"void main( ) \n",
        //    (GLchar *)"{ \n",
        //    (GLchar *)"    color_out = color_in; \n",
        //    (GLchar *)"} \n",
        //    (GLchar *)"\n" };
        //
        // GLchar * shader_fragment_text[] = {
        //    (GLchar *)"#version 110 \n",
        //    (GLchar *)"in vec3 color_in; \n",
        //    (GLchar *)"out vec4 color_out; \n",
        //    (GLchar *)"void main( ) \n",
        //    (GLchar *)"{ \n",
        //    (GLchar *)"    color_out = vec4( color_in, 1.0 ); \n",
        //    (GLchar *)"} \n",
        //    (GLchar *)"\n" };

        // GLuint shader_vertex_id = glCreateShader( GL_VERTEX_SHADER );
        // glShaderSource( shader_vertex_id, 9, shader_vertex_text, nullptr );
        // glCompileShader( shader_vertex_id );

        // GLuint shader_fragment_id = glCreateShader( GL_FRAGMENT_SHADER );
        // glShaderSource( shader_fragment_id, 8, shader_fragment_text, nullptr );
        // glCompileShader( shader_fragment_id );

        // GLint is_compiled = GL_TRUE;
        // glGetShaderiv( shader_vertex_id, GL_COMPILE_STATUS, &is_compiled );
        // glGetShaderiv( shader_fragment_id, GL_COMPILE_STATUS, &is_compiled );

        // GLuint program_id = glCreateProgram( );
        // glAttachShader( program_id, shader_vertex_id );
        // glAttachShader( program_id, shader_fragment_id );
        // glLinkProgram( program_id );

        // GLint is_linked = GL_TRUE;
        // glGetProgramiv( program_id, GL_LINK_STATUS, &is_linked );

        // glUseProgram( program_id );

        // glDetachShader( program_id, shader_vertex_id );
        // glDetachShader( program_id, shader_fragment_id );
        // glDeleteShader( shader_vertex_id );
        // glDeleteShader( shader_fragment_id );
    }

    load_identity( );
    m_initializing = false;
    return error_check( );
}

bool ogl::state_bool( const int _gl_enum )
{
    error_check( );
    GLboolean b;
    glGetBooleanv( _gl_enum, &b );
    error_check( );
    return b;
}

int ogl::state_int( const int _gl_enum )
{
    error_check( );
    GLint i;
    glGetIntegerv( _gl_enum, &i );
    error_check( );
    return i;
}

double ogl::state_double( const int _gl_enum )
{
    error_check( );
    GLdouble d;
    glGetDoublev( _gl_enum, &d );
    error_check( );
    return d;
}

error ogl::hint( const int _gl_enum_target, const int _gl_enum )
{
    glHint( _gl_enum_target, _gl_enum );
    return error_check( );
}

error ogl::enable( const int _gl_enum )
{
    glEnable( _gl_enum );
    return error_check( );
}

bool ogl::enabled( const int _gl_enum )
{
    return glIsEnabled( _gl_enum );
}

error ogl::disable( const int _gl_enum )
{
    glDisable( _gl_enum );
    return error_check( );
}

bool ogl::disabled( const int _gl_enum )
{
    return !enabled( _gl_enum );
}

error ogl::init( )
{
    error init_err = reset( );
    m_initialized = !init_err;
    return init_err;
}

error ogl::close( )
{
    m_initialized = false;
    return reset( );
}

error ogl::clear( const int _gl_bit )
{
    glClear( _gl_bit );
    return error_check( );
}

error ogl::clear( )
{
    clear_color( );
    clear_depth( );
    clear_stencil( );

    return error_check( );
}

error ogl::clear_color( const dec _r, const dec _g, const dec _b )
{
    glClearColor( _r, _g, _b, 1.0 );
    error_check( );

    return clear( GL_COLOR_BUFFER_BIT );
}

error ogl::clear_depth( )
{
    bool mask = m_depth_mask;

    depth_mask( true );

    glClearDepth( 0 );
    error_check( );

    clear( GL_DEPTH_BUFFER_BIT );

    depth_mask( mask );

    return error_check( );
}

error ogl::clear_stencil( )
{
    bool mask = m_stencil_mask;

    stencil_mask( true );

    glClearStencil( 0 );
    error_check( );

    clear( GL_STENCIL_BUFFER_BIT );

    stencil_mask( mask );

    return error_check( );
}

error ogl::load_identity( )
{
    glLoadIdentity( );
    return error_check( );
}

error ogl::push_attribute( )
{
    ++m_attrib_stack_count;
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    return error_check( );
}

error ogl::pop_attribute( )
{
    if( Assert( m_attrib_stack_count > 0 ) )
    {
        glPopAttrib( );
        --m_attrib_stack_count;
    }

    return error_check( );
}

error ogl::push_matrix( )
{
    glPushMatrix( );
    ++m_matrix_stack_count;
    return error_check( );
}

error ogl::pop_matrix( )
{
    if( Assert( m_matrix_stack_count > 0 ) )
    {
        glPopMatrix( );
        --m_matrix_stack_count;
    }

    return error_check( );
}

error ogl::matrix_modelview_mode( const bool _load_identity )
{
    glMatrixMode( GL_MODELVIEW );

    if( _load_identity )
    {
        load_identity( );
    }

    return error_check( );
}

error ogl::matrix_projection_mode( const bool _load_identity )
{
    glMatrixMode( GL_PROJECTION );

    if( _load_identity )
    {
        load_identity( );
    }

    return error_check( );
}

error ogl::transform( const Transform & _transform )
{
    dec matrix[ 16 ] = {
        _transform.get( 0, 0 ),
        _transform.get( 1, 0 ),
        _transform.get( 2, 0 ), 0,
        _transform.get( 0, 1 ),
        _transform.get( 1, 1 ),
        _transform.get( 2, 1 ), 0,
        _transform.get( 0, 2 ),
        _transform.get( 1, 2 ),
        _transform.get( 2, 2 ),
        0, 0, 0, 0, 1 };

    glMultMatrixd( matrix );
    return error_check( );
}

error ogl::translate( const dec _x, const dec _y )
{
    glTranslated( _x, _y, ZERO );
    return error_check( );
}

error ogl::scale( const dec _x, const dec _y )
{
    glScaled( _x, _y, 1.0 );
    return error_check( );
}

error ogl::begin_points( ) { return begin( GL_POINTS ); }
error ogl::begin_lines( ) { return begin( GL_LINES ); }
error ogl::begin_lines_strip( ) { return begin( GL_LINE_STRIP ); }
error ogl::begin_lines_loop( ) { return begin( GL_LINE_LOOP ); }
error ogl::begin_triangles( ) { return begin( GL_TRIANGLES ); }
error ogl::begin_triangles_strip( ) { return begin( GL_TRIANGLE_STRIP ); }
error ogl::begin_triangles_fan( ) { return begin( GL_TRIANGLE_FAN ); }
error ogl::begin_polygons( ) { return begin( GL_POLYGON ); }

error ogl::begin( const int _gl_enum )
{
    glBegin( _gl_enum );
    m_disable_error_check = true;
    m_started_sequence = true;
    return error_check( );
}

error ogl::end( )
{
    glEnd( );
    if( m_disable_error_check )
    {
        m_disable_error_check = false;
    }
    m_started_sequence = false;
    return error_check( );
}

error ogl::vertex( const dec _x, const dec _y, const dec _z, const dec _w )
{
    if( !m_started_sequence )
    {
        return error_system;
    }

    glVertex4d( _x, _y, _z, _w );
    return error_check( );
}

error ogl::faces_counterclockwise( const bool _ccw )
{
    glFrontFace( _ccw ? GL_CCW : GL_CW );
    return error_check( );
}

bool ogl::faces_counterclockwise( )
{
    return ( state_int( GL_FRONT_FACE ) == GL_CCW );
}

error ogl::shade_flat( )
{
    glShadeModel( GL_FLAT );
    return error_check( );
}

bool ogl::shaded_flat( )
{
    return ( state_int( GL_SHADE_MODEL ) == GL_FLAT );
}

error ogl::shade_smooth( )
{
    glShadeModel( GL_SMOOTH );
    return error_check( );
}

bool ogl::shaded_smooth( )
{
    return ( state_int( GL_SHADE_MODEL ) == GL_SMOOTH );
}

error ogl::point_radius( const dec _d )
{
    return point_diameter( _d * 2.0 );
}

dec ogl::point_radius( )
{
    return ( point_diameter( ) / 2.0 );
}

error ogl::point_diameter( const dec _d )
{
    glPointSize( _d );
    return error_check( );
}

dec ogl::point_diameter( )
{
    return state_double( GL_POINT_SIZE );
}

error ogl::line_width( const dec _d )
{
    glLineWidth( _d );
    return error_check( );
}

dec ogl::line_width( )
{
    return state_double( GL_LINE_WIDTH );
}

error ogl::color( const dec _r, const dec _g, const dec _b, const dec _a )
{
    glColor4d( _r, _g, _b, _a );
    return error_check( );
}

error ogl::blend_normal( )
{
    enable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    // glBlendEquation( GL_FUNC_ADD );
    return error_check( );
}

error ogl::blend_clear( )
{
    enable( GL_BLEND );
    glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
    // glBlendEquation( GL_FUNC_ADD );
    return error_check( );
}

error ogl::enable_anti_alias( )
{
    #ifndef OS_WINDOWS
    enable( GL_MULTISAMPLE );
    #endif

    enable( GL_LINE_SMOOTH );
    enable( GL_POLYGON_SMOOTH );

    static_once( )
    {
        hint( GL_LINE_SMOOTH_HINT, GL_NICEST );
        hint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    }

    return error_check( );
}

error ogl::disable_anti_alias( )
{
    #ifndef OS_WINDOWS
    disable( GL_MULTISAMPLE );
    #endif

    disable( GL_LINE_SMOOTH );
    disable( GL_POLYGON_SMOOTH );

    return error_check( );
}

error ogl::depth_func( const int _gl_enum )
{
    glDepthFunc( _gl_enum );
    return ogl::error_check( );
}

error ogl::depth_always( )
{
    return depth_func( GL_ALWAYS );
}

error ogl::depth_never( )
{
    return depth_func( GL_NEVER );
}

error ogl::depth_equal( )
{
    return depth_func( GL_EQUAL );
}

error ogl::depth_not_equal( )
{
    return depth_func( GL_NOTEQUAL );
}

error ogl::depth_equal_greater( )
{
    return depth_func( GL_GEQUAL );
}

error ogl::depth_greater( )
{
    return depth_func( GL_GREATER );
}

error ogl::depth_equal_less( )
{
    return depth_func( GL_LEQUAL );
}

error ogl::depth_less( )
{
    return depth_func( GL_LESS );
}

error ogl::depth_mask( const bool b )
{
    m_depth_mask = b;
    glDepthMask( m_depth_mask ? 0xFF : 0x0 );
    return ogl::error_check( );
}

bool ogl::depth_mask( )
{
    return m_depth_mask;
}

error ogl::stencil_func( const int _gl_enum )
{
    glStencilFunc( _gl_enum, 1, 0xFF );
    return ogl::error_check( );
}

error ogl::stencil_always( )
{
    return stencil_func( GL_ALWAYS );
}

error ogl::stencil_never( )
{
    return stencil_func( GL_NEVER );
}

error ogl::stencil_equal( )
{
    return stencil_func( GL_EQUAL );
}

error ogl::stencil_not_equal( )
{
    return stencil_func( GL_NOTEQUAL );
}

error ogl::stencil_equal_greater( )
{
    return stencil_func( GL_GEQUAL );
}

error ogl::stencil_greater( )
{
    return stencil_func( GL_GREATER );
}

error ogl::stencil_equal_less( )
{
    return stencil_func( GL_LEQUAL );
}

error ogl::stencil_less( )
{
    return stencil_func( GL_LESS );
}

error ogl::stencil_mask( const bool b )
{
    m_stencil_mask = b;
    glStencilMask( m_stencil_mask ? 0xFF : 0x0 );
    return error_check( );
}

bool ogl::stencil_mask( )
{
    return m_stencil_mask;
}

error ogl::stencil_add( )
{
    glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
    return error_check( );
}

error ogl::stencil_remove( )
{
    glStencilOp( GL_ZERO, GL_ZERO, GL_ZERO );
    return error_check( );
}

} // namespace axn
