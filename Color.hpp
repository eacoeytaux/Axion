#ifndef Color_hpp
#define Color_hpp

#include "STD.hpp"
#include "Assert.hpp"
#include "Utility.hpp"

namespace axn
{
namespace graphics
{

cdec FILLED = 0.0;

// https://www.calculatorology.com/conversion/color/

axnclass( Color )
{

public:

    Color( ) :
        m_r( 0.0 ),
        m_g( 0.0 ),
        m_b( 0.0 ),
        m_a( 1.0 )
    {
    }

    Color( Color cref c ) :
        m_r( c.m_r ),
        m_g( c.m_g ),
        m_b( c.m_b ),
        m_a( c.m_a )
    {
    }

    Color( Color cref c, dec a ) :
        m_r( c.m_r ),
        m_g( c.m_g ),
        m_b( c.m_b ),
        m_a( a )
    {
    }

    static Color rgb( dec r, dec g, dec b ) { return Color::rgba( r, g, b, 1.0 ); }
    static Color rgba( dec r, dec g, dec b, dec a )
    {
        Assert( ( r >= 0.0 ) && ( r <= 1.0 ), "red must be [ 0, 1 ]" );
        Assert( ( g >= 0.0 ) && ( g <= 1.0 ), "green must be [ 0, 1 ]" );
        Assert( ( b >= 0.0 ) && ( b <= 1.0 ), "blue must be [ 0, 1 ]" );
        Assert( ( a >= 0.0 ) && ( a <= 1.0 ), "alpha must be [ 0, 1 ]" );

        Color c;

        c.m_r = r;
        c.m_g = g;
        c.m_b = b;
        c.m_a = a;

        return c;
    }

    static Color rgb( uint rgb ) { return Color::rgba( rgb, 1.0 ); }
    static Color rgba( uint rgb, dec a )
    {
        return Color::rgba( (dec)( ( rgb & 0xFF0000 ) >> 16 ) / 255.0,
                            (dec)( ( rgb & 0x00FF00 ) >> 8 ) / 255.0,
                            (dec)( ( rgb & 0x0000FF ) >> 0 ) / 255.0, a );
    }

    static Color cmyk( dec c, dec m, dec y, dec k ) { return Color::cmyka( c, m, y, k, 1.0 ); }
    static Color cmyka( dec c, dec m, dec y, dec k, dec a )
    {
        Assert( ( c >= 0.0 ) && ( c <= 1.0 ), "cyan must be [ 0, 1 ]" );
        Assert( ( m >= 0.0 ) && ( m <= 1.0 ), "magenta must be [ 0, 1 ]" );
        Assert( ( y >= 0.0 ) && ( y <= 1.0 ), "yellow must be [ 0, 1 ]" );
        Assert( ( k >= 0.0 ) && ( k <= 1.0 ), "black must be [ 0, 1 ]" );
        Assert( ( a >= 0.0 ) && ( a <= 1.0 ), "alpha must be [ 0, 1 ]" );

        k = ( 1.0 - k );
        dec r = ( 1.0 - c ) * k;
        dec g = ( 1.0 - m ) * k;
        dec b = ( 1.0 - y ) * k;
        return Color::rgba( r, g, b, a );
    }

    static Color hsv( dec h, dec s, dec v ) { return Color::hsva( h, s, v, 1.0 ); }
    static Color hsva( dec h, dec s, dec v, dec a )
    {
        Assert( ( h >= 0.0 ) && ( h <= 1.0 ), "hue must be [ 0, 1 ]" );
        Assert( ( s >= 0.0 ) && ( s <= 1.0 ), "saturation must be [ 0, 1 ]" );
        Assert( ( v >= 0.0 ) && ( v <= 1.0 ), "value must be [ 0, 1 ]" );
        Assert( ( a >= 0.0 ) && ( a <= 1.0 ), "alpha must be [ 0, 1 ]" );

        dec r, g, b;

        dec c = v * s;
        dec m = v - c;
        dec x = c * ( 1.0 - abs( ( h * 6.0 ) - floor( h * 3.0 ) - 1.0 ) );

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
        else // if( x <= 6.0 )
        {
            r = m + c;
            g = m;
            b = m + x;
        }

        return Color::rgba( r, g, b, a );
    }

    static Color hsl( dec h, dec s, dec l ) { return Color::hsla( h, s, l, 1.0 ); }
    static Color hsla( dec h, dec s, dec l, dec a )
    {
        Assert( ( h >= 0.0 ) && ( h <= 1.0 ), "hue must be [ 0, 1 ]" );
        Assert( ( s >= 0.0 ) && ( s <= 1.0 ), "saturation must be [ 0, 1 ]" );
        Assert( ( l >= 0.0 ) && ( l <= 1.0 ), "lightness must be [ 0, 1 ]" );
        Assert( ( a >= 0.0 ) && ( a <= 1.0 ), "alpha must be [ 0, 1 ]" );

        dec r, g, b;

        dec c = s * ( 1.0 - abs( ( l * 2.0 ) - 1.0 ) );
        dec m = l - ( c / 2.0 );
        dec x = c * ( 1.0 - abs( ( h * 6.0 ) - floor( h * 3.0 ) - 1.0 ) );

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
        else // if( x <= 6.0 )
        {
            r = m + c;
            g = m;
            b = m + x;
        }

        return Color::rgba( r, g, b, a );
    }

    dec r( ) const { return m_r; }
    Color r( dec r ) const { return Color::rgba( r, g( ), b( ), a( ) ); }
    Color & r( dec r ) { rethis = Color::rgba( r, g( ), b( ), a( ) ); }

    dec g( ) const { return m_g; }
    Color g( dec g ) const { return Color::rgba( r( ), g, b( ), a( ) ); }
    Color & g( dec g ) { rethis = Color::rgba( r( ), g, b( ), a( ) ); }

    dec b( ) const { return m_b; }
    Color b( dec b ) const { return Color::rgba( r( ), g( ), b, a( ) ); }
    Color & b( dec b ) { rethis = Color::rgba( r( ), g( ), b, a( ) ); }

    dec c( ) const { return 1.0 - max_rgb( ); }
    Color c( dec c ) const { return Color::cmyka( c, m( ), y( ), k( ), a( ) ); }
    Color & c( dec c ) { rethis = Color::cmyka( c, y( ), m( ), k( ), a( ) ); }

    dec m( ) const { return ( max_rgb( ) - r( ) ) / max_rgb( ); }
    Color m( dec m ) const { return Color::cmyka( c( ), m, y( ), k( ), a( ) ); }
    Color & m( dec m ) { rethis = Color::cmyka( c( ), m, y( ), k( ), a( ) ); }

    dec y( ) const { return ( max_rgb( ) - g( ) ) / max_rgb( ); }
    Color y( dec y ) const { return Color::cmyka( c( ), m( ), y, k( ), a( ) ); }
    Color & y( dec y ) { rethis = Color::cmyka( c( ), m( ), y, k( ), a( ) ); }

    dec k( ) const { return ( max_rgb( ) - b( ) ) / max_rgb( ); }
    Color k( dec k ) const { return Color::cmyka( c( ), m( ), y( ), k, a( ) ); }
    Color & k( dec k ) { rethis = Color::cmyka( c( ), m( ), y( ), k, a( ) ); }

    dec h( ) const
    {
        if( max_rgb( ) != min_rgb( ) )
        {
            dec d_rgb = max_rgb( ) - min_rgb( );
            if( max_rgb( ) == r( ) )
            {
                return ( ( g( ) - b( ) ) / d_rgb ) / 6.0;
            }
            else if( max_rgb( ) == g( ) )
            {
                return ( 2.0 + ( ( b( ) - r( ) ) / d_rgb ) ) / 6.0;
            }
            else if( max_rgb( ) == b( ) )
            {
                return ( 4.0 + ( ( r( ) - g( ) ) / d_rgb ) ) / 6.0;
            }
        }

        return 0.0;
    };
    Color h( dec h ) const { return Color::hsva( h, sv( ), v( ), a( ) ); }
    Color & h( dec h ) { rethis = Color::hsva( h, sv( ), v( ), a( ) ); }

    dec sv( ) const { return max_rgb( ) ? ( ( max_rgb( ) - min_rgb( ) ) / max_rgb( ) ) : 0.0; }
    Color sv( dec s ) const { return Color::hsva( h( ), s, v( ), a( ) ); }
    Color & sv( dec s ) { rethis = Color::hsva( h( ), s, v( ), a( ) ); }

    dec v( ) const { return max_rgb( ); }
    Color v( dec v ) const { return Color::hsva( h( ), sv( ), v, a( ) ); }
    Color & v( dec v ) { rethis = Color::hsva( h( ), sv( ), v, a( ) ); }

    dec sl( ) const
    {
        return ( max_rgb( ) == min_rgb( ) ) ? 0.0 :
            ( ( less_or_equal( l( ), 0.5 ) ) ?
              ( ( max_rgb( ) - min_rgb( ) ) / ( max_rgb( ) + min_rgb( ) ) ) :
              ( ( max_rgb( ) - min_rgb( ) / ( 2.0 - ( max_rgb( ) + min_rgb( ) ) ) ) ) );
    }
    Color sl( dec s ) const { return Color::hsla( h( ), s, l( ), a( ) ); }
    Color & sl( dec s ) { rethis = Color::hsla( h( ), s, l( ), a( ) ); }

    dec l( ) const { return half( max_rgb( ) + min_rgb( ) ); }
    Color l( dec l ) const { return Color::hsla( h( ), sl( ), l, a( ) ); }
    Color & l( dec l ) { rethis = Color::hsla( h( ), sl( ), l, a( ) ); }

    dec a( ) const { return m_a; }
    Color a( dec a ) const { return Color::rgba( r( ), g( ), b( ), a ); }
    Color & a( dec a ) { rethis = Color::rgba( r( ), g( ), b( ), a ); }

    bool opaque( ) const { return ( m_a == 1.0 ); }
    bool transparent( ) const { return ( m_a != 1.0 ); }
    bool visible( ) const { return m_a; }

    default_equal( Color );

private:

    dec m_r = 1.0; // red value
    dec m_g = 1.0; // green value
    dec m_b = 1.0; // blue value
    dec m_a = 1.0; // alpha value

    dec max_rgb( ) const { return max<dec>( { m_r, m_g, m_b } ); }
    dec min_rgb( ) const { return min<dec>( { m_r, m_g, m_b } ); }
    
};

cColor TRANSPARENT = Color::rgba( 1.0, 1.0, 1.0, 0.0 );

cColor WHITE = Color::rgb( 1.0, 1.0, 1.0 );
cColor GRAY_LIGHT = Color::rgb( 0.75, 0.75, 0.75 );
cColor GRAY_MID = Color::rgb( 0.5, 0.5, 0.5 );
cColor GRAY_DARK = Color::rgb( 0.25, 0.25, 0.25 );
cColor BLACK = Color::rgb( 0.0, 0.0, 0.0 );

cColor RED = Color::rgb( 1.0, 0.0, 0.0 );
cColor GREEN = Color::rgb( 0.0, 1.0, 0.0 );
cColor BLUE = Color::rgb( 0.0, 0.0, 1.0 );

cColor CYAN = Color::rgb( 0.0, 1.0, 1.0 );
cColor MAGENTA = Color::rgb( 1.0, 0.0, 1.0 );
cColor YELLOW = Color::rgb( 1.0, 1.0, 0.0 );

axnclass( ColorSlider )
{

public:

    ColorSlider( Color cref start, Color cref end ) : m_color_start( start ), m_color_end( end ), m_slide( 0.0 ) { }

    void color_start( Color cref c )
    {
        m_color_start = c;
    }
    void color_end( Color cref c )
    {
        m_color_end = c;
    }

    Color cref color_start( ) const
    {
        return m_color_start;
    }
    Color cref color_end( ) const
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
        Assert( ( m_slide >= 0.0 ) && ( m_slide <= 1.0 ), "slide must be [ 0, 1 ]" ); \
    }

    Color color_at( dec f ) const
    {
        Assert( ( f >= 0.0 ) && ( f <= 1.0 ), "slide must be [ 0, 1 ]" );

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
