#include "axn/system/Logger.hpp"

#include "axn/AXN.hpp"

#if defined( AXN_DEBUG )

#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>

var_static( bool b_initialized = ( false ) );

var_static( bool b_paused = ( false ) );
var_static( stack<bool> b_paused_stack );

var_static( bool b_using_file = ( false ) );
var_static( bool b_using_console = ( true ) );

var_static( string log_directory = ( "logs/" ) );
var_static( file * log_file = ( nullptr ) );

bool Logger::printing_to_console( ) { return( b_using_console ); }
Error Logger::print_to_console( bool in_print ) { b_using_console = in_print; return( no_error ); }

bool Logger::paused( ) { return( b_paused ); }
Error Logger::pause( bool in_pause ) { b_paused = in_pause; return( no_error ); }

Error Logger::pop_pause( ) { Assert( b_paused_stack.size( ) ); return( pause( b_paused_stack.pop( ) ) ); }
Error Logger::push_pause( bool in_pause ) { b_paused_stack.push( in_pause ); return( pause( in_pause ) ); }

Error Logger::init( bool in_file )
{
    b_paused = false;
    b_initialized = false;

    if( in_file )
    {
        string log_filename = log_directory + datestamp( ) + "-" + timestamp( ) + ".log";

        #if defined( OS_WINDOWS )
        fopen_s( &log_file, charify( log_filename ), "w+" );
        #elif defined( OS_APPLE )
        log_file = fopen( charify( log_filename ), "w+" );
        #elif defined( OS_LINUX )
        log_file = fopen( charify( log_filename ), "w+" );
        #endif

        return_if( !( b_using_file = log_file ), error_not_init );
    }

    b_initialized = true;

    log_message( INFO_LOG, "starting ............... ( v%s )\n", AXN_VERSION );

    return( no_error );
}

Error Logger::log_message( message_type in_type, const char * in_entry, ... )
{
    return_if( !b_initialized || b_paused, error_not_init );

    try
    {
        va_list va_args;
        va_start( va_args, in_entry );

        string type_str;

        switch( in_type )
        {
            default:
            {
                type_str = "~";
            }

            b_case( INFO_LOG ) :
            {
                type_str = "INFO";
            }

            b_case( WARNING_LOG ) :
            {
                type_str = "WARNING";
            }

            b_case( ERROR_LOG ) :
            {
                type_str = "ERROR";
            }

            b_case( DEBUG_LOG ) :
            {
                #if defined( AXN_DEBUG )
                type_str = "DEBUG";
                #else
                va_end( va_args );
                return( no_error );
                #endif
            }
        }

        if( b_using_console )
        {
            printf( "[ %s ] ", charify( type_str ) );
            vprintf( in_entry, va_args );
            printf( "\n" );
        }

        if( b_using_file )
        {
            fprintf( log_file, "[ %s ] ", charify( timestamp( ) ) );
            fprintf( log_file, "[ %s ] ", charify( type_str ) );
            vfprintf( log_file, in_entry, va_args );
            fprintf( log_file, "\n" );

            fflush( log_file );
        }

        va_end( va_args );
    }
    catch_error_return( );

    return( no_error );
}

Error Logger::close( )
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
