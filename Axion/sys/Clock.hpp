#ifndef Clock_hpp
#define Clock_hpp

#include "STD.hpp"

namespace axn
{

class Clock
{
public:
    Clock( );
    Clock( uint ms );

    uint total_ms( ) const;

    uint seconds( ) const;
    uint minutes( ) const;
    uint hours( ) const;

    string timestamp( string delim = ":" ) const;
    string datestamp( string delim = "-" ) const;

    Clock operator+( const Clock & c ) const;
    Clock operator-( const Clock & c ) const;

    Clock & operator+=( const Clock & c );
    Clock & operator-=( const Clock & c );

private:
    uint m_ms; // milliseconds since epoch time
};

} // namespace axn

#endif /* Clock_hpp */
