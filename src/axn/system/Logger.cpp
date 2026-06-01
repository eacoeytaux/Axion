#include "axn/system/Logger.hpp"

#include "axn/AXN.hpp"

#if defined( AXN_DEBUG )

#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>

var_static( bool b_initialized = ( false ) );

var_static( bool b_paused = ( false ) );

var_static( bool b_using_file = ( false ) );
var_static( bool b_using_console = ( true ) );

var_static( string log_directory = ( "logs/" ) );

var_static( FILE * log_file = ( nullptr ) );

bool Logger::printing_to_console( ) { return( b_using_console ); }
error Logger::print_to_console( bool in_print ) { b_using_console = in_print; return( no_error ); }

bool Logger::paused( ) { return( b_paused ); }
error Logger::pause( bool in_pause ) { b_paused = in_pause; return( no_error ); }

error Logger::init( bool in_file )
{
    b_paused = false;
    b_initialized = false;

    if( in_file )
    {
        const char * log_filename_chars = ( log_directory + datestamp( ) + "-" + timestamp( ) + ".log" ).c_str( );

        #if defined( OS_WINDOWS )
        fopen_s( &log_file, log_filename_chars, "w+" );
        #elif defined( OS_APPLE )
        log_file = fopen( log_filename_chars, "w+" );
        #elif defined( OS_LINUX )
        log_file = fopen( log_filename_chars, "w+" );
        #endif

        return_if( !( b_using_file = log_file ), error_not_init );
    }

    b_initialized = true;

    log_message( INFO_LOG, "starting ............... ( v%s )\n", AXN_VERSION );

    return( no_error );
}

error Logger::log_message( message_type in_type, const char * in_entry, ... )
{
    return_if( !b_initialized || b_paused, error_not_init );

    try
    {
        va_list va_args;
        va_start( va_args, in_entry );

        char * type_str;

        switch( in_type )
        {
            default:
            {
                type_str = chars( "~" );
            }

            b_case( INFO_LOG ) :
            {
                type_str = chars( "INFO" );
            }

            b_case( WARNING_LOG ) :
            {
                type_str = chars( "WARNING" );
            }

            b_case( ERROR_LOG ) :
            {
                type_str = chars( "ERROR" );
            }

            b_case( DEBUG_LOG ) :
            {
                #if defined( AXN_DEBUG )
                type_str = chars( "DEBUG" );
                #else
                va_end( va_args );
                return( no_error );
                #endif
            }
        }

        if( b_using_console )
        {
            printf( "[ %s ] ", type_str );
            vprintf( in_entry, va_args );
            printf( "\n" );
        }

        if( b_using_file )
        {
            fprintf( log_file, "[ %s ] ", timestamp( ).c_str( ) );
            fprintf( log_file, "[ %s ] ", type_str );
            vfprintf( log_file, in_entry, va_args );
            fprintf( log_file, "\n" );

            fflush( log_file );
        }

        va_end( va_args );
    }
    catch( exception e )
    {
        cout << e.what( ) << endl;

        return( error_system );
    }

    return( no_error );
}

error Logger::close( )
{
    if( b_initialized )
    {
        log_message( INFO_LOG, "complete ............... :)\n" );
    }

    b_initialized = false;

    if( log_file )
    {
        return( fclose( log_file ) ? error_system : no_error );
    }
    else
    {
        return( no_error );
    }
}

#endif
