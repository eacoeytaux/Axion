#ifndef Debug_hpp
#define Debug_hpp

#include "OS.hpp"
#if defined( OS_WINDOWS )
// ---------------- //
#ifdef _DEBUG
#define AXN_DEBUG
#endif
// ---------------- //
#elif defined( OS_APPLE )
// ---------------- //
#ifdef DEBUG
#define AXN_DEBUG
#endif
// ---------------- //
#elif defined( OS_LINUX
// ---------------- //
// todo
// ---------------- //
#endif

#ifdef AXN_DEBUG
#define breakpoint Debug::breakpoint_function( );
namespace axn
{
class Debug
{
public:
    static bool active;
    inline static bool breakpoint_function( ) { return true; }
};
} // namespace axn
#else
#define breakpoint
#endif

#endif /* Debug_hpp */
