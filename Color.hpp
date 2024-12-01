#ifndef Color_hpp
#define Color_hpp

#include "STD.hpp"
#include "Assert.hpp"
#include "Utility.hpp"

namespace axn
{
namespace graphics
{

const dec FILLED = ZERO;

namespace
{
const uint RGB_DEPTH = 255;
const uint ALPHA_DEPTH = 255;
} // namespace

// https://www.calculatorology.com/conversion/color/

class Color
{
public:
    virtual ~Color( ) { }

    Color( ) : m_r( ZERO ), m_g( ZERO ), m_b( ZERO ), m_a( ONE ) { }

    Color( const Color & c ) : m_r( round( c.m_r * (dec)RGB_DEPTH ) / (dec)RGB_DEPTH ),
        m_g( round( c.m_g * (dec)RGB_DEPTH ) / (dec)RGB_DEPTH ),
        m_b( round( c.m_b * (dec)RGB_DEPTH ) / (dec)RGB_DEPTH ),
        m_a( round( c.m_a * (dec)ALPHA_DEPTH ) / (dec)ALPHA_DEPTH )
    {
    }

    Color( const Color & c, dec a ) : m_r( round( c.m_r * (dec)RGB_DEPTH ) / (dec)RGB_DEPTH ),
        m_g( round( c.m_g * (dec)RGB_DEPTH ) / (dec)RGB_DEPTH ),
        m_b( round( c.m_b * (dec)RGB_DEPTH ) / (dec)RGB_DEPTH ),
        m_a( round( a * (dec)ALPHA_DEPTH ) / (dec)ALPHA_DEPTH )
    {
    }

    static Color rgb( dec r, dec g, dec b ) { return Color::rgba( r, g, b, ONE ); }
    static Color rgba( dec r, dec g, dec b, dec a )
    {
        Assert( ( r >= ZERO ) && ( r <= ONE ), "red must be [ 0, 1 ]" );
        Assert( ( g >= ZERO ) && ( g <= ONE ), "green must be [ 0, 1 ]" );
        Assert( ( b >= ZERO ) && ( b <= ONE ), "blue must be [ 0, 1 ]" );
        Assert( ( a >= ZERO ) && ( a <= ONE ), "alpha must be [ 0, 1 ]" );

        Color c;

        c.m_r = r;
        c.m_g = g;
        c.m_b = b;
        c.m_a = a;

        //c.m_r = ( round( c.m_r * (dec)RGB_DEPTH ) / (dec)RGB_DEPTH );
        //c.m_g = ( round( c.m_g * (dec)RGB_DEPTH ) / (dec)RGB_DEPTH );
        //c.m_b = ( round( c.m_b * (dec)RGB_DEPTH ) / (dec)RGB_DEPTH );
        //c.m_a = ( round( c.m_a * (dec)ALPHA_DEPTH ) / (dec)ALPHA_DEPTH );

        return c;
    }

    static Color rgb( uint rgb ) { return Color::rgba( rgb, ONE ); }
    static Color rgba( uint rgb, dec a )
    {
        return Color::rgba( (dec)( ( rgb & 0xFF0000 ) >> 16 ) / (dec)255,
                            (dec)( ( rgb & 0x00FF00 ) >> 8 ) / (dec)255,
                            (dec)( ( rgb & 0x0000FF ) >> 0 ) / (dec)255, a );
    }

    static Color cmyk( dec c, dec m, dec y, dec k ) { return Color::cmyka( c, m, y, k, ONE ); }
    static Color cmyka( dec c, dec m, dec y, dec k, dec a )
    {
        Assert( ( c >= ZERO ) && ( c <= ONE ), "cyan must be [ 0, 1 ]" );
        Assert( ( m >= ZERO ) && ( m <= ONE ), "magenta must be [ 0, 1 ]" );
        Assert( ( y >= ZERO ) && ( y <= ONE ), "yellow must be [ 0, 1 ]" );
        Assert( ( k >= ZERO ) && ( k <= ONE ), "black must be [ 0, 1 ]" );
        Assert( ( a >= ZERO ) && ( a <= ONE ), "alpha must be [ 0, 1 ]" );

        k = ( ONE - k );
        dec r = ( ONE - c ) * k;
        dec g = ( ONE - m ) * k;
        dec b = ( ONE - y ) * k;
        return Color::rgba( r, g, b, a );
    }

    static Color hsv( dec h, dec s, dec v ) { return Color::hsva( h, s, v, ONE ); }
    static Color hsva( dec h, dec s, dec v, dec a )
    {
        Assert( ( h >= ZERO ) && ( h <= ONE ), "hue must be [ 0, 1 ]" );
        Assert( ( s >= ZERO ) && ( s <= ONE ), "saturation must be [ 0, 1 ]" );
        Assert( ( v >= ZERO ) && ( v <= ONE ), "value must be [ 0, 1 ]" );
        Assert( ( a >= ZERO ) && ( a <= ONE ), "alpha must be [ 0, 1 ]" );

        dec r, g, b;

        dec c = v * s;
        dec m = v - c;
        dec x = c * ( ONE - abs( ( h * SIX ) - floor( h * THREE ) - ONE ) );

        if( x <= ONE )
        {
            r = m + c;
            g = m + x;
            b = m;
        }
        else if( x <= TWO )
        {
            r = m + x;
            g = m + c;
            b = m;
        }
        else if( x <= THREE )
        {
            r = m;
            g = m + c;
            b = m + x;
        }
        else if( x <= FOUR )
        {
            r = m;
            g = m + x;
            b = m + c;
        }
        else if( x <= FIVE )
        {
            r = m + x;
            g = m;
            b = m + c;
        }
        else // if( x <= SIX )
        {
            r = m + c;
            g = m;
            b = m + x;
        }

        return Color::rgba( r, g, b, a );
    }

    static Color hsl( dec h, dec s, dec l ) { return Color::hsla( h, s, l, ONE ); }
    static Color hsla( dec h, dec s, dec l, dec a )
    {
        Assert( ( h >= ZERO ) && ( h <= ONE ), "hue must be [ 0, 1 ]" );
        Assert( ( s >= ZERO ) && ( s <= ONE ), "saturation must be [ 0, 1 ]" );
        Assert( ( l >= ZERO ) && ( l <= ONE ), "lightness must be [ 0, 1 ]" );
        Assert( ( a >= ZERO ) && ( a <= ONE ), "alpha must be [ 0, 1 ]" );

        dec r, g, b;

        dec c = s * ( ONE - abs( ( l * TWO ) - ONE ) );
        dec m = l - ( c / TWO );
        dec x = c * ( ONE - abs( ( h * SIX ) - floor( h * THREE ) - ONE ) );

        if( x <= ONE )
        {
            r = m + c;
            g = m + x;
            b = m;
        }
        else if( x <= TWO )
        {
            r = m + x;
            g = m + c;
            b = m;
        }
        else if( x <= THREE )
        {
            r = m;
            g = m + c;
            b = m + x;
        }
        else if( x <= FOUR )
        {
            r = m;
            g = m + x;
            b = m + c;
        }
        else if( x <= FIVE )
        {
            r = m + x;
            g = m;
            b = m + c;
        }
        else // if( x <= SIX )
        {
            r = m + c;
            g = m;
            b = m + x;
        }

        return Color::rgba( r, g, b, a );
    }

    dec r( ) const { return m_r; }
    Color r( dec r ) const { return Color::rgba( r, g( ), b( ), a( ) ); }
    Color & r( dec r ) { return *this = Color::rgba( r, g( ), b( ), a( ) ); }

    dec g( ) const { return m_g; }
    Color g( dec g ) const { return Color::rgba( r( ), g, b( ), a( ) ); }
    Color & g( dec g ) { return *this = Color::rgba( r( ), g, b( ), a( ) ); }

    dec b( ) const { return m_b; }
    Color b( dec b ) const { return Color::rgba( r( ), g( ), b, a( ) ); }
    Color & b( dec b ) { return *this = Color::rgba( r( ), g( ), b, a( ) ); }

    dec c( ) const { return ONE - max_rgb( ); }
    Color c( dec c ) const { return Color::cmyka( c, m( ), y( ), k( ), a( ) ); }
    Color & c( dec c ) { return *this = Color::cmyka( c, y( ), m( ), k( ), a( ) ); }

    dec m( ) const { return ( max_rgb( ) - r( ) ) / max_rgb( ); }
    Color m( dec m ) const { return Color::cmyka( c( ), m, y( ), k( ), a( ) ); }
    Color & m( dec m ) { return *this = Color::cmyka( c( ), m, y( ), k( ), a( ) ); }

    dec y( ) const { return ( max_rgb( ) - g( ) ) / max_rgb( ); }
    Color y( dec y ) const { return Color::cmyka( c( ), m( ), y, k( ), a( ) ); }
    Color & y( dec y ) { return *this = Color::cmyka( c( ), m( ), y, k( ), a( ) ); }

    dec k( ) const { return ( max_rgb( ) - b( ) ) / max_rgb( ); }
    Color k( dec k ) const { return Color::cmyka( c( ), m( ), y( ), k, a( ) ); }
    Color & k( dec k ) { return *this = Color::cmyka( c( ), m( ), y( ), k, a( ) ); }

    dec h( ) const
    {
        if( max_rgb( ) != min_rgb( ) )
        {
            dec d_rgb = max_rgb( ) - min_rgb( );
            if( max_rgb( ) == r( ) )
            {
                return ( ( g( ) - b( ) ) / d_rgb ) / SIX;
            }
            else if( max_rgb( ) == g( ) )
            {
                return ( TWO + ( ( b( ) - r( ) ) / d_rgb ) ) / SIX;
            }
            else if( max_rgb( ) == b( ) )
            {
                return ( FOUR + ( ( r( ) - g( ) ) / d_rgb ) ) / SIX;
            }
        }
        return ZERO;
    };
    Color h( dec h ) const { return Color::hsva( h, sv( ), v( ), a( ) ); }
    Color & h( dec h ) { return *this = Color::hsva( h, sv( ), v( ), a( ) ); }

    dec sv( ) const { return max_rgb( ) ? ( ( max_rgb( ) - min_rgb( ) ) / max_rgb( ) ) : ZERO; }
    Color sv( dec s ) const { return Color::hsva( h( ), s, v( ), a( ) ); }
    Color & sv( dec s ) { return *this = Color::hsva( h( ), s, v( ), a( ) ); }

    dec v( ) const { return max_rgb( ); }
    Color v( dec v ) const { return Color::hsva( h( ), sv( ), v, a( ) ); }
    Color & v( dec v ) { return *this = Color::hsva( h( ), sv( ), v, a( ) ); }

    dec sl( ) const
    {
        return ( max_rgb( ) == min_rgb( ) ) ? ZERO :
            ( ( dec_le( l( ), HALF ) ) ?
              ( ( max_rgb( ) - min_rgb( ) ) / ( max_rgb( ) + min_rgb( ) ) ) :
              ( ( max_rgb( ) - min_rgb( ) / ( TWO - ( max_rgb( ) + min_rgb( ) ) ) ) ) );
    }
    Color sl( dec s ) const { return Color::hsla( h( ), s, l( ), a( ) ); }
    Color & sl( dec s ) { return *this = Color::hsla( h( ), s, l( ), a( ) ); }

    dec l( ) const { return half( max_rgb( ) + min_rgb( ) ); }
    Color l( dec l ) const { return Color::hsla( h( ), sl( ), l, a( ) ); }
    Color & l( dec l ) { return *this = Color::hsla( h( ), sl( ), l, a( ) ); }

    dec a( ) const { return m_a; }
    Color a( dec a ) const { return Color::rgba( r( ), g( ), b( ), a ); }
    Color & a( dec a ) { return *this = Color::rgba( r( ), g( ), b( ), a ); }

    bool opaque( ) const { return ( m_a == ONE ); }
    bool transparent( ) const { return ( m_a != ONE ); }
    bool visible( ) const { return m_a; }

    default_equal( Color );

private:
    dec m_r = ONE; // red value
    dec m_g = ONE; // green value
    dec m_b = ONE; // blue value
    dec m_a = ONE; // alpha value

    dec max_rgb( ) const { return max<dec>( { m_r, m_g, m_b } ); }
    dec min_rgb( ) const { return min<dec>( { m_r, m_g, m_b } ); }
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
    ColorSlider( const Color & start, const Color & end ) : m_color_start( start ), m_color_end( end ), m_slide( 0.0 ) { }

    void color_start( const Color & c )
    {
        m_color_start = c;
    }
    void color_end( const Color & c )
    {
        m_color_end = c;
    }

    const Color & color_start( ) const
    {
        return m_color_start;
    }
    const Color & color_end( ) const
    {
        return m_color_end;
    }

    dec slide( ) const
    {
        return m_slide;
    }
    void slide( dec s )
    {
        m_slide = s;
        Assert( ( m_slide >= ZERO ) && ( m_slide <= ONE ), "slide must be [ 0, 1 ]" ); \
    }

    Color color_at( dec f ) const
    {
        Assert( ( f >= ZERO ) && ( f <= ONE ), "slide must be [ 0, 1 ]" );

        dec d_r = m_color_end.r( ) - m_color_start.r( );
        dec d_g = m_color_end.g( ) - m_color_start.g( );
        dec d_b = m_color_end.b( ) - m_color_start.b( );
        dec d_a = m_color_end.a( ) - m_color_start.a( );

        dec r = m_color_start.r( ) + ( d_r * f );
        dec g = m_color_start.g( ) + ( d_g * f );
        dec b = m_color_start.b( ) + ( d_b * f );
        dec a = m_color_start.a( ) + ( d_a * f );

        return Color::rgba( r, g, b, a );
    }

    operator Color( ) const
    {
        return color_at( m_slide );
    }

    default_equal( ColorSlider );

private:
    Color m_color_start;
    Color m_color_end;
    dec m_slide;
};

} // namespace graphics
} // namespace axn

#endif /* Color_hpp */
