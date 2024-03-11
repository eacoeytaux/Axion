#ifndef Assert_hpp
#define Assert_hpp

#include "Includes.hpp"
#include "Error.hpp"
#include "Logger.hpp"

namespace axn {
namespace system {

class Assert {
private:
    bool m_b;

    static bool b_test_mode;
    static void fail( const char* message = nullptr ) {
        if( strlen( message ) )
            Logger::log( ERROR_LOG, "assert failed! ......... ( %s )", message );
        else
            Logger::log( ERROR_LOG, "assert failed! ........." );

        if( !b_test_mode ) {
            static bool breakpoint = true;
            if( breakpoint ) {
                // insert breakpoint
            }
        }

        throw runtime_error( message );
    }

public:
    static void set_test_mode( bool b ) { b_test_mode = b; }

    Assert( bool b, const char* message = "", ... ) : m_b( b ) {
        if( !b ) {
            va_list va_args;
            va_start( va_args, message );
            fail( message );
            va_end( va_args );
        }
    }

    operator bool( ) const { return m_b; }
};

} // namespace system
} // namespace axn

#endif /* Assert_hpp */
