#include "axn/system/Clock.hpp"

#include "axn/AXN.hpp"

#include <ctime>
#include <chrono>
#include <iomanip>

Clock::Clock( ) : m_ms( uint64( std::chrono::duration_cast<std::chrono::milliseconds>( system_clock::now( ).time_since_epoch( ) ).count( ) ) ) { }

Clock::Clock( uint64 in_ms ) : m_ms( in_ms ) { }

string Clock::timestamp( ) const
{
    char time_str[ 9 ];
    tm tm_buf = { };
    tm * ptm = &tm_buf;
    time_t time = time_t( m_ms / 1000 );

    #if defined( OS_WINDOWS )
    localtime_s( ptm, &time );
    #elif defined( OS_APPLE )
    ptm = localtime( &time );
    #elif defined( OS_LINUX )
    localtime_r( &time, ptm );
    #endif

    strftime( time_str, sizeof( time_str ), "%H:%M:%S", ptm );
    return( string( time_str ) );
}

string Clock::datestamp( ) const
{
    char time_str[ 11 ];
    tm tm_buf = { };
    tm * ptm = &tm_buf;
    time_t time = time_t( m_ms / 1000 );

    #if defined( OS_WINDOWS )
    localtime_s( ptm, &time );
    #elif defined( OS_APPLE )
    ptm = localtime( &time );
    #elif defined( OS_LINUX )
    localtime_r( &time, ptm );
    #endif

    strftime( time_str, sizeof( time_str ), "%m-%d-%Y", ptm );
    return( string( time_str ) );
}
