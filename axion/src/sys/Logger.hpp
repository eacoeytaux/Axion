#ifndef Logger_hpp
#define Logger_hpp

#include "Error.hpp"

namespace axn {
namespace system {

enum MESSAGE_TYPE {
    INFO_LOG = 0,
    WARNING_LOG,
    ERROR_LOG,
    DEBUG_LOG,
};

class Logger {
private:
    Logger( ) {}

public:
    static err init( bool print_to_file = false );
    static err close( );
    static err log( MESSAGE_TYPE type, const char* entry, ... );
    static err print_to_console( bool );
    static bool printing_to_console( );
    static err pause( bool );
    static bool paused( );
};

} // namespace system
} // namespace axn

#endif /* Logger_hpp */
