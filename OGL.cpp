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
// todo
// ---------------- //
#endif

namespace axn
{

error ogl::m_error = no_error;
bool ogl::m_initialized = false;
bool ogl::m_initializing = false;
uint ogl::m_attrib_stack_count = 0;
uint ogl::m_matrix_stack_count = 0;
bool ogl::m_started_sequence = false;

int check_errors_gl( )
{
    return glGetError( );
}

error ogl::check_errors( )
{
    return set_error( ( check_errors_gl( ) == GL_NO_ERROR ) ? no_error : error_todo );
}

error ogl::set_error( const error _e )
{
    return m_error = ( m_error == no_error ) ? _e : m_error;
}

error ogl::reset( )
{
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

    clear_screen( 0.0, 0.0, 0.0 );

    faces_counterclockwise( true );
    blend_normal( );

    enable( GL_DEPTH_TEST );
    depth_always( );

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
    return check_errors( );
}

bool ogl::state_bool( const int _gl_enum )
{
    check_errors( );
    GLboolean b;
    glGetBooleanv( _gl_enum, &b );
    check_errors( );
    return b;
}

int ogl::state_int( const int _gl_enum )
{
    check_errors( );
    GLint i;
    glGetIntegerv( _gl_enum, &i );
    check_errors( );
    return i;
}

double ogl::state_double( const int _gl_enum )
{
    check_errors( );
    GLdouble d;
    glGetDoublev( _gl_enum, &d );
    check_errors( );
    return d;
}

error ogl::hint( const int _gl_enum_target, const int _gl_enum )
{
    glHint( _gl_enum_target, _gl_enum );
    return check_errors( );
}

error ogl::enable( const int _gl_enum )
{
    glEnable( _gl_enum );
    return check_errors( );
}

bool ogl::enabled( const int _gl_enum )
{
    return glIsEnabled( _gl_enum );
}

error ogl::disable( const int _gl_enum )
{
    glDisable( _gl_enum );
    return check_errors( );
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
    return check_errors( );
}

error ogl::clear( )
{
    clear_screen( );
    clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    return check_errors( );
}

error ogl::clear_color( )
{
    clear( GL_COLOR_BUFFER_BIT );
    return check_errors( );
}

error ogl::clear_depth( )
{
    clear( GL_DEPTH_BUFFER_BIT );
    return check_errors( );
}

error ogl::clear_stencil( )
{
    clear( GL_STENCIL_BUFFER_BIT );
    return check_errors( );
}

error ogl::clear_screen( const double _r, const double _g, const double _b )
{
    glClearColor( _r, _g, _b, 1.0 );
    return clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT );
}

error ogl::load_identity( )
{
    glLoadIdentity( );
    return check_errors( );
}

error ogl::push_attribute( )
{
    ++m_attrib_stack_count;
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    return check_errors( );
}

error ogl::pop_attribute( )
{
    if( Assert( m_attrib_stack_count > 0 ) )
    {
        glPopAttrib( );
        --m_attrib_stack_count;
    }

    return check_errors( );
}

error ogl::push_matrix( )
{
    glPushMatrix( );
    ++m_matrix_stack_count;
    return check_errors( );
}

error ogl::pop_matrix( )
{
    if( Assert( m_matrix_stack_count > 0 ) )
    {
        glPopMatrix( );
        --m_matrix_stack_count;
    }

    return check_errors( );
}

error ogl::matrix_modelview_mode( const bool _load_identity )
{
    glMatrixMode( GL_MODELVIEW );

    if( _load_identity )
    {
        load_identity( );
    }
    
    return check_errors( );
}

error ogl::matrix_projection_mode( const bool _load_identity )
{
    glMatrixMode( GL_PROJECTION );

    if( _load_identity )
    {
        load_identity( );
    }
    
    return check_errors( );
}

error ogl::transform( const Transform & _transform )
{
    double matrix[ 16 ] = {
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
    return check_errors( );
}

error ogl::translate( const double _x, const double _y )
{
    glTranslatef( _x, _y, 0.0 );
    return check_errors( );
}

error ogl::scale( const double _x, const double _y )
{
    glScalef( _x, _y, 1.0 );
    return check_errors( );
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
    m_started_sequence = true;
    return check_errors( );
}

error ogl::end( )
{
    glEnd( );
    m_started_sequence = false;
    return check_errors( );
}

error ogl::vertex( const double _x, const double _y, const double _z, const double _w )
{
    if( !m_started_sequence )
    {
        return error_todo;
    }
    
    glVertex4f( _x, _y, _z, _w );
    return check_errors( );
}

error ogl::faces_counterclockwise( const bool _ccw )
{
    glFrontFace( _ccw ? GL_CCW : GL_CW );
    return check_errors( );
}

bool ogl::faces_counterclockwise( )
{
    return ( state_int( GL_FRONT_FACE ) == GL_CCW );
}

error ogl::shade_flat( )
{
    glShadeModel( GL_FLAT );
    return check_errors( );
}

bool ogl::shaded_flat( )
{
    return ( state_int( GL_SHADE_MODEL ) == GL_FLAT );
}

error ogl::shade_smooth( )
{
    glShadeModel( GL_SMOOTH );
    return check_errors( );
}

bool ogl::shaded_smooth( )
{
    return ( state_int( GL_SHADE_MODEL ) == GL_SMOOTH );
}

error ogl::point_radius( const double _f )
{
    return point_diameter( _f * 2.0 );
}

double ogl::point_radius( )
{
    return ( point_diameter( ) / 2.0 );
}

error ogl::point_diameter( const double _f )
{
    glPointSize( _f );
    return check_errors( );
}

double ogl::point_diameter( )
{
    return state_double( GL_POINT_SIZE );
}

error ogl::line_width( const double _f )
{
    glLineWidth( _f );
    return check_errors( );
}

double ogl::line_width( )
{
    return state_double( GL_LINE_WIDTH );
}

error ogl::color( const double _r, const double _g, const double _b, const double _a )
{
    glColor4f( _r, _g, _b, _a );
    return check_errors( );
}

error ogl::blend_normal( )
{
    enable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    // glBlendEquation( GL_FUNC_ADD );
    return check_errors( );
}

error ogl::enable_anti_alias( )
{
    // enable( GL_MULTISAMPLE );
    enable( GL_LINE_SMOOTH );
    enable( GL_POLYGON_SMOOTH );
    static_once( )
    {
        hint( GL_LINE_SMOOTH_HINT, GL_NICEST );
        hint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    }
    return check_errors( );
}

error ogl::disable_anti_alias( )
{
    // disable( GL_MULTISAMPLE );
    disable( GL_LINE_SMOOTH );
    disable( GL_POLYGON_SMOOTH );
    return check_errors( );
}

error ogl::depth_func( const int _gl_enum )
{
    glDepthFunc( _gl_enum );
    return ogl::check_errors( );
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

error ogl::depth_greater( const bool _equal )
{
    return depth_func( _equal ? GL_GEQUAL : GL_GREATER );
}

error ogl::depth_less( const bool _equal )
{
    return depth_func( _equal ? GL_LEQUAL : GL_LESS );
}

} // namespace axn
