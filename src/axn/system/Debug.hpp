#ifndef Debug_hpp
#define Debug_hpp

#include "axn/system/OS.hpp"

namespace axn
{

#define AXN_DEBUG_CONTROLS

// ------------------- //
#if defined( OS_WINDOWS )
// ------------------- //
#if defined( _DEBUG )
#undef AXN_DEBUG // C! the project can define it too (Axion.vcxproj does, as 1), and redefining it differently is a warning
#define AXN_DEBUG
#endif
// ------------------- //
#elif defined( OS_APPLE )
// ------------------- //
#if defined( DEBUG )
#undef AXN_DEBUG // C! the project can define it too (Axion.vcxproj does, as 1), and redefining it differently is a warning
#define AXN_DEBUG
#endif
// ------------------- //
#elif defined( OS_LINUX )
// ------------------- //
#if defined( DEBUG )
#undef AXN_DEBUG // C! the project can define it too (Axion.vcxproj does, as 1), and redefining it differently is a warning
#define AXN_DEBUG
#endif
// ------------------- //
#endif

#if defined( AXN_DEBUG )

#define test_result( type, ... ) const type macro_unique( type ) = __VA_ARGS__; macro_end

#if !defined( AXN_DEBUG_CONTROLS )
#define AXN_DEBUG_CONTROLS // always defined here
#endif

#define AXN_TESTS

#define breakpoint Debug::breakpointf( ); macro_end
#define breakpoint_if( b ) Debug::breakpointiff( b ); macro_end

class Debug final
{

public:

    static bool active;

    static bool bbreakpoint;
    static bool breakpointf( )
    {
        if( bbreakpoint )
        {
            return( true );
        }
        else
        {
            return( true );
        }
    }

    static bool breakpointiff( bool in_break )
    {
        if( in_break )
        {
            return( breakpointf( ) );
        }
        else
        {
            return( false );
        }
    }

};

class Base { }; class Derived : public Base { };

#else

#define breakpoint macro_end
#define breakpoint_if( b ) macro_end

#endif

} // namespace axn

#endif /* Debug_hpp */
