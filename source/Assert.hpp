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
    bool m_b;

    static void fail( const char * message = nullptr )
    {
        if( strlen( message ) )
            Log( ERROR_LOG, "assert failed! ......... ( %s )", message );
        else
            Log( ERROR_LOG, "assert failed! ........." );

        breakpoint;
        throw runtime_error( message );
    }

public:
    virtual ~Assert( ) { }

    Assert( bool b, const char * message = "", ... ) : m_b( b )
    {
        if( !b )
        {
            va_list va_args;
            va_start( va_args, message );
            Assert::fail( message );
            va_end( va_args );
        }
    }

    Assert( const char * message = "", ... ) : m_b( false )
    {
        va_list va_args;
        va_start( va_args, message );
        Assert::fail( message );
        va_end( va_args );
    }

    operator bool( ) const { return m_b; }
};

} // namespace axn

#else
#define Assert( ... ) true
#endif

#endif /* Assert_hpp */
