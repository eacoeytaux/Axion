#include "axn/system/Clock.hpp"

#include "axn/AXN.hpp"

#include <ctime>
#include <chrono>
#include <iomanip>

Clock::Clock( ) : m_ms( uint( ( system_clock::now( ).time_since_epoch( ).count( ) ) / 1000.0 ) ) { }

Clock::Clock( uint in_ms ) : m_ms( in_ms ) { }

string Clock::timestamp( ) const
{
    char time_str[ 9 ];
    tm * ptm = nullptr;
    time_t time = m_ms / 1000.0;

    #if defined( OS_WINDOWS )
    localtime_s( ptm, &time );
    #elif defined( OS_APPLE )
    ptm = localtime( &time );
    #elif defined( OS_LINUX )
    // todo
    #endif

    strftime( time_str, sizeof( time_str ), "%H:%M:%S", ptm );
    return( string( time_str ) );
}

string Clock::datestamp( ) const
{
    char time_str[ 11 ];
    tm * ptm = nullptr;
    time_t time = m_ms / 1000.0;

    #if defined( OS_WINDOWS )
    localtime_s( ptm, &time );
    #elif defined( OS_APPLE )
    ptm = localtime( &time );
    #elif defined( OS_LINUX )
    // todo
    #endif

    strftime( time_str, sizeof( time_str ), "%m-%d-%Y", ptm );
    return( string( time_str ) );
}
