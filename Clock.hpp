#ifndef Clock_hpp
#define Clock_hpp

#include "STD.hpp"

namespace axn
{

class Clock
{

private:

    uint m_ms; // milliseconds since epoch time

public:

    Clock( );

    Clock( uint ms );

    operator uint( ) const { return m_ms; }

    uint milliseconds( ) const { return m_ms; }
    uint seconds( ) const { return milliseconds( ) / 1000.0; }
    uint minutes( ) const { return seconds( ) / 60.0; }
    uint hours( ) const { return minutes( ) / 60.0; }

    string timestamp( ) const;
    string datestamp( ) const;

    Clock operator+( Clock c ) const { return Clock( m_ms + c.m_ms ); }
    Clock operator-( Clock c ) const { return Clock( m_ms - c.m_ms ); }

    Clock & operator+=( Clock c ) { m_ms += c.m_ms; return *this; }
    Clock & operator-=( Clock c ) { m_ms -= c.m_ms; return *this; }
    
};

inline uint milliseconds( ) { return Clock( ).milliseconds( ); }
inline uint seconds( ) { return milliseconds( ) / 1000.0; }
inline uint minutes( ) { return seconds( ) / 60.0; }
inline uint hours( ) { return minutes( ) / 60.0; }

inline string timestamp( ) { return Clock( ).timestamp( ); }
inline string datestamp( ) { return Clock( ).datestamp( ); }

} // namespace axn

#endif /* Clock_hpp */
