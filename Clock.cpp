#include "Clock.hpp"

#include "AXN.hpp"

#include <ctime>
#include <chrono>
#include <iomanip>

Clock::Clock( ) : m_ms( (uint)system_clock::now( ).time_since_epoch( ).count( ) / 1000.0 ) { }

Clock::Clock( cuint _ms ) : m_ms( _ms ) { }

string Clock::timestamp( ) const
{
    char time_str[ 9 ];
    time_t time = m_ms / 1000.0;
    tm * const ptm = nullptr;
    localtime_s( ptm, &time );
    strftime( time_str, sizeof( time_str ), "%H:%M:%S", ptm );
    return string( time_str );
}

string Clock::datestamp( ) const
{
    char time_str[ 11 ];
    time_t time = m_ms / 1000.0;
    tm * const ptm = nullptr;
    localtime_s( ptm, &time );
    strftime( time_str, sizeof( time_str ), "%m-%d-%Y", ptm );
    return string( time_str );
}
