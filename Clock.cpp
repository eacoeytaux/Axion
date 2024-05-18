#include "Clock.hpp"

#include <ctime>
#include <chrono>
#include <iomanip>

using namespace axn;
using std::chrono::system_clock;

Clock::Clock( ) : m_ms( (uint)system_clock::now( ).time_since_epoch( ).count( ) / 1000.0 ) { }

Clock::Clock( uint _ms ) : m_ms( _ms ) { }

string Clock::timestamp( const string _delim ) const
{
    char time_str[ 9 ];
    time_t time = m_ms / 1000.0;
    const tm * ptm = nullptr;
#if defined( OS_WINDOWS )
    // ---------------- //
    localtime_s( &ptm, &time );
// ---------------- //
#elif defined( OS_APPLE )
    // ---------------- //
    ptm = localtime( &time );
// ---------------- //
#elif defined( OS_LINUX )
// ---------------- //
// todo
// ---------------- //
#endif
    strftime( time_str, sizeof( time_str ), ( "%H" + _delim + "%M" + _delim + "%S" ).c_str( ), ptm );
    return string( time_str );
}

string Clock::datestamp( const string _delim ) const
{
    char time_str[ 11 ];
    time_t time = m_ms / 1000.0;
    const tm * ptm = nullptr;
#if defined( OS_WINDOWS )
    // ---------------- //
    localtime_s( &ptm, &time );
// ---------------- //
#elif defined( OS_APPLE )
    // ---------------- //
    ptm = localtime( &time );
// ---------------- //
#elif defined( OS_LINUX )
// ---------------- //
// todo
// ---------------- //
#endif
    strftime( time_str, sizeof( time_str ), ( "%m" + _delim + "%d" + _delim + "%Y" ).c_str( ), ptm );
    return string( time_str );
}

uint Clock::total_ms( ) const { return m_ms; }
uint Clock::seconds( ) const { return m_ms / 1000.0; }
uint Clock::minutes( ) const { return m_ms / ( 1000.0 * 60.0 ); }
uint Clock::hours( ) const { return m_ms / ( 1000.0 * 60.0 * 60.0 ); }

Clock Clock::operator+( const Clock & _c ) const { return Clock( m_ms + _c.m_ms ); }
Clock Clock::operator-( const Clock & _c ) const { return Clock( m_ms - _c.m_ms ); }

Clock & Clock::operator+=( const Clock & _c )
{
    m_ms += _c.m_ms;
    return *this;
}

Clock & Clock::operator-=( const Clock & _c )
{
    m_ms -= _c.m_ms;
    return *this;
}
