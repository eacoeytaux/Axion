#ifndef Planc_hpp
#define Planc_hpp

#include "Utility.hpp"

namespace axn
{
namespace geometry
{

class Planc
{
private:
    dec m_value = ZERO;

protected:
    dec value( ) const { return m_value; }
    Planc & value( dec value )
    {
        m_value = value;
        return *this;
    }

public:
    virtual ~Planc( ) { }
    Planc( const dec v = ZERO ) { value( v ); }

    operator bool( ) const { return dec_neq( value( ), ZERO ) && !isnan( value( ) ); }

    operator int( ) const { return value( ); }
    operator uint( ) const { return abs( value( ) ); }
    operator dec( ) const { return value( ); }

    Planc & invert( ) { return value( inverse( ) ); }
    Planc inverse( ) const
    {
        Assert( value( ), "cannot divide by zero" );
        return Planc( ONE / value( ) );
    }

    Planc half( ) const { return Planc( value( ) / TWO ); }

    Planc & operator=( dec d ) { return value( d ); }
    Planc & operator=( const Planc & p ) { return value( p.value( ) ); }
    Planc operator-( ) const { return Planc( -value( ) ); }

    Planc operator+( const Planc & p ) const { return Planc( value( ) + p.value( ) ); }
    Planc operator-( const Planc & p ) const { return Planc( value( ) - p.value( ) ); }
    Planc operator*( const Planc & p ) const { return Planc( value( ) * p.value( ) ); }
    Planc operator/( const Planc & p ) const
    {
        Assert( p.value( ), "cannot divide by zero" );
        return Planc( value( ) / p.value( ) );
    }

    Planc & operator+=( const Planc & p ) { return value( value( ) + p.value( ) ); }
    Planc & operator-=( const Planc & p ) { return value( value( ) - p.value( ) ); }
    Planc & operator*=( const Planc & p ) { return value( value( ) * p.value( ) ); }
    Planc & operator/=( const Planc & p )
    {
        Assert( p.value( ), "cannot divide by zero" );
        return value( value( ) / p.value( ) );
    }

    bool operator==( const Planc & p ) const { return dec_eq( value( ), p.value( ) ); }
    bool operator!=( const Planc & p ) const { return dec_neq( value( ), p.value( ) ); }
    bool operator<=( const Planc & p ) const { return dec_le( value( ), p.value( ) ); }
    bool operator>=( const Planc & p ) const { return dec_ge( value( ), p.value( ) ); }
    bool operator<( const Planc & p ) const { return dec_lt( value( ), p.value( ) ); }
    bool operator>( const Planc & p ) const { return dec_gt( value( ), p.value( ) ); }

    Planc operator+( dec d ) const { return Planc( value( ) + d ); }
    Planc operator-( dec d ) const { return Planc( value( ) - d ); }
    Planc operator*( dec d ) const { return Planc( value( ) * d ); }
    Planc operator/( dec d ) const
    {
        Assert( d, "cannot divide by zero" );
        return Planc( value( ) / d );
    }

    Planc & operator+=( dec d ) { return value( value( ) + d ); }
    Planc & operator-=( dec d ) { return value( value( ) - d ); }
    Planc & operator*=( dec d ) { return value( value( ) * d ); }
    Planc & operator/=( dec d )
    {
        Assert( d, "cannot divide by zero" );
        return value( value( ) / d );
    }

    bool operator==( dec d ) const { return dec_eq( value( ), d ); }
    bool operator!=( dec d ) const { return dec_neq( value( ), d ); }
    bool operator>=( dec d ) const { return dec_ge( value( ), d ); }
    bool operator<=( dec d ) const { return dec_le( value( ), d ); }
    bool operator>( dec d ) const { return dec_gt( value( ), d ); }
    bool operator<( dec d ) const { return dec_lt( value( ), d ); }

    Planc operator+( int i ) const { return Planc( value( ) + (dec)i ); }
    Planc operator-( int i ) const { return Planc( value( ) - (dec)i ); }
    Planc operator*( int i ) const { return Planc( value( ) * (dec)i ); }
    Planc operator/( int i ) const
    {
        Assert( i, "cannot divide by zero" );
        return Planc( value( ) / (dec)i );
    }

    Planc & operator+=( int i ) { return value( value( ) + (dec)i ); }
    Planc & operator-=( int i ) { return value( value( ) - (dec)i ); }
    Planc & operator*=( int i ) { return value( value( ) * (dec)i ); }
    Planc & operator/=( int i )
    {
        Assert( i, "cannot divide by zero" );
        return value( value( ) / (dec)i );
    }

    bool operator==( int i ) const { return dec_eq( value( ), (dec)i ); }
    bool operator!=( int i ) const { return dec_neq( value( ), (dec)i ); }
    bool operator>=( int i ) const { return dec_ge( value( ), (dec)i ); }
    bool operator<=( int i ) const { return dec_le( value( ), (dec)i ); }
    bool operator>( int i ) const { return dec_gt( value( ), (dec)i ); }
    bool operator<( int i ) const { return dec_lt( value( ), (dec)i ); }

    Planc operator+( uint u ) const { return Planc( value( ) + (dec)u ); }
    Planc operator-( uint u ) const { return Planc( value( ) - (dec)u ); }
    Planc operator*( uint u ) const { return Planc( value( ) * (dec)u ); }
    Planc operator/( uint u ) const
    {
        Assert( u, "cannot divide by zero" );
        return Planc( value( ) / (dec)u );
    }

    Planc & operator+=( uint u ) { return value( value( ) + (dec)u ); }
    Planc & operator-=( uint u ) { return value( value( ) - (dec)u ); }
    Planc & operator*=( uint u ) { return value( value( ) * (dec)u ); }
    Planc & operator/=( uint u )
    {
        Assert( u, "cannot divide by zero" );
        return value( value( ) / (dec)u );
    }

    bool operator==( uint u ) const { return dec_eq( value( ), (dec)u ); }
    bool operator!=( uint u ) const { return dec_neq( value( ), (dec)u ); }
    bool operator>=( uint u ) const { return dec_ge( value( ), (dec)u ); }
    bool operator<=( uint u ) const { return dec_le( value( ), (dec)u ); }
    bool operator>( uint u ) const { return dec_gt( value( ), (dec)u ); }
    bool operator<( uint u ) const { return dec_lt( value( ), (dec)u ); }
};

inline Planc operator+( dec d, const Planc & p ) { return (Planc)d + p; }
inline Planc operator-( dec d, const Planc & p ) { return (Planc)d - p; }
inline Planc operator*( dec d, const Planc & p ) { return (Planc)d * p; }
inline Planc operator/( dec d, const Planc & p )
{
    Assert( p, "cannot divide by zero" );
    return (Planc)d / p;
}

inline dec & operator+=( dec & d, const Planc & p ) { return d = (Planc)d + p; }
inline dec & operator-=( dec & d, const Planc & p ) { return d = (Planc)d - p; }
inline dec & operator*=( dec & d, const Planc & p ) { return d = (Planc)d * p; }
inline dec & operator/=( dec & d, const Planc & p )
{
    Assert( p, "cannot divide by zero" );
    return d = (Planc)d / p;
}

inline bool operator==( dec d, const Planc & p ) { return ( (Planc)d == p ); }
inline bool operator!=( dec d, const Planc & p ) { return ( (Planc)d != p ); }
inline bool operator<=( dec d, const Planc & p ) { return ( (Planc)d <= p ); }
inline bool operator>=( dec d, const Planc & p ) { return ( (Planc)d >= p ); }
inline bool operator<( dec d, const Planc & p ) { return ( (Planc)d < p ); }
inline bool operator>( dec d, const Planc & p ) { return ( (Planc)d > p ); }

inline Planc operator+( int i, const Planc & p ) { return (Planc)i + p; }
inline Planc operator-( int i, const Planc & p ) { return (Planc)i - p; }
inline Planc operator*( int i, const Planc & p ) { return (Planc)i * p; }
inline Planc operator/( int i, const Planc & p )
{
    Assert( p, "cannot divide by zero" );
    return (Planc)i / p;
}

inline int & operator+=( int & i, const Planc & p ) { return i = (Planc)i + p; }
inline int & operator-=( int & i, const Planc & p ) { return i = (Planc)i - p; }
inline int & operator*=( int & i, const Planc & p ) { return i = (Planc)i * p; }
inline int & operator/=( int & i, const Planc & p )
{
    Assert( p, "cannot divide by zero" );
    return i = (Planc)i / p;
}

inline bool operator==( int i, const Planc & p ) { return ( (Planc)i == p ); }
inline bool operator!=( int i, const Planc & p ) { return ( (Planc)i != p ); }
inline bool operator<=( int i, const Planc & p ) { return ( (Planc)i <= p ); }
inline bool operator>=( int i, const Planc & p ) { return ( (Planc)i >= p ); }
inline bool operator<( int i, const Planc & p ) { return ( (Planc)i < p ); }
inline bool operator>( int i, const Planc & p ) { return ( (Planc)i > p ); }

inline Planc operator+( uint u, const Planc & p ) { return (Planc)u + p; }
inline Planc operator-( uint u, const Planc & p ) { return (Planc)u - p; }
inline Planc operator*( uint u, const Planc & p ) { return (Planc)u * p; }
inline Planc operator/( uint u, const Planc & p )
{
    Assert( p, "cannot divide by zero" );
    return (Planc)u / p;
}

inline uint & operator+=( uint & u, const Planc & p ) { return u = (Planc)u + p; }
inline uint & operator-=( uint & u, const Planc & p ) { return u = (Planc)u - p; }
inline uint & operator*=( uint & u, const Planc & p ) { return u = (Planc)u * p; }
inline uint & operator/=( uint & u, const Planc & p )
{
    Assert( p, "cannot divide by zero" );
    return u = (Planc)u / p;
}

inline bool operator==( uint u, const Planc & p ) { return ( (Planc)u == p ); }
inline bool operator!=( uint u, const Planc & p ) { return ( (Planc)u != p ); }
inline bool operator<=( uint u, const Planc & p ) { return ( (Planc)u <= p ); }
inline bool operator>=( uint u, const Planc & p ) { return ( (Planc)u >= p ); }
inline bool operator<( uint u, const Planc & p ) { return ( (Planc)u < p ); }
inline bool operator>( uint u, const Planc & p ) { return ( (Planc)u > p ); }

const Planc P0 = Planc( ZERO );

} // namespace geometry
} // namespace axn

#endif /* Planc_hpp */
