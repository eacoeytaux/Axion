#include "axn/system/Assert.hpp"

#include "axn/AXN.hpp"

#if defined( AXN_DEBUG )

#include "axn/system/STDc.hpp"

var_static( stack<bool> active_stack );

void Assert::pop_enable( ) { Assert( active_stack.size( ) ); enable( active_stack.pop( ) ); }
void Assert::push_enable( bool in_active ) { active_stack.push( in_active ); enable( in_active ); }

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