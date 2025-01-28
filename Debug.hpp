#ifndef Debug_hpp
#define Debug_hpp

#include "OS.hpp"
// -------------------- //
#ifdef OS_WINDOWS
// -------------------- //
#ifdef _DEBUG
#define AXN_DEBUG
#endif
// -------------------- //
#elifdef OS_APPLE
// -------------------- //
#ifdef DEBUG
#define AXN_DEBUG
#endif
// -------------------- //
#elifdef OS_LINUX
// -------------------- //
#ifdef DEBUG
#define AXN_DEBUG
#endif
// -------------------- //
#endif

#define AXN_DEBUG

#ifdef AXN_DEBUG
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
