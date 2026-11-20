#include "axn/system/Clock.hpp"

#include "axn/AXN.hpp"

#include <ctime>
#include <chrono>
#include <iomanip>

string Clock::stamp( string cref in_format ) const
{
    char time_str[ 64 ];
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

    return_if( is_zero( strftime( time_str, sizeof( time_str ), charify( in_format ), ptm ) ), nostring ); // C! nothing written: the format didn't fit
    return( string( time_str ) );
}

Clock::Clock( ) : m_ms( uint64( std::chrono::duration_cast<std::chrono::milliseconds>( system_clock::now( ).time_since_epoch( ) ).count( ) ) ) { }

Clock::Clock( uint64 in_ms ) : m_ms( in_ms ) { }

string Clock::timestamp( char in_delim ) const { return( stamp( "%H" + string( 1, in_delim ) + "%M" + string( 1, in_delim ) + "%S" ) ); }

string Clock::datestamp( char in_delim ) const { return( stamp( "%m" + string( 1, in_delim ) + "%d" + string( 1, in_delim ) + "%Y" ) ); }
