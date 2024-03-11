#include "OGL.hpp"

#if defined( __APPLE__ ) // osx
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
// TODO if linux
// TODO if windows
#endif

namespace axn {

#define ogl_check( f )                                                                         \
    if( !m_initialized && !m_initializing )                                                    \
        return ogl::set_error( error_not_init );                                               \
    if( err start_err = ogl::check_errors( ) )                                                 \
        return ogl::set_error( start_err );                                                    \
    { f }                                                                                      \
    return ogl::check_errors( );
#define ogl_attrib_check( f )                                                                  \
    ogl_check( if( ( ogl::m_attrib_stack_count == 0 ) &&                                       \
                   !m_initializing ) return ogl::set_error( error_todo );                      \
               f )
#define ogl_matrix_check( f )                                                                  \
    ogl_check( if( ( ogl::m_matrix_stack_count == 0 ) &&                                       \
                   !m_initializing ) return ogl::set_error( error_todo );                      \
               f )

err ogl::m_error = no_error;
bool ogl::m_initialized = false;
bool ogl::m_initializing = false;
uint ogl::m_attrib_stack_count = 0;
uint ogl::m_matrix_stack_count = 0;
bool ogl::m_started_sequence = false;

inline int check_errors_gl( ) { return glGetError( ); }
err ogl::check_errors( ) {
    return set_error( ( check_errors_gl( ) == GL_NO_ERROR ) ? no_error : error_todo );
}
err ogl::set_error( const err _e ) {
    return ogl::m_error = ( ogl::m_error == no_error ) ? _e : ogl::m_error;
}

bool ogl::state_bool( const int _gl_enum ) {
    ogl::check_errors( );
    GLboolean b;
    glGetBooleanv( _gl_enum, &b );
    ogl::check_errors( );
    return b;
}
int ogl::state_int( const int _gl_enum ) {
    ogl::check_errors( );
    GLint i;
    glGetIntegerv( _gl_enum, &i );
    ogl::check_errors( );
    return i;
}
float ogl::state_float( const int _gl_enum ) {
    ogl::check_errors( );
    GLfloat f;
    glGetFloatv( _gl_enum, &f );
    ogl::check_errors( );
    return f;
}

err ogl::enable( const int _gl_enum ) {
    ogl_attrib_check( { glEnable( _gl_enum ); } )
}
bool ogl::enabled( const int _gl_enum ) { return glIsEnabled( _gl_enum ); }

err ogl::disable( const int _gl_enum ) {
    ogl_attrib_check( { glDisable( _gl_enum ); } )
}
bool ogl::disabled( const int _gl_enum ) { return !ogl::enabled( _gl_enum ); }

err ogl::init( ) {
    err init_err = reset( );
    m_initialized = !init_err;
    return init_err;
}
err ogl::close( ) {
    m_initialized = false;
    return reset( );
}
err ogl::reset( ) {
    while( m_matrix_stack_count ) {
        glPopMatrix( );
        --m_matrix_stack_count;
    }
    while( m_attrib_stack_count ) {
        glPopAttrib( );
        --m_attrib_stack_count;
    }

    m_initializing = true;

    clear_screen( 0.0, 0.0, 0.0 );

    ogl::faces_counterclockwise( true );
    ogl::blend_normal( );

    m_initializing = false;

    return ogl::check_errors( );
}

err ogl::clear( const int _gl_bit ) { ogl_check( { glClear( _gl_bit ); } ) }

err ogl::clear_screen( const float _r, const float _g, const float _b ) {
    glClearColor( _r, _g, _b, 1.f );
    return ogl::clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT |
                       GL_ACCUM_BUFFER_BIT );
}

err ogl::push_attribute( ) { ogl_check( {
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    ++m_attrib_stack_count;
} ) } err ogl::pop_attribute( ) { ogl_check( {
    if( Assert( m_attrib_stack_count > 0 ) ) {
        glPopAttrib( );
        --m_attrib_stack_count;
    }
} ) }

err ogl::push_matrix( ) { ogl_check( {
    glPushMatrix( );
    ++m_matrix_stack_count;
} ) } err ogl::pop_matrix( ) { ogl_check( {
    if( Assert( m_matrix_stack_count > 0 ) ) {
        glPopMatrix( );
        --m_matrix_stack_count;
    }
} ) }

err ogl::matrix_modelview_mode( const bool _load_identity ) { ogl_matrix_check( {
    glMatrixMode( GL_MODELVIEW );
    if( _load_identity )
        glLoadIdentity( );
} ) } err ogl::matrix_projection_mode( const bool _load_identity ) { ogl_matrix_check( {
    glMatrixMode( GL_PROJECTION );
    if( _load_identity )
        glLoadIdentity( );
} ) }

err ogl::begin_points( ) {
    return ogl::begin( GL_POINTS );
}
err ogl::begin_lines( ) { return ogl::begin( GL_LINES ); }
err ogl::begin_lines_strip( ) { return ogl::begin( GL_LINE_STRIP ); }
err ogl::begin_lines_loop( ) { return ogl::begin( GL_LINE_LOOP ); }
err ogl::begin_triangles( ) { return ogl::begin( GL_TRIANGLES ); }
err ogl::begin_triangles_strip( ) { return ogl::begin( GL_TRIANGLE_STRIP ); }
err ogl::begin_triangles_fan( ) { return ogl::begin( GL_TRIANGLE_FAN ); }
err ogl::begin_polygons( ) { return ogl::begin( GL_POLYGON ); }

err ogl::begin( const int _gl_enum ) { // if ( m_started_sequence ) return error_todo; if (
                                       // err e = check_errors( ) ) return e;
    glBegin( _gl_enum );
    m_started_sequence = true;
    return check_errors( );
}
err ogl::end( ) { // if ( !m_started_sequence ) return error_todo; if ( err e =
                  // check_errors( ) ) return e;
    glEnd( );
    m_started_sequence = false;
    return check_errors( );
}

err ogl::vertex( const float _x, const float _y, const float _z, const float _w ) {
    if( m_started_sequence )
        return error_todo;
    if( err e = check_errors( ) )
        return e;
    glVertex4f( _x, _y, _z, _w );
    return check_errors( );
}

err ogl::faces_counterclockwise( const bool _ccw ) {
    ogl_attrib_check( { glFrontFace( _ccw ? GL_CCW : GL_CW ); } )
}
bool ogl::faces_counterclockwise( ) { return ( ogl::state_int( GL_FRONT_FACE ) == GL_CCW ); }

err ogl::shade_flat( ) {
    ogl_attrib_check( { glShadeModel( GL_FLAT ); } )
}
bool ogl::shaded_flat( ) { return ( ogl::state_int( GL_SHADE_MODEL ) == GL_FLAT ); }

err ogl::shade_smooth( ) {
    ogl_attrib_check( { glShadeModel( GL_SMOOTH ); } )
}
bool ogl::shaded_smooth( ) { return ( ogl::state_int( GL_SHADE_MODEL ) == GL_SMOOTH ); }

err ogl::point_radius( const float _f ) { return point_diameter( _f * 2.0 ); }
float ogl::point_radius( ) { return ( point_diameter( ) / 2.0f ); }
err ogl::point_diameter( const float _f ) {
    ogl_attrib_check( { glPointSize( _f ); } )
}
float ogl::point_diameter( ) { return ogl::state_float( GL_POINT_SIZE ); }

err ogl::line_width( const float _f ) {
    ogl_attrib_check( { glLineWidth( _f ); } )
}
float ogl::line_width( ) { return ogl::state_float( GL_LINE_WIDTH ); }

err ogl::color( const float _r, const float _g, const float _b,
                const float _a ) { ogl_attrib_check( { glColor4f( _r, _g, _b, _a ); } ) }

err ogl::blend_normal( ) { ogl_attrib_check( {
    ogl::enable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glBlendEquation( GL_FUNC_ADD );
} ) }

err ogl::depth_always( ) { ogl_attrib_check( {
    glDepthFunc( GL_ALWAYS );
} ) } err ogl::depth_never( ) { ogl_attrib_check( {
    glDepthFunc( GL_NEVER );
} ) } err ogl::depth_equal( ) { ogl_attrib_check( {
    glDepthFunc( GL_EQUAL );
} ) } err ogl::depth_not_equal( ) { ogl_attrib_check( {
    glDepthFunc( GL_NOTEQUAL );
} ) } err ogl::depth_greater( const bool _equal ) { ogl_attrib_check( {
    glDepthFunc( _equal ? GL_GEQUAL : GL_GREATER );
} ) } err ogl::depth_less( const bool _equal ) {
    ogl_attrib_check( { glDepthFunc( _equal ? GL_LEQUAL : GL_LESS ); } )
}

} // namespace axn
