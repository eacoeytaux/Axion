#include "Logger.hpp"

#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>

#include "axn.hpp"

AXN_NAMESPACES

#define LOG_DIRECTORY "logs/"

bool b_initialized = false;
bool b_paused = false;
bool b_using_file = false;
bool b_using_console = true;
FILE* log_file = nullptr;

err Logger::print_to_console( const bool b ) {
    b_using_console = b;
    return no_error;
}
bool Logger::printing_to_console( ) { return b_using_console; }

err Logger::pause( bool p ) {
    b_paused = p;
    return no_error;
}
bool Logger::paused( ) { return b_paused; }

err Logger::init( const bool _file ) {
    b_initialized = b_paused = false;

    if( _file ) {
        Clock clock;
        log_file = fopen(
            ( LOG_DIRECTORY + clock.timestamp( "." ) + "." + clock.datestamp( "." ) + ".log" )
                .c_str( ),
            "w+" );
        if( !log_file )
            return error_not_init;
    }

    b_initialized = true;
    log( INFO_LOG, "starting ............... ( v%s )\n", VERSION );
    return no_error;
}

err Logger::log( MESSAGE_TYPE _type, const char* _entry, ... ) {
    if( ( !b_initialized ) || b_paused )
        return error_not_init;

    try {
        va_list va_args;
        va_start( va_args, _entry );

        Clock current;

        bool type = true;
        char* type_str;
        switch( _type ) {
        case INFO_LOG: {
            type = true;
            type_str = (char*)"INFO";
            break;
        }
        case WARNING_LOG: {
            type = true;
            type_str = (char*)"WARNING";
            break;
        }
        case ERROR_LOG: {
            type = true;
            type_str = (char*)"ERROR";
            break;
        }
        case DEBUG_LOG: {
#ifdef AXN_DEBUG
            type = true;
            type_str = (char*)"DEBUG";
            break;
#elif
            return;
#endif
        }
        default: {
            type = false;
            type_str = (char*)"";
            break;
        }
        }

        if( b_using_console ) {
            printf( "~ " ); // used for log filtering
            vprintf( _entry, va_args );
            printf( "\n" );
        }

        if( b_using_file ) {
            return_err( fprintf( log_file, "[%s] ", current.timestamp( ).c_str( ) ) );
            if( type )
                return_err( fprintf( log_file, "[%s] ", type_str ) );
            return_err( vfprintf( log_file, _entry, va_args ) );
            return_err( fprintf( log_file, "\n" ) );
            return_err( fflush( log_file ) );
        }

        va_end( va_args );
    } catch( exception e ) {
        cout << e.what( ) << endl;
        return error_todo;
    }

    return no_error;
}

err Logger::close( ) {
    if( b_initialized )
        log( INFO_LOG, "complete ...............\n" );
    b_initialized = false;

    return fclose( log_file ) ? error_todo : no_error;
}
