#ifndef Clock_hpp
#define Clock_hpp

#include "axn/system/OS.hpp"
#include "axn/system/STD.hpp"

namespace axn
{

class Clock
{

private:

    uint64 m_ms = 0; // milliseconds since epoch time

    string stamp( string cref in_format ) const; // C! the local time written with strftime( )'s in_format

public:

    Clock( );

    Clock( uint64 in_ms );

    operator uint64( ) const { return( m_ms ); }

    uint64 milliseconds( ) const { return( m_ms ); }
    uint seconds( ) const { return( milliseconds( ) / 1000.0 ); }
    uint minutes( ) const { return( seconds( ) / 60.0 ); }
    uint hours( ) const { return( minutes( ) / 60.0 ); }

    string timestamp( char in_delim = '-' ) const; // C! hours, minutes, and seconds: 13-05-09
    string datestamp( char in_delim = '-' ) const; // C! month, day, and year: 11-19-2026

    Clock ref operator+=( Clock cref in_clock ) { m_ms += in_clock.m_ms; rethis; }
    Clock ref operator-=( Clock cref in_clock ) { m_ms -= in_clock.m_ms; rethis; }

    Clock operator+( Clock cref in_clock ) const { return( Clock( m_ms + in_clock.m_ms ) ); }
    Clock operator-( Clock cref in_clock ) const { return( Clock( m_ms - in_clock.m_ms ) ); }

};

inline uint64 milliseconds( ) { return( Clock( ).milliseconds( ) ); }
inline uint seconds( ) { return( milliseconds( ) / 1000.0 ); }
inline uint minutes( ) { return( seconds( ) / 60.0 ); }
inline uint hours( ) { return( minutes( ) / 60.0 ); }

inline string timestamp( char in_delim = '-' ) { return( Clock( ).timestamp( in_delim ) ); }
inline string datestamp( char in_delim = '-' ) { return( Clock( ).datestamp( in_delim ) ); }

} // namespace axn

#endif /* Clock_hpp */
