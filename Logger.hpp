#ifndef Logger_hpp
#define Logger_hpp

#include "Debug.hpp"
#include "Error.hpp"

#if defined ( AXN_DEBUG )

namespace axn
{

enum MessageType
{
    INFO_LOG = 0,
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
    static error init( bool print_to_file = false );
    static error print_to_console( bool );
    static bool printing_to_console( );
    static error pause( bool );
    static bool paused( );

    static error log_message( MessageType type, const char * entry, ... );

};

#define Log( ... ) Logger::log_message( __VA_ARGS__ )

} // namespace axn

#else
#define Log( ... )
#endif

#endif /* Logger_hpp */
