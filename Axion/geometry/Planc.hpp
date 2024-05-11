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
    double m_value;

    double value( ) const { return m_value; }
    Planc & value( double d )
    {
        m_value = d;
        return *this;
    }

public:
    Planc( ) : m_value( 0.0 ) { }
    Planc( const double & d ) : m_value( d ) { }

    inline operator bool( ) const { return double_neq( value( ), 0.0 ) && !isnan( value( ) ); }

    inline operator double( ) const { return value( ); }
    inline operator int( ) const { return value( ); }
    inline operator uint( ) const { return abs( value( ) ); }

    inline Planc inverse( ) const { return value( ) ? ( 1.0 / value( ) ) : 0.0; }
    inline Planc & invert( ) { return value( inverse( ) ); }

    inline Planc half( ) const { return value( ) / 2.0; }
    inline Planc & halve( ) { return value( half( ) ); }

    inline Planc & operator=( double d ) { return value( d ); }
    inline Planc operator-( ) const { return Planc( -value( ) ); }

    inline Planc operator+( const Planc & p ) const { return Planc( value( ) + p.value( ) ); }
    inline Planc operator-( const Planc & p ) const { return Planc( value( ) - p.value( ) ); }
    inline Planc operator*( const Planc & p ) const { return Planc( value( ) * p.value( ) ); }
    inline Planc operator/( const Planc & p ) const { return Planc( value( ) / p.value( ) ); }

    inline Planc & operator+=( const Planc & p ) { return *this = value( ) + p.value( ); }
    inline Planc & operator-=( const Planc & p ) { return *this = value( ) - p.value( ); }
    inline Planc & operator*=( const Planc & p ) { return *this = value( ) * p.value( ); }
    inline Planc & operator/=( const Planc & p ) { return *this = value( ) / p.value( ); }

    inline bool operator==( const Planc & p ) const { return double_eq( value( ), p.value( ) ); }
    inline bool operator!=( const Planc & p ) const { return double_neq( value( ), p.value( ) ); }
    inline bool operator>=( const Planc & p ) const { return double_ge( value( ), p.value( ) ); }
    inline bool operator>( const Planc & p ) const { return double_gt( value( ), p.value( ) ); }
    inline bool operator<=( const Planc & p ) const { return double_le( value( ), p.value( ) ); }
    inline bool operator<( const Planc & p ) const { return double_lt( value( ), p.value( ) ); }

    inline Planc operator+( double d ) const { return Planc( value( ) + d ); }
    inline Planc operator-( double d ) const { return Planc( value( ) - d ); }
    inline Planc operator*( double d ) const { return Planc( value( ) * d ); }
    inline Planc operator/( double d ) const { return Planc( value( ) / d ); }

    inline Planc & operator+=( double d ) { return *this = value( ) + d; }
    inline Planc & operator-=( double d ) { return *this = value( ) - d; }
    inline Planc & operator*=( double d ) { return *this = value( ) * d; }
    inline Planc & operator/=( double d ) { return *this = value( ) / d; }

    inline bool operator==( double d ) const { return double_eq( value( ), d ); }
    inline bool operator!=( double d ) const { return double_neq( value( ), d ); }
    inline bool operator>=( double d ) const { return double_ge( value( ), d ); }
    inline bool operator>( double d ) const { return double_gt( value( ), d ); }
    inline bool operator<=( double d ) const { return double_le( value( ), d ); }
    inline bool operator<( double d ) const { return double_lt( value( ), d ); }

    inline Planc operator+( int i ) const { return Planc( value( ) + (double)i ); }
    inline Planc operator-( int i ) const { return Planc( value( ) - (double)i ); }
    inline Planc operator*( int i ) const { return Planc( value( ) * (double)i ); }
    inline Planc operator/( int i ) const { return Planc( value( ) / (double)i ); }

    inline Planc & operator+=( int i ) { return *this = value( ) + (double)i; }
    inline Planc & operator-=( int i ) { return *this = value( ) - (double)i; }
    inline Planc & operator*=( int i ) { return *this = value( ) * (double)i; }
    inline Planc & operator/=( int i ) { return *this = value( ) / (double)i; }

    inline bool operator==( int i ) const { return double_eq( value( ), (double)i ); }
    inline bool operator!=( int i ) const { return double_neq( value( ), (double)i ); }
    inline bool operator>=( int i ) const { return double_ge( value( ), (double)i ); }
    inline bool operator>( int i ) const { return double_gt( value( ), (double)i ); }
    inline bool operator<=( int i ) const { return double_le( value( ), (double)i ); }
    inline bool operator<( int i ) const { return double_lt( value( ), (double)i ); }

    inline Planc operator+( uint ui ) const { return Planc( value( ) + (double)ui ); }
    inline Planc operator-( uint ui ) const { return Planc( value( ) - (double)ui ); }
    inline Planc operator*( uint ui ) const { return Planc( value( ) * (double)ui ); }
    inline Planc operator/( uint ui ) const { return Planc( value( ) / (double)ui ); }

    inline Planc & operator+=( uint ui ) { return *this = value( ) + (double)ui; }
    inline Planc & operator-=( uint ui ) { return *this = value( ) - (double)ui; }
    inline Planc & operator*=( uint ui ) { return *this = value( ) * (double)ui; }
    inline Planc & operator/=( uint ui ) { return *this = value( ) / (double)ui; }

    inline bool operator==( uint ui ) const { return double_eq( value( ), (double)ui ); }
    inline bool operator!=( uint ui ) const { return double_neq( value( ), (double)ui ); }
    inline bool operator>=( uint ui ) const { return double_ge( value( ), (double)ui ); }
    inline bool operator>( uint ui ) const { return double_gt( value( ), (double)ui ); }
    inline bool operator<=( uint ui ) const { return double_le( value( ), (double)ui ); }
    inline bool operator<( uint ui ) const { return double_lt( value( ), (double)ui ); }
};

const Planc P0 = Planc( 0.0 );

inline Planc operator+( double d, const Planc & p ) { return ( p + (double)d ); }
inline Planc operator-( double d, const Planc & p ) { return ( -p + (double)d ); }
inline Planc operator*( double d, const Planc & p ) { return ( p * (double)d ); }
inline Planc operator/( double d, const Planc & p ) { return ( p / (double)d ); }

inline double & operator+=( double & d, const Planc & p ) { return d += (int)p; }
inline double & operator-=( double & d, const Planc & p ) { return d -= (int)p; }
inline double & operator*=( double & d, const Planc & p ) { return d *= (int)p; }
inline double & operator/=( double & d, const Planc & p ) { return d /= (int)p; }

inline bool operator==( double d, const Planc & p ) { return ( p == (double)d ); }
inline bool operator!=( double d, const Planc & p ) { return !( p == (double)d ); }
inline bool operator>=( double d, const Planc & p ) { return ( p <= (double)d ); }
inline bool operator>( double d, const Planc & p ) { return ( p < (double)d ); }
inline bool operator<=( double d, const Planc & p ) { return ( p >= (double)d ); }
inline bool operator<( double d, const Planc & p ) { return ( p > (double)d ); }

inline Planc operator+( int i, const Planc & p ) { return ( p + (double)i ); }
inline Planc operator-( int i, const Planc & p ) { return ( -p + (double)i ); }
inline Planc operator*( int i, const Planc & p ) { return ( p * (double)i ); }
inline Planc operator/( int i, const Planc & p ) { return ( p / (double)i ); }

inline int & operator+=( int & i, const Planc & p ) { return i += (int)p; }
inline int & operator-=( int & i, const Planc & p ) { return i -= (int)p; }
inline int & operator*=( int & i, const Planc & p ) { return i *= (int)p; }
inline int & operator/=( int & i, const Planc & p ) { return i /= (int)p; }

inline bool operator==( int i, const Planc & p ) { return ( p == (double)i ); }
inline bool operator!=( int i, const Planc & p ) { return !( p == (double)i ); }
inline bool operator>=( int i, const Planc & p ) { return ( p <= (double)i ); }
inline bool operator>( int i, const Planc & p ) { return ( p < (double)i ); }
inline bool operator<=( int i, const Planc & p ) { return ( p >= (double)i ); }
inline bool operator<( int i, const Planc & p ) { return ( p > (double)i ); }

inline Planc operator+( uint ui, const Planc & p ) { return ( p + (double)ui ); }
inline Planc operator-( uint ui, const Planc & p ) { return ( -p + (double)ui ); }
inline Planc operator*( uint ui, const Planc & p ) { return ( p * (double)ui ); }
inline Planc operator/( uint ui, const Planc & p ) { return ( p / (double)ui ); }

inline uint & operator+=( uint & ui, const Planc & p ) { return ui += (uint)p; }
inline uint & operator-=( uint & ui, const Planc & p ) { return ui -= (uint)p; }
inline uint & operator*=( uint & ui, const Planc & p ) { return ui *= (uint)p; }
inline uint & operator/=( uint & ui, const Planc & p ) { return ui /= (uint)p; }

inline bool operator==( uint ui, const Planc & p ) { return ( p == (double)ui ); }
inline bool operator!=( uint ui, const Planc & p ) { return !( p == (double)ui ); }
inline bool operator>=( uint ui, const Planc & p ) { return ( p <= (double)ui ); }
inline bool operator>( uint ui, const Planc & p ) { return ( p < (double)ui ); }
inline bool operator<=( uint ui, const Planc & p ) { return ( p >= (double)ui ); }
inline bool operator<( uint ui, const Planc & p ) { return ( p > (double)ui ); }

} // namespace geometry
} // namespace axn

#endif /* Planc_hpp */
