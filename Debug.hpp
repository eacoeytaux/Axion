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

#if defined ( AXN_DEBUG )
#define breakpoint Debug::breakpointf( );
namespace axn
{

class Debug
{

public:

    static bool active;
    static bool breakpointf( ) { return true; }

    static bool shifty; // if shift key is held
};

} // namespace axn
#else
#define breakpoint
#endif

#endif /* Debug_hpp */
