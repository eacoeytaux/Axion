#ifndef Logger_hpp
#define Logger_hpp

#include "axn/system/Debug.hpp"
#include "axn/system/Error.hpp"

#if defined( AXN_DEBUG )

namespace axn
{

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

    static error close( );

    static error init( bool in_print_to_file = false );

    static error log_message( message_type in_type, const char * in_entry, ... );

    static error print_to_console( bool in_print );
    static bool printing_to_console( );

    static error pause( bool in_pause );
    static bool paused( );

};

#define Log( ... ) Logger::log_message( __VA_ARGS__ )

} // namespace axn

#else

#define Log( ... )

#endif

#endif /* Logger_hpp */
