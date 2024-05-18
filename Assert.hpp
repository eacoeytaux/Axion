#ifndef Assert_hpp
#define Assert_hpp

#include "Debug.hpp"
#include "Error.hpp"
#include "Logger.hpp"

#ifdef AXN_DEBUG

namespace axn
{

class Assert
{
private:
    bool m_passed;
    string m_message;

    void fail( const char * message )
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

        throw runtime_error( message );
    }

public:
    virtual ~Assert( ) { }

    Assert( const char * message = "", ... ) : m_passed( false )
    {
        va_list va_args;
        va_start( va_args, message );
        Assert::fail( message );
        va_end( va_args );
    }

    Assert( bool b, const char * message = "", ... ) : m_passed( b )
    {
        if( !b )
        {
            va_list va_args;
            va_start( va_args, message );
            Assert::fail( message );
            va_end( va_args );
        }
    }

    operator bool( ) const { return m_passed; }
    operator string( ) const { return m_message; }
};

} // namespace axn

#else
#define Assert( ... ) true
#endif

#endif /* Assert_hpp */
