#include "axn/system/Logger.hpp"

#include "axn/AXN.hpp"
#include "axn/system/Engine.hpp"

#if defined( AXN_DEBUG )

#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>

var_const( string LOGS_DIR = ( "logs" ) );
var_const( string MAIN_LOG = ( "main" ) );

var_static( mutex log_mutex );

var_static( bool b_initialized = ( false ) );

var_static( bool b_using_console = ( true ) );

var_static( bool b_paused = ( false ) );
var_static( stack<bool> b_paused_stack );

struct LogFile { file * opened = nullptr; uset<message_type> types; bool tried = false; };
var_static( umap<string, LogFile> log_files );

file * log_file( string cref in_log, LogFile ref in_log_file )
{
    return_if( in_log_file.tried, in_log_file.opened );

    in_log_file.tried = true;

    string log_filename = Engine::run_directory( "logs" ) + in_log + ".log";

    #if defined( OS_WINDOWS )
    fopen_s( &in_log_file.opened, charify( log_filename ), "w+" );
    #elif defined( OS_APPLE )
    in_log_file.opened = fopen( charify( log_filename ), "w+" );
    #elif defined( OS_LINUX )
    in_log_file.opened = fopen( charify( log_filename ), "w+" );
    #endif

    return( in_log_file.opened );
}

Error Logger::close( )
{
    if( b_initialized )
    {
        log_message( INFO_LOG, "complete ............... :)\n" );
    }

    b_initialized = false;

    lock_guard<mutex> lock( log_mutex );

    Error error = no_error;

    for_each( log, log_files )
    {
        if( log.second.opened && fclose( log.second.opened ) ) { error = error_system; }
    }

    log_files.clear( );

    return( error );
}

Error Logger::init( bool in_print_to_file )
{
    b_paused = false;
    b_initialized = false;

    if( in_print_to_file ) { add_log( MAIN_LOG ); }

    b_initialized = true;

    log_message( INFO_LOG, "starting ............... ( v%s )\n", AXN_VERSION );

    return( no_error );
}

Error Logger::add_log( string cref in_name, initializer_list<message_type> in_types )
{
    lock_guard<mutex> lock( log_mutex );

    return_if( log_files.contains( in_name ), error_system ); // C! already a log by that name

    LogFile ref log = log_files.insert( in_name );
    for_each_const( type, in_types ) { log.types.insert( type ); }

    return( no_error );
}

Error Logger::log_message( message_type in_type, const char * in_entry, ... )
{
    return_if( !b_initialized || b_paused, error_not_init );

    try
    {
        lock_guard<mutex> lock( log_mutex );

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
                type_str = "DEBUG";
            }
        }

        va_list va_args;
        va_start( va_args, in_entry );

        if( b_using_console )
        {
            va_list console_args;
            va_copy( console_args, va_args ); // C! a va_list can only be read once, so each place written to gets a copy
            printf( "[ %s ] ", charify( type_str ) );
            vprintf( in_entry, console_args );
            printf( "\n" );
            va_end( console_args );
        }

        for_each( log, log_files )
        {
            continue_if( !log.second.types.empty( ) && !log.second.types.contains( in_type ) ); // C! not a type this log takes

            file * opened = log_file( log.first, log.second );
            continue_if( !opened );

            va_list file_args;
            va_copy( file_args, va_args );
            fprintf( opened, "[ %s ] ", charify( timestamp( ':' ) ) );
            fprintf( opened, "[ %s ] ", charify( type_str ) );
            vfprintf( opened, in_entry, file_args );
            fprintf( opened, "\n" );
            va_end( file_args );

            fflush( opened );
        }

        va_end( va_args );
    }
    catch_error_return( );

    return( no_error );
}

bool Logger::printing_to_console( ) { return( b_using_console ); }
Error Logger::print_to_console( bool in_print ) { b_using_console = in_print; return( no_error ); }

bool Logger::paused( ) { return( b_paused ); }
Error Logger::pause( bool in_pause ) { b_paused = in_pause; return( no_error ); }

Error Logger::pop_pause( ) { Assert( b_paused_stack.size( ) ); return( pause( b_paused_stack.pop( ) ) ); }
Error Logger::push_pause( bool in_pause ) { b_paused_stack.push( in_pause ); return( pause( in_pause ) ); }

#endif
