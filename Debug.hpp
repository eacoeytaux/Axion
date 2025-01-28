#ifndef Debug_hpp
#define Debug_hpp

#include "OS.hpp"
// -------------------- //
#if defined ( OS_WINDOWS )
// -------------------- //
#if defined ( _DEBUG )
#define AXN_DEBUG
#endif
// -------------------- //
#elif defined ( OS_APPLE )
// -------------------- //
#if defined ( DEBUG )
#define AXN_DEBUG
#endif
// -------------------- //
#elif defined ( OS_LINUX )
// -------------------- //
#if defined ( DEBUG )
#define AXN_DEBUG
#endif
// -------------------- //
#endif

#define AXN_DEBUG

#if defined ( AXN_DEBUG )
#define breakpoint Debug::breakpointf( );
namespace axn
{

class Debug
{

public:

    static bool active;
    static bool breakpointf( ) { return true; }

};

} // namespace axn
#else
#define breakpoint
#endif

#endif /* Debug_hpp */
