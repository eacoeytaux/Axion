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

class Logger
{

private:

    Logger( ) { }

public:

    static Error close( );

    static Error init( bool in_print_to_file = false );

    static Error log_message( message_type in_type, const char * in_entry, ... );

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
