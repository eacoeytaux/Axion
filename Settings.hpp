#ifndef Settings_hpp
#define Settings_hpp

#include "AXN.hpp"

namespace axn
{

class Settings
{

public:

    enum Option
    {
        GRAPHICS_DETAIL_ALPHA,
        
        #ifdef AXN_DEBUG
        DEBUG_BACKGROUND,
        DEBUG_CAMERA,
        DEBUG_HEALTH,
        DEBUG_PHYSICS,
        DEBUG_GRID,
        #endif
    };

    static void init( );

    static bool get( Option opt ) { return settings.contains( opt ); }
    static void set( Option opt, bool b ) { if( b ) { settings.insert( opt ); } else { settings.remove( opt ); } }
    static void flip( Option opt ) { set( opt, !get( opt ) ); }

private:

    static uset<Option> settings;
    
};

} // namespace axn

#endif /* Settings_hpp */
