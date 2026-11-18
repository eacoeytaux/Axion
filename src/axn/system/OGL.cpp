#include "axn/system/OGL.hpp"

#include "axn/system/OS.hpp"

#undef ref

// ------------------- //
#if defined( OS_WINDOWS )
// ------------------- //
#include <GLFW/glfw3.h>
// ------------------- //
#elif defined( OS_APPLE )
// ------------------- //
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
// ------------------- //
#elif defined( OS_LINUX )
// ------------------- //
// #include <opengl.h>
// ------------------- //
#endif

namespace axn
{

Error ogl::m_error = no_error;
int ogl::m_error_code = GL_NO_ERROR;
bool ogl::m_disable_error_check = false;

bool ogl::m_initialized = false;
bool ogl::m_initializing = false;

uint ogl::m_attrib_stack_count = 0;
uint ogl::m_matrix_stack_count = 0;

bool ogl::m_started_sequence = false;

bool ogl::m_depth_mask = false;
bool ogl::m_stencil_mask = false;

Error ogl::error_check( )
{
    if( !m_disable_error_check )
    {
        m_error_code = glGetError( );

        if( m_error_code )
        {
            m_error = error_system; // TODO use error_code instead
        }
    }

    return( m_error );
}

Error ogl::reset( )
{
    error_check( );

    while( m_matrix_stack_count )
    {
        pop_matrix( );

        m_matrix_stack_count--;
    }

    while( m_attrib_stack_count )
    {
        pop_attribute( );

        m_attrib_stack_count--;
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

    load_identity( );

    m_initializing = false;

    return( error_check( ) );
}

Error ogl::init( )
{
    Error init_error = reset( );

    m_initialized = !init_error;

    return( init_error );
}

Error ogl::close( )
{
    m_initialized = false;

    return( reset( ) );
}

Error ogl::clear( uint in_gl_bit )
{
    glClear( in_gl_bit );

    return( error_check( ) );
}

Error ogl::clear( )
{
    clear_color( );
    clear_depth( );
    clear_stencil( );

    return( error_check( ) );
}

Error ogl::clear_color( )
{
    return( clear( GL_COLOR_BUFFER_BIT ) );
}

Error ogl::clear_color( Color cref in_color, bool in_clear )
{
    glClearColor( in_color.r( ), in_color.g( ), in_color.b( ), 1.0 );

    if( in_clear )
    {
        error_check( );
        clear_color( );
    }

    return( error_check( ) );
}

Error ogl::clear_depth( )
{
    bool mask = m_depth_mask;

    depth_mask( true );

    glClearDepth( 0 );
    error_check( );

    clear( GL_DEPTH_BUFFER_BIT );

    depth_mask( mask );

    return( error_check( ) );
}

Error ogl::clear_stencil( )
{
    bool mask = m_stencil_mask;

    stencil_mask( true );

    glClearStencil( 0 );
    error_check( );

    clear( GL_STENCIL_BUFFER_BIT );

    stencil_mask( mask );

    return( error_check( ) );
}

bool ogl::state_bool( uint in_gl_enum )
{
    GLboolean b;

    glGetBooleanv( in_gl_enum, &b );
    error_check( );

    return( b );
}

int ogl::state_int( uint in_gl_enum )
{
    GLint i;

    glGetIntegerv( in_gl_enum, &i );
    error_check( );

    return( i );
}

double ogl::state_double( uint in_gl_enum )
{
    GLdouble d;

    glGetDoublev( in_gl_enum, &d );
    error_check( );

    return( d );
}

Error ogl::hint( uint in_gl_enum_target, uint in_gl_enum )
{
    glHint( in_gl_enum_target, in_gl_enum );

    return( error_check( ) );
}

Error ogl::enable( uint in_gl_enum )
{
    glEnable( in_gl_enum );

    return( error_check( ) );
}

bool ogl::enabled( uint in_gl_enum )
{
    return( glIsEnabled( in_gl_enum ) );
}

Error ogl::disable( uint in_gl_enum )
{
    glDisable( in_gl_enum );

    return( error_check( ) );
}

bool ogl::disabled( uint in_gl_enum )
{
    return( !enabled( in_gl_enum ) );
}

Error ogl::load_identity( )
{
    glLoadIdentity( );

    return( error_check( ) );
}

Error ogl::push_attribute( uint in_attribute )
{
    m_attrib_stack_count++;

    glPushAttrib( in_attribute );

    return( error_check( ) );
}

Error ogl::push_attribute( )
{
    m_attrib_stack_count++;

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    return( error_check( ) );
}

Error ogl::pop_attribute( )
{
    Assert( is_pos( m_attrib_stack_count ), "attribute stack is empty" );

    glPopAttrib( );

    m_attrib_stack_count--;

    return( error_check( ) );
}

Error ogl::push_matrix( )
{
    glPushMatrix( );

    m_matrix_stack_count++;

    return( error_check( ) );
}

Error ogl::pop_matrix( )
{
    Assert( is_pos( m_matrix_stack_count ), "matrix stack is empty" );

    glPopMatrix( );

    m_matrix_stack_count--;

    return( error_check( ) );
}

Error ogl::transform( Transform cref in_trans )
{
    dec matrix[ 16 ] = {
        in_trans.get( 0, 0 ),
        in_trans.get( 1, 0 ),
        in_trans.get( 2, 0 ), 0,
        in_trans.get( 0, 1 ),
        in_trans.get( 1, 1 ),
        in_trans.get( 2, 1 ), 0,
        in_trans.get( 0, 2 ),
        in_trans.get( 1, 2 ),
        in_trans.get( 2, 2 ), 0,
        0, 0, 0, 1 };

    glMultMatrixd( matrix );
    return( error_check( ) );
}

Error ogl::translate( Planc cref in_x, Planc cref in_y )
{
    glTranslated( in_x, in_y, 0.0 );

    return( error_check( ) );
}

Error ogl::scale( Planc cref in_x, Planc cref in_y )
{
    glScaled( in_x, in_y, 1.0 );

    return( error_check( ) );
}

Error ogl::begin_points( ) { return( begin( GL_POINTS ) ); }
Error ogl::begin_lines( ) { return( begin( GL_LINES ) ); }
Error ogl::begin_lines_strip( ) { return( begin( GL_LINE_STRIP ) ); }
Error ogl::begin_lines_loop( ) { return( begin( GL_LINE_LOOP ) ); }
Error ogl::begin_triangles( ) { return( begin( GL_TRIANGLES ) ); }
Error ogl::begin_triangles_strip( ) { return( begin( GL_TRIANGLE_STRIP ) ); }
Error ogl::begin_triangles_fan( ) { return( begin( GL_TRIANGLE_FAN ) ); }
Error ogl::begin_polygons( ) { return( begin( GL_POLYGON ) ); }

Error ogl::begin( uint in_gl_enum )
{
    glBegin( in_gl_enum );

    m_disable_error_check = true;
    m_started_sequence = true;

    return( error_check( ) );
}

Error ogl::end( )
{
    glEnd( );

    if( m_disable_error_check )
    {
        m_disable_error_check = false;
    }

    m_started_sequence = false;

    return( error_check( ) );
}

Error ogl::vertex( dec in_x, dec in_y, dec in_z, dec in_w )
{
    return_if( !m_started_sequence, error_system );

    glVertex4d( in_x, in_y, in_z, in_w );

    return( error_check( ) );
}

Error ogl::faces_counterclockwise( bool in_ccw )
{
    glFrontFace( in_ccw ? GL_CCW : GL_CW );

    return( error_check( ) );
}

bool ogl::faces_counterclockwise( )
{
    return( state_int( GL_FRONT_FACE ) == GL_CCW );
}

Error ogl::shade_flat( )
{
    glShadeModel( GL_FLAT );

    return( error_check( ) );
}

bool ogl::shaded_flat( )
{
    return( state_int( GL_SHADE_MODEL ) == GL_FLAT );
}

Error ogl::shade_smooth( )
{
    glShadeModel( GL_SMOOTH );

    return( error_check( ) );
}

bool ogl::shaded_smooth( )
{
    return( state_int( GL_SHADE_MODEL ) == GL_SMOOTH );
}

Error ogl::point_radius( dec in_d )
{
    return( point_diameter( x2( in_d ) ) );
}

dec ogl::point_radius( )
{
    return( point_diameter( ) / 2.0 );
}

Error ogl::point_diameter( dec in_d )
{
    glPointSize( in_d );

    return( error_check( ) );
}

dec ogl::point_diameter( )
{
    return( state_double( GL_POINT_SIZE ) );
}

Error ogl::line_width( dec in_d )
{
    glLineWidth( in_d );

    return( error_check( ) );
}

dec ogl::line_width( )
{
    return( state_double( GL_LINE_WIDTH ) );
}

Error ogl::color( Color cref in_color )
{
    glColor4d( in_color.r( ), in_color.g( ), in_color.b( ), in_color.a( ) );

    return( error_check( ) );
}

Error ogl::blend_normal( )
{
    enable( GL_BLEND );

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    // glBlendEquation( GL_FUNC_ADD );

    return( error_check( ) );
}

Error ogl::blend_clear( )
{
    enable( GL_BLEND );

    glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
    // glBlendEquation( GL_FUNC_ADD );

    return( error_check( ) );
}

Error ogl::enable_anti_alias( )
{
    #if !defined( OS_WINDOWS )
    enable( GL_MULTISAMPLE );
    #endif

    enable( GL_LINE_SMOOTH );
    enable( GL_POLYGON_SMOOTH );

    static_once
    {
        hint( GL_LINE_SMOOTH_HINT, GL_NICEST );
        hint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    }

    return( error_check( ) );
}

Error ogl::disable_anti_alias( )
{
    #if !defined( OS_WINDOWS )
    disable( GL_MULTISAMPLE );
    #endif

    disable( GL_LINE_SMOOTH );
    disable( GL_POLYGON_SMOOTH );

    return( error_check( ) );
}

Error ogl::depth_func( uint in_gl_enum )
{
    glDepthFunc( in_gl_enum );

    return( error_check( ) );
}

Error ogl::depth_always( )
{
    return( depth_func( GL_ALWAYS ) );
}

Error ogl::depth_never( )
{
    return( depth_func( GL_NEVER ) );
}

Error ogl::depth_equal( )
{
    return( depth_func( GL_EQUAL ) );
}

Error ogl::depth_not_equal( )
{
    return( depth_func( GL_NOTEQUAL ) );
}

Error ogl::depth_equal_greater( )
{
    return( depth_func( GL_GEQUAL ) );
}

Error ogl::depth_greater( )
{
    return( depth_func( GL_GREATER ) );
}

Error ogl::depth_equal_less( )
{
    return( depth_func( GL_LEQUAL ) );
}

Error ogl::depth_less( )
{
    return( depth_func( GL_LESS ) );
}

Error ogl::depth_mask( bool in_mask )
{
    m_depth_mask = in_mask;

    glDepthMask( m_depth_mask ? 0xFF : 0x0 );

    return( ogl::error_check( ) );
}

bool ogl::depth_mask( )
{
    return( m_depth_mask );
}

Error ogl::stencil_func( uint in_gl_enum )
{
    glStencilFunc( in_gl_enum, 1, 0xFF );

    return( ogl::error_check( ) );
}

Error ogl::stencil_always( )
{
    return( stencil_func( GL_ALWAYS ) );
}

Error ogl::stencil_never( )
{
    return( stencil_func( GL_NEVER ) );
}

Error ogl::stencil_equal( )
{
    return( stencil_func( GL_EQUAL ) );
}

Error ogl::stencil_not_equal( )
{
    return( stencil_func( GL_NOTEQUAL ) );
}

Error ogl::stencil_equal_greater( )
{
    return( stencil_func( GL_GEQUAL ) );
}

Error ogl::stencil_greater( )
{
    return( stencil_func( GL_GREATER ) );
}

Error ogl::stencil_equal_less( )
{
    return( stencil_func( GL_LEQUAL ) );
}

Error ogl::stencil_less( )
{
    return( stencil_func( GL_LESS ) );
}

Error ogl::stencil_mask( bool in_mask )
{
    m_stencil_mask = in_mask;

    glStencilMask( m_stencil_mask ? 0xFF : 0x0 );

    return( error_check( ) );
}

bool ogl::stencil_mask( )
{
    return( m_stencil_mask );
}

Error ogl::stencil_add( )
{
    glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );

    return( error_check( ) );
}

Error ogl::stencil_remove( )
{
    glStencilOp( GL_KEEP, GL_KEEP, GL_ZERO );

    return( error_check( ) );
}

} // namespace axn
