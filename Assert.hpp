#ifndef Assert_hpp
#define Assert_hpp

#include "Debug.hpp"
#include "Error.hpp"
#include "Logger.hpp"

#if defined ( AXN_DEBUG )

namespace axn
{

class Assert
{

private:

    bool m_passed = true;
    string m_message = "";

public:

    operator bool( ) const { return m_passed; }
    operator string( ) const { return m_message; }

private:

    void fail( const char * message = "" )
    {
        m_passed = false;
        m_message = string( message );

        if( strlen( message ) )
        {
            Log( ERROR_LOG, "assert failed! ......... ( %s )", message );
        }
        else
        {
            Log( ERROR_LOG, "assert failed! ........." );
        }

        breakpoint;

        // throw runtime_error( message );
    }

public:

    Assert( ) { fail( ); }
    Assert( const char * message, ... ) { va_list va_args; va_start( va_args, message ); fail( message ); va_end( va_args ); }

    Assert( bool b ) { if( !b ) { fail( ); } }
    Assert( bool b, const char * message, ... ) { if( !b ) { va_list va_args; va_start( va_args, message ); fail( message ); va_end( va_args ); } }

};

} // namespace axn

#else
#define Assert( ... ) true
#endif

#endif /* Assert_hpp */
