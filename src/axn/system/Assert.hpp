#ifndef Assert_hpp
#define Assert_hpp

#include "axn/system/Debug.hpp"
#include "axn/system/Error.hpp"
#include "axn/system/Logger.hpp"

namespace axn
{

#if defined( AXN_DEBUG )

class Assert
{

private:

    static bool active;

public:

    static bool enabled( ) { return( active ); }
    static void enable( bool in_active ) { active = in_active; }

    static void pop_enable( );
    static void push_enable( bool in_active );

    static void init( ) { enable( true ); }

private:

    bool m_passed = true;

    string m_message = nostring;

public:

    operator bool( ) const { return( m_passed ); }

    operator string( ) const { return( m_message ); }

private:

    void pass( const char * in_message = nostring );
    void fail( const char * in_message = nostring );

public:

    Assert( ) { fail( ); }
    Assert( const char * in_message, ... ) { va_list va_args; va_start( va_args, in_message ); fail( in_message ); va_end( va_args ); }

    Assert( bool in_pass ) { if( !in_pass ) { fail( ); } else { pass( ); } }
    Assert( bool in_pass, const char * in_message, ... ) { va_list va_args; va_start( va_args, in_message ); if( !in_pass ) { fail( in_message ); } else { pass( ); } va_end( va_args ); }

};

#pragma warning( disable : 26444 )

#else

#define Assert( ... ) macro_end

#endif

} // namespace axn

#endif /* Assert_hpp */
