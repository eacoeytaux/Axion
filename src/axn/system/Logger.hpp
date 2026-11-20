#ifndef Logger_hpp
#define Logger_hpp

#include "axn/system/Debug.hpp"
#include "axn/system/Error.hpp"

namespace axn
{

#if defined( AXN_DEBUG )

enum message_type
{
    INFO_LOG,
    WARNING_LOG,
    ERROR_LOG,
    DEBUG_LOG,
};

class Logger final
{

private:

    Logger( ) { }

public:

    static Error close( );

    static Error init( bool in_print_to_file = false ); // C! in_print_to_file adds the log "main", which takes every message

    // C! another log file, run_directory( "logs" ) + in_name + ".log", taking the messages of in_types (every type when empty).
    // C! Each file is made the first time a message is written to it
    static Error add_log( string cref in_name, initializer_list<message_type> in_types = { } );

    static Error log_message( message_type in_type, const char * in_entry, ... ); // C! to the console and every log that takes in_type

    static bool printing_to_console( );
    static Error print_to_console( bool in_print );

    static bool paused( );
    static Error pause( bool in_pause );

    static Error pop_pause( );
    static Error push_pause( bool in_pause );

};

#define Log( ... ) Logger::log_message( __VA_ARGS__ ); macro_end

#else

#define Log( ... ) macro_end

#endif

} // namespace axn

#endif /* Logger_hpp */
