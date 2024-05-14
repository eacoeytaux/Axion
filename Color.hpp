#ifndef Color_hpp
#define Color_hpp

#include "STD.hpp"
#include "Assert.hpp"

namespace axn
{
namespace graphics
{

const double FILLED = 0.0;

namespace
{
const uint RGB_DEPTH = 8;
const uint ALPHA_DEPTH = 128;
} // namespace

// https://www.calculatorology.com/conversion/color/

class Color
{
public:
    virtual ~Color( ) { }
    Color( ) : m_r( 0.0 ), m_g( 0.0 ), m_b( 0.0 ), m_a( 1.0 ) { }
    Color( const Color & c ) : m_r( round( c.m_r * RGB_DEPTH ) / RGB_DEPTH ),
                               m_g( round( c.m_g * RGB_DEPTH ) / RGB_DEPTH ),
                               m_b( round( c.m_b * RGB_DEPTH ) / RGB_DEPTH ),
                               m_a( round( c.m_a * ALPHA_DEPTH ) / ALPHA_DEPTH ) { }
    Color( const Color & c, double a ) : m_r( round( c.m_r * RGB_DEPTH ) / RGB_DEPTH ),
                                         m_g( round( c.m_g * RGB_DEPTH ) / RGB_DEPTH ),
                                         m_b( round( c.m_b * RGB_DEPTH ) / RGB_DEPTH ),
                                         m_a( round( a * ALPHA_DEPTH ) / ALPHA_DEPTH ) { }

    static Color rgb( double r, double g, double b ) { return Color::rgba( r, g, b, 1.0 ); }
    static Color rgba( double r, double g, double b, double a )
    {
        Assert( ( r >= 0.0 ) && ( r <= 1.0 ), "r must be [ 0, 1 ]" );
        Assert( ( g >= 0.0 ) && ( g <= 1.0 ), "g must be [ 0, 1 ]" );
        Assert( ( b >= 0.0 ) && ( b <= 1.0 ), "b must be [ 0, 1 ]" );
        Assert( ( a >= 0.0 ) && ( a <= 1.0 ), "a must be [ 0, 1 ]" );

        Color c;
        c.r( r );
        c.g( g );
        c.b( b );
        c.a( a );
        return c;
    }

    static Color rgb( uint rgb ) { return Color::rgba( rgb, 1.0 ); }
    static Color rgba( uint rgb, double a ) { return Color::rgba( (double)( ( rgb & 0xFF0000 ) >> 16 ) / 255.0,
                                                                  (double)( ( rgb & 0x00FF00 ) >> 8 ) / 255.0,
                                                                  (double)( ( rgb & 0x0000FF ) >> 0 ) / 255.0, a ); }

    static Color cmyk( double c, double m, double y, double k ) { return Color::cmyka( c, m, y, k, 1.0 ); }
    static Color cmyka( double c, double m, double y, double k, double a )
    {
        Assert( ( c >= 0.0 ) && ( c <= 1.0 ), "c must be [ 0, 1 ]" );
        Assert( ( m >= 0.0 ) && ( m <= 1.0 ), "m must be [ 0, 1 ]" );
        Assert( ( y >= 0.0 ) && ( y <= 1.0 ), "y must be [ 0, 1 ]" );
        Assert( ( k >= 0.0 ) && ( k <= 1.0 ), "k must be [ 0, 1 ]" );
        Assert( ( a >= 0.0 ) && ( a <= 1.0 ), "a must be [ 0, 1 ]" );

        k = ( 1.0 - k );
        double r = ( 1.0 - c ) * k;
        double g = ( 1.0 - m ) * k;
        double b = ( 1.0 - y ) * k;
        return Color::rgba( r, g, b, a );
    }

    static Color hsv( double h, double s, double v ) { return Color::hsva( h, s, v, 1.0 ); }
    static Color hsva( double h, double s, double v, double a )
    {
        Assert( ( h >= 0.0 ) && ( h <= 1.0 ), "h must be [ 0, 1 ]" );
        Assert( ( s >= 0.0 ) && ( s <= 1.0 ), "s must be [ 0, 1 ]" );
        Assert( ( v >= 0.0 ) && ( v <= 1.0 ), "v must be [ 0, 1 ]" );
        Assert( ( a >= 0.0 ) && ( a <= 1.0 ), "a must be [ 0, 1 ]" );

        double r, g, b;

        double c = v * s;
        double m = v - c;
        double x = c * ( 1.0 - abs( ( h * 6.0 ) - floor( h * 3.0 ) - 1.0 ) );

        if( x <= 1.0 )
        {
            r = m + c;
            g = m + x;
            b = m;
        }
        else if( x <= 2.0 )
        {
            r = m + x;
            g = m + c;
            b = m;
        }
        else if( x <= 3.0 )
        {
            r = m;
            g = m + c;
            b = m + x;
        }
        else if( x <= 4.0 )
        {
            r = m;
            g = m + x;
            b = m + c;
        }
        else if( x <= 5.0 )
        {
            r = m + x;
            g = m;
            b = m + c;
        }
        else // if ( x <= 6.0 )
        {
            r = m + c;
            g = m;
            b = m + x;
        }

        return Color::rgba( r, g, b, a );
    }

    static Color hsl( double h, double s, double l ) { return Color::hsla( h, s, l, 1.0 ); }
    static Color hsla( double h, double s, double l, double a )
    {
        Assert( ( h >= 0.0 ) && ( h <= 1.0 ), "h must be [ 0, 1 ]" );
        Assert( ( s >= 0.0 ) && ( s <= 1.0 ), "s must be [ 0, 1 ]" );
        Assert( ( l >= 0.0 ) && ( l <= 1.0 ), "l must be [ 0, 1 ]" );
        Assert( ( a >= 0.0 ) && ( a <= 1.0 ), "a must be [ 0, 1 ]" );

        double r, g, b;

        double c = s * ( 1.0 - abs( ( l * 2.0 ) - 1.0 ) );
        double m = l - ( c / 2.0 );
        double x = c * ( 1.0 - abs( ( h * 6.0 ) - floor( h * 3.0 ) - 1.0 ) );

        if( x <= 1.0 )
        {
            r = m + c;
            g = m + x;
            b = m;
        }
        else if( x <= 2.0 )
        {
            r = m + x;
            g = m + c;
            b = m;
        }
        else if( x <= 3.0 )
        {
            r = m;
            g = m + c;
            b = m + x;
        }
        else if( x <= 4.0 )
        {
            r = m;
            g = m + x;
            b = m + c;
        }
        else if( x <= 5.0 )
        {
            r = m + x;
            g = m;
            b = m + c;
        }
        else // if ( x <= 6.0 )
        {
            r = m + c;
            g = m;
            b = m + x;
        }

        return Color::rgba( r, g, b, a );
    }

    double r( ) const { return m_r; }
    Color r( double r ) const { return Color::rgba( r, m_g, m_b, m_a ); }
    Color & r( double r )
    {
        m_r = round( r * RGB_DEPTH ) / RGB_DEPTH;
        return *this;
    }

    double g( ) const { return m_g; }
    Color g( double g ) const { return Color::rgba( m_r, g, m_b, m_a ); }
    Color & g( double g )
    {
        m_g = round( g * RGB_DEPTH ) / RGB_DEPTH;
        return *this;
    }

    double b( ) const { return m_b; }
    Color b( double b ) const { return Color::rgba( m_r, m_g, b, m_a ); }
    Color & b( double b )
    {
        m_b = round( b * RGB_DEPTH ) / RGB_DEPTH;
        return *this;
    }

    double a( ) const { return m_a; }
    Color a( double a ) const { return Color::rgba( m_r, m_g, m_b, a ); }
    Color & a( double a )
    {
        m_a = round( a * ALPHA_DEPTH ) / ALPHA_DEPTH;
        return *this;
    }

    bool opaque( ) const { return ( m_a == 1.0 ); }
    bool transparent( ) const { return ( m_a != 1.0 ); }
    bool visible( ) const { return m_a; }

    bool operator==( const Color & c ) const { return ( m_r == c.m_r ) && ( m_g == c.m_g ) && ( m_b == c.m_b ) && ( m_a == c.m_a ); }
    bool operator!=( const Color & c ) const { return !( *this == c ); }

private:
    double m_r = 1.0; // red value
    double m_g = 1.0; // green value
    double m_b = 1.0; // blue value
    double m_a = 1.0; // alpha value
};

const Color TRANSPARENT = Color::rgba( 1.0, 1.0, 1.0, 0.0 );

const Color WHITE = Color::rgb( 1.0, 1.0, 1.0 );
const Color GRAY_LIGHT = Color::rgb( 0.75, 0.75, 0.75 );
const Color GRAY_MID = Color::rgb( 0.5, 0.5, 0.5 );
const Color GRAY_DARK = Color::rgb( 0.25, 0.25, 0.25 );
const Color BLACK = Color::rgb( 0.0, 0.0, 0.0 );

const Color RED = Color::rgb( 1.0, 0.0, 0.0 );
const Color GREEN = Color::rgb( 0.0, 1.0, 0.0 );
const Color BLUE = Color::rgb( 0.0, 0.0, 1.0 );

const Color CYAN = Color::rgb( 0.0, 1.0, 1.0 );
const Color MAGENTA = Color::rgb( 1.0, 0.0, 1.0 );
const Color YELLOW = Color::rgb( 1.0, 1.0, 0.0 );

class ColorSlider
{
public:
    virtual ~ColorSlider( ) { }
    ColorSlider( const Color & c1, const Color & c2 ) : m_color_start( c1 ), m_color_end( c2 ), m_slide( 0.0 ) { }

    ColorSlider & color_start( const Color & c )
    {
        m_color_start = c;
        return *this;
    }
    ColorSlider & color_end( const Color & c )
    {
        m_color_end = c;
        return *this;
    }

    const Color & color_start( ) const { return m_color_start; }
    const Color & color_end( ) const { return m_color_end; }

    double slide( ) const { return m_slide; }
    Color slide( double s )
    {
        m_slide = s;
        Assert( ( m_slide >= 0.0 ) && ( m_slide <= 1.0 ), "slide must be [ 0, 1 ]" );
        return *this;
    }

    Color color_at( double d ) const
    {
        Assert( ( d >= 0.0 ) && ( d <= 1.0 ), "slide must be [ 0, 1 ]" );

        double d_r = m_color_end.r( ) - m_color_start.r( );
        double d_g = m_color_end.g( ) - m_color_start.g( );
        double d_b = m_color_end.b( ) - m_color_start.b( );
        double d_a = m_color_end.a( ) - m_color_start.a( );

        double r = m_color_start.r( ) + ( d_r * d );
        double g = m_color_start.g( ) + ( d_g * d );
        double b = m_color_start.b( ) + ( d_b * d );
        double a = m_color_start.a( ) + ( d_a * d );

        return Color::rgba( r, g, b, a );
    }

    operator Color( ) const
    {
        return color_at( m_slide );
    }

private:
    Color m_color_start;
    Color m_color_end;
    double m_slide;
};

} // namespace graphics
} // namespace axn

#endif /* Color_hpp */
