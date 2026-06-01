#ifndef Assert_hpp
#define Assert_hpp

#include "axn/system/Debug.hpp"
#include "axn/system/Error.hpp"
#include "axn/system/Logger.hpp"

#if defined( AXN_DEBUG )

namespace axn
{

class Assert
{

public:

    static void init( ) { enable( true ); }

    static void enable( bool in_active ) { active = in_active; }
    static bool enabled( ) { return( active ); }

private:

    static bool active;

    bool m_passed = true;

    string m_message;

public:

    operator bool( ) const { return( m_passed ); }

    operator string( ) const { return( m_message ); }

private:

    void fail( const char * in_message = "" )
    {
        m_passed = false;

        m_message = string( in_message );

        if( active )
        {
            if( strlen( in_message ) )
            {
                Log( ERROR_LOG, "assert failed! ......... ( %s )", in_message );
            }
            else
            {
                Log( ERROR_LOG, "assert failed! ........." );
            }

            breakpoint;

            // throw runtime_error( message );
        }
    }

public:

    Assert( ) { fail( ); }
    Assert( const char * in_message, ... ) { va_list va_args; va_start( va_args, in_message ); fail( in_message ); va_end( va_args ); }

    Assert( bool in_pass ) { if( !in_pass ) { fail( ); } }
    Assert( bool in_pass, const char * in_message, ... ) { if( !in_pass ) { va_list va_args; va_start( va_args, in_message ); fail( in_message ); va_end( va_args ); } }

};

#pragma warning( disable : 26444 )

} // namespace axn

#else

#define Assert( ... )

#endif

#endif /* Assert_hpp */
