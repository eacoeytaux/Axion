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
    };

    static void init( );

    static bool get( Option opt ) { return settings.contains( opt ); }
    static void set( Option opt, bool b ) { if( b ) { settings.insert( opt ); } else { settings.erase( opt ); } }
    static void flip( Option opt ) { set( opt, !get( opt ) ); }

private:

    static uset<Option> settings;
};

} // namespace axn

#endif /* Settings_hpp */
