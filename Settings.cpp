#include "Settings.hpp"

uset<Settings::Option> Settings::settings = uset<Settings::Option>( );

void Settings::init( )
{
    set( GRAPHICS_DETAIL_ALPHA, true );
}
