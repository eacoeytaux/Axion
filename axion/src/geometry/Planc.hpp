#ifndef Planc_hpp
#define Planc_hpp

#include "axn.hpp"

namespace axn {
namespace geometry {

enum Quadrant {
    No_Quadrant = -1, // origin or axes
    Q1 = 0,
    Q2 = 1,
    Q3 = 2,
    Q4 = 3,
    Quadrants
};

enum Axis {
    No_Axis = -1, // origin or quadrant
    X_Axis = 0,
    Y_Axis = 1,
    Origin = 2,
    Axes
};

class planc {
private:
    float m_num;

public:
    planc( ) : m_num( 0 ) {}
    planc( const float & f ) : m_num( f ) {}
    planc( const ufloat & f ) : m_num( f ) {}

    operator bool( ) const { return !float_eq( m_num, 0.f ); }

    operator int( ) const { return m_num; }
    operator uint( ) const {
        Assert( m_num >= 0 );
        return m_num;
    }
    operator float( ) const { return m_num; }
    operator ufloat( ) const {
        Assert( m_num >= 0 );
        return m_num;
    }

    planc inverse( ) const { return m_num ? ( 1.f / m_num ) : 0.f; }
    planc & invert( ) {
        m_num = inverse( );
        return *this;
    }

    planc half( ) const { return m_num / 2.f; }
    planc & halve( ) {
        m_num = half( );
        return *this;
    }

    planc & operator=( const float & f ) {
        m_num = f;
        return *this;
    }
    planc & operator=( const ufloat & f ) {
        m_num = f;
        return *this;
    }

    planc operator-( ) const { return planc( -m_num ); }

    planc operator+( const planc & p ) const { return planc( m_num + p.m_num ); }
    planc operator-( const planc & p ) const { return planc( m_num - p.m_num ); }
    planc operator*( const planc & p ) const { return planc( m_num * p.m_num ); }
    planc operator/( const planc & p ) const { return planc( m_num / p.m_num ); }

    planc & operator+=( const planc & p ) {
        *this = m_num + p.m_num;
        return *this;
    }
    planc & operator-=( const planc & p ) {
        *this = m_num - p.m_num;
        return *this;
    }
    planc & operator*=( const planc & p ) {
        *this = m_num * p.m_num;
        return *this;
    }
    planc & operator/=( const planc & p ) {
        *this = m_num / p.m_num;
        return *this;
    }

    bool operator==( const planc & p ) const { return float_eq( m_num, p.m_num ); }
    bool operator!=( const planc & p ) const { return !( *this == p ); }
    bool operator>=( const planc & p ) const { return float_ge( m_num, p.m_num ); }
    bool operator>( const planc & p ) const { return float_gt( m_num, p.m_num ); }
    bool operator<=( const planc & p ) const { return float_le( m_num, p.m_num ); }
    bool operator<( const planc & p ) const { return float_lt( m_num, p.m_num ); }

    planc operator+( float f ) const { return planc( m_num + f ); }
    planc operator-( float f ) const { return planc( m_num - f ); }
    planc operator*( float f ) const { return planc( m_num * f ); }
    planc operator/( float f ) const { return planc( m_num / f ); }

    planc & operator+=( float f ) {
        *this = m_num + f;
        return *this;
    }
    planc & operator-=( float f ) {
        *this = m_num - f;
        return *this;
    }
    planc & operator*=( float f ) {
        *this = m_num * f;
        return *this;
    }
    planc & operator/=( float f ) {
        *this = m_num / f;
        return *this;
    }

    bool operator==( float f ) const { return float_eq( m_num, f ); }
    bool operator!=( float f ) const { return !( *this == f ); }
    bool operator>=( float f ) const { return float_ge( m_num, f ); }
    bool operator>( float f ) const { return float_gt( m_num, f ); }
    bool operator<=( float f ) const { return float_le( m_num, f ); }
    bool operator<( float f ) const { return float_lt( m_num, f ); }

    planc operator+( ufloat f ) const { return planc( m_num + f ); }
    planc operator-( ufloat f ) const { return planc( m_num - f ); }
    planc operator*( ufloat f ) const { return planc( m_num * f ); }
    planc operator/( ufloat f ) const { return planc( m_num / f ); }

    planc & operator+=( ufloat f ) {
        *this = m_num + f;
        return *this;
    }
    planc & operator-=( ufloat f ) {
        *this = m_num - f;
        return *this;
    }
    planc & operator*=( ufloat f ) {
        *this = m_num * f;
        return *this;
    }
    planc & operator/=( ufloat f ) {
        *this = m_num / f;
        return *this;
    }

    bool operator==( ufloat f ) const { return float_eq( m_num, f ); }
    bool operator!=( ufloat f ) const { return !( *this == f ); }
    bool operator>=( ufloat f ) const { return float_ge( m_num, f ); }
    bool operator>( ufloat f ) const { return float_gt( m_num, f ); }
    bool operator<=( ufloat f ) const { return float_le( m_num, f ); }
    bool operator<( ufloat f ) const { return float_lt( m_num, f ); }
};

// extern planc operator""_p(long double d ) { return d; }

} // namespace geometry
} // namespace axn

#endif /* Planc_hpp */
