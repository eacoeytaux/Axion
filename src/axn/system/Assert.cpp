#include "axn/system/Assert.hpp"

#include "axn/AXN.hpp"

#if defined( AXN_DEBUG )

#include "axn/system/STDc.hpp"

var_static( stack<bool> active_stack );

void Assert::pop_enable( ) { Assert( active_stack.size( ) ); enable( active_stack.pop( ) ); }
void Assert::push_enable( bool in_active ) { active_stack.push( in_active ); enable( in_active ); }

string Assert::format( const char * in_message, va_list in_args )
{
    va_list args_copy;
    va_copy( args_copy, in_args );
    int length = vsnprintf( nullptr, 0, in_message, args_copy );
    va_end( args_copy );

    return_if( length <= 0, string( in_message ) );

    string message( size_t( length ), '\0' );
    vsnprintf( message.data( ), size_t( length ) + 1, in_message, in_args );

    return( message );
}

void Assert::pass( const char * in_message )
{
    m_passed = true;
    m_message = string( in_message );
}

void Assert::fail( const char * in_message )
{
    m_passed = false;
    m_message = string( in_message );

    if( active )
    {
        if( strlen( in_message ) )
        {
            Log( ERROR_LOG, "assertion failed! ...... ( %s )", in_message );
        }
        else
        {
            Log( ERROR_LOG, "assertion failed! ...... " );
        }

        breakpoint;

        // throw runtime_error( message );
    }
}

#endif