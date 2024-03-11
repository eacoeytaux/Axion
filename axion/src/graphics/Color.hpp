#ifndef Color_hpp
#define Color_hpp

#include "axn.hpp"

namespace axn {
namespace graphics {

const ufloat FILLED = 0.f;

class Color {
public:
    virtual ~Color( ) {}
    Color( ) : m_r( 1.f ), m_g( 1.f ), m_b( 1.f ), m_a( 1.f ) {}

private:
    Color( float r, float g, float b, float a ) : m_r( r ), m_g( g ), m_b( b ), m_a( a ) {}

public:
    Color( const Color & c ) : m_r( c.m_r ), m_g( c.m_g ), m_b( c.m_b ), m_a( c.m_a ) {}

    static Color rgb( float r, float g, float b, float a = 1.0 ) { return Color( r, g, b, a ); }
    static Color rgb( uint rgb, float a = 1.0 ) {
        return Color( (float)( ( rgb & 0xFF0000 ) >> 16 ) / 255.f,
                      (float)( ( rgb & 0x00FF00 ) >> 8 ) / 255.f,
                      (float)( rgb & 0x0000FF ) / 255.f, a );
    }

    static Color hsv( float h, float s, float v, float a = 1.0 ) { return Color( ); } // TODO

    float r( ) const { return m_r; }
    Color r( float r ) const { return Color( r, m_g, m_b, m_a ); }
    Color & r( float r ) {
        m_r = r;
        return *this;
    }

    float g( ) const { return m_g; }
    Color g( float g ) const { return Color( m_r, g, m_b, m_a ); }
    Color & g( float g ) {
        m_g = g;
        return *this;
    }

    float b( ) const { return m_b; }
    Color b( float b ) const { return Color( m_r, m_g, b, m_a ); }
    Color & b( float b ) {
        m_b = b;
        return *this;
    }

    float a( ) const { return m_a; }
    Color a( float a ) const { return Color( m_r, m_g, m_b, a ); }
    Color & a( float a ) {
        m_a = a;
        return *this;
    }

    // Color transparent() const;
    // Color opaue() const;
    // Color inverse() const;

    bool operator==( const Color & c ) const {
        return ( m_r == c.m_r ) && ( m_g == c.m_g ) && ( m_b == c.m_b ) && ( m_a == c.m_a );
    }
    bool operator!=( const Color & c ) const { return !( *this == c ); }

private:
    float m_r = 1.f; // red value
    float m_g = 1.f; // green value
    float m_b = 1.f; // blue value
    float m_a = 1.f; // alpha value
};

const Color WHITE = Color::rgb( 1.f, 1.f, 1.f );
const Color GRAY_LIGHT = Color::rgb( 0.75f, 0.75f, 0.75f );
const Color GRAY_MID = Color::rgb( 0.5f, 0.5f, 0.5f );
const Color GRAY_DARK = Color::rgb( 0.25f, 0.25f, 0.25f, 0.25f );
const Color BLACK = Color::rgb( 0.f, 0.f, 0.f );

const Color RED = Color::rgb( 1.f, 0.f, 0.f );
const Color GREEN = Color::rgb( 0.f, 1.f, 0.f );
const Color BLUE = Color::rgb( 0.f, 0.f, 1.f );

const Color CYAN = Color::rgb( 0.f, 1.f, 1.f );
const Color MAGENTA = Color::rgb( 1.f, 0.f, 1.f );
const Color YELLOW = Color::rgb( 1.f, 1.f, 0.f );

} // namespace graphics
} // namespace axn

#endif /* Color_hpp */
