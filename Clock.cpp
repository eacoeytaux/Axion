#include "Clock.hpp"

#include "AXN.hpp"

#include <ctime>
#include <chrono>
#include <iomanip>

Clock::Clock( ) : m_ms( (uint)system_clock::now( ).time_since_epoch( ).count( ) / 1000.0 ) { }

Clock::Clock( cuint _ms ) : m_ms( _ms ) { }

string Clock::timestamp( const char _delim ) const
{
    time_t time = m_ms / 1000.0;
    #if defined( OS_WINDOWS )
    tm * const ptm = nullptr;
    localtime_s( ptm, &time );
    #elif defined( OS_APPLE )
    tm * ptm = nullptr;
    ptm = localtime( &time );
    #elif defined( OS_LINUX )
    tm * ptm = nullptr;
    ptm = localtime( &time );
    #endif
    // char time_str[ 9 ];
    // strftime( time_str, sizeof( time_str ), ( "%H" + _delim + "%M" + _delim + "%S" ).c_str( ), ptm );
    // return string( time_str );
    return "";
}

string Clock::datestamp( const char _delim ) const
{
    time_t time = m_ms / 1000.0;
    #if defined( OS_WINDOWS )
    tm * const ptm = nullptr;
    localtime_s( ptm, &time );
    #elif defined( OS_APPLE )
    tm * ptm = nullptr;
    ptm = localtime( &time );
    #elif defined( OS_LINUX )
    tm * ptm = nullptr;
    ptm = localtime( &time );
    #endif
    // char time_str[ 11 ];
    // strftime( time_str, sizeof( time_str ), ( "%m" + _delim + "%d" + _delim + "%Y" ).c_str( ), ptm );
    // return string( time_str );
    return "";
}
