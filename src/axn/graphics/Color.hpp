#ifndef Color_hpp
#define Color_hpp

#include "axn/system/Utility.hpp"

namespace axn
{
namespace graphics
{

// https://www.calculatorology.com/conversion/color/

class Color
{

public:

    enum grayscale_type
    {
        GRAYSCALE_AVERAGE,
        GRAYSCALE_LIGHTEST,
        GRAYSCALE_LUMINOSITY
    };

private:

    dec m_r = 1.0; // red value
    dec m_g = 1.0; // green value
    dec m_b = 1.0; // blue value
    dec m_a = 1.0; // alpha value

public:

    Color( ) : m_r( 0.0 ), m_g( 0.0 ), m_b( 0.0 ), m_a( 1.0 ) { }

    Color( Color cref in_color ) : m_r( in_color.m_r ), m_g( in_color.m_g ), m_b( in_color.m_b ), m_a( in_color.m_a ) { }
    Color( Color cref in_color, dec in_a ) : m_r( in_color.m_r ), m_g( in_color.m_g ), m_b( in_color.m_b ), m_a( in_a ) { }

    static Color rgb( uint in_rgb, dec in_a = 1.0 )
    {
        return( rgb( dec( ( in_rgb & 0xFF0000 ) >> 16 ) / 255.0,
                     dec( ( in_rgb & 0x00FF00 ) >> 8 ) / 255.0,
                     dec( ( in_rgb & 0x0000FF ) >> 0 ) / 255.0, in_a ) );
    }

    static Color rgb( dec in_r, dec in_g, dec in_b, dec in_a = 1.0 )
    {
        Assert( in_range_0_to_1( in_r, true ), "red must be [ 0-1 ]" );
        Assert( in_range_0_to_1( in_g, true ), "green must be [ 0-1 ]" );
        Assert( in_range_0_to_1( in_b, true ), "blue must be [ 0-1 ]" );
        Assert( in_range_0_to_1( in_a, true ), "alpha must be [ 0-1 ]" );

        Color color;

        color.m_r = in_r;
        color.m_g = in_g;
        color.m_b = in_b;
        color.m_a = in_a;

        return( color );
    }

    static Color cmyk( dec in_c, dec in_m, dec in_y, dec in_k, dec in_a = 1.0 )
    {
        Assert( in_range_0_to_1( in_c, true ), "cyan must be [ 0-1 ]" );
        Assert( in_range_0_to_1( in_m, true ), "magenta must be [ 0-1 ]" );
        Assert( in_range_0_to_1( in_y, true ), "yellow must be [ 0-1 ]" );
        Assert( in_range_0_to_1( in_k, true ), "black must be [ 0-1 ]" );
        Assert( in_range_0_to_1( in_a, true ), "alpha must be [ 0-1 ]" );

        in_k = one_minus( in_k );

        dec r = one_minus( in_c ) * in_k;
        dec g = one_minus( in_m ) * in_k;
        dec b = one_minus( in_y ) * in_k;

        return( rgb( r, g, b, in_a ) );
    }

    static Color hsv( dec in_h, dec in_s, dec in_v, dec in_a = 1.0 )
    {
        Assert( in_range_0_to_1( in_h, true ), "hue must be [ 0-1 ]" );
        Assert( in_range_0_to_1( in_s, true ), "saturation must be [ 0-1 ]" );
        Assert( in_range_0_to_1( in_v, true ), "value must be [ 0-1 ]" );
        Assert( in_range_0_to_1( in_a, true ), "alpha must be [ 0-1 ]" );

        dec r, g, b;

        dec h6 = ( in_h * 6.0 );

        dec c = ( in_v * in_s );
        dec m = ( in_v - c );
        dec x = ( c * one_minus( abs( mod( ( in_h * 6.0 ), 2.0 ) - 1.0 ) ) );

        if( h6 <= 1.0 )
        {
            r = m + c;
            g = m + x;
            b = m;
        }
        else if( h6 <= 2.0 )
        {
            r = m + x;
            g = m + c;
            b = m;
        }
        else if( h6 <= 3.0 )
        {
            r = m;
            g = m + c;
            b = m + x;
        }
        else if( h6 <= 4.0 )
        {
            r = m;
            g = m + x;
            b = m + c;
        }
        else if( h6 <= 5.0 )
        {
            r = m + x;
            g = m;
            b = m + c;
        }
        else // if( h6 <= 6.0 )
        {
            r = m + c;
            g = m;
            b = m + x;
        }

        return( rgb( r, g, b, in_a ) );
    }

    static Color hsl( dec in_h, dec in_s, dec in_l, dec in_a = 1.0 )
    {
        Assert( in_range_0_to_1( in_h, true ), "hue must be [ 0-1 ]" );
        Assert( in_range_0_to_1( in_s, true ), "saturation must be [ 0-1 ]" );
        Assert( in_range_0_to_1( in_l, true ), "lightness must be [ 0-1 ]" );
        Assert( in_range_0_to_1( in_a, true ), "alpha must be [ 0-1 ]" );

        dec r, g, b;

        dec h6 = ( in_h * 6.0 );

        dec c = ( in_s * one_minus( abs( ( in_l * 2.0 ) - 1.0 ) ) );
        dec m = ( in_l - half( c ) );
        dec x = ( c * one_minus( abs( mod( ( in_h * 6.0 ), 2.0 ) - 1.0 ) ) );

        if( h6 <= 1.0 )
        {
            r = m + c;
            g = m + x;
            b = m;
        }
        else if( h6 <= 2.0 )
        {
            r = m + x;
            g = m + c;
            b = m;
        }
        else if( h6 <= 3.0 )
        {
            r = m;
            g = m + c;
            b = m + x;
        }
        else if( h6 <= 4.0 )
        {
            r = m;
            g = m + x;
            b = m + c;
        }
        else if( h6 <= 5.0 )
        {
            r = m + x;
            g = m;
            b = m + c;
        }
        else // if( h6 <= 6.0 )
        {
            r = m + c;
            g = m;
            b = m + x;
        }

        return( rgb( r, g, b, in_a ) );
    }

    static Color gray( dec in_gray, dec in_a = 1.0 ) { return( rgb( in_gray, in_gray, in_gray, in_a ) ); }

    static Color grayscale( Color cref in_color, grayscale_type in_grayscale = GRAYSCALE_LUMINOSITY )
    {
        dec x;

        switch( in_grayscale )
        {
            default:
            {
                x = 0.0;
            }

            b_case( GRAYSCALE_AVERAGE ) :
            {
                x = ( in_color.r( ) + in_color.g( ) + in_color.b( ) ) / 3.0;
            }

            b_case( GRAYSCALE_LIGHTEST ) :
            {
                x = ( in_color.min_rgb( ) + in_color.max_rgb( ) ) / 2.0;
            }

            b_case( GRAYSCALE_LUMINOSITY ) :
            {
                x = ( in_color.r( ) * 0.299 ) + ( in_color.g( ) * 0.587 ) + ( in_color.b( ) * 0.114 );
            }
        }

        return( gray( x * in_color.a( ), in_color.a( ) ) );
    }

    static Color transparent( Color cref in_color ) { return( in_color.a( 0.0 ) ); }

    bool is_visible( ) const { return( !is_zero( m_a ) ); }
    bool is_opaque( ) const { return( is_one( m_a ) ); }
    bool is_transparent( ) const { return( !is_one( m_a ) ); }

    dec r( ) const { return( m_r ); }
    Color r( dec in_r ) const { return( rgb( in_r, g( ), b( ), a( ) ) ); }
    Color ref r( dec in_r ) { rethis = rgb( in_r, g( ), b( ), a( ) ); }

    dec g( ) const { return( m_g ); }
    Color g( dec in_g ) const { return( rgb( r( ), in_g, b( ), a( ) ) ); }
    Color ref g( dec in_g ) { rethis = rgb( r( ), in_g, b( ), a( ) ); }

    dec b( ) const { return( m_b ); }
    Color b( dec in_b ) const { return( rgb( r( ), g( ), in_b, a( ) ) ); }
    Color ref b( dec in_b ) { rethis = rgb( r( ), g( ), in_b, a( ) ); }

    dec c( ) const { return( one_minus( max_rgb( ) ) ); }
    Color c( dec in_c ) const { return( cmyk( in_c, m( ), y( ), k( ), a( ) ) ); }
    Color ref c( dec in_c ) { rethis = cmyk( in_c, y( ), m( ), k( ), a( ) ); }

    dec m( ) const { return( max_rgb( ) - r( ) ) / max_rgb( ); }
    Color m( dec in_m ) const { return( cmyk( c( ), in_m, y( ), k( ), a( ) ) ); }
    Color ref m( dec in_m ) { rethis = cmyk( c( ), in_m, y( ), k( ), a( ) ); }

    dec y( ) const { return( max_rgb( ) - g( ) ) / max_rgb( ); }
    Color y( dec in_y ) const { return( cmyk( c( ), m( ), in_y, k( ), a( ) ) ); }
    Color ref y( dec in_y ) { rethis = cmyk( c( ), m( ), in_y, k( ), a( ) ); }

    dec k( ) const { return( max_rgb( ) - b( ) ) / max_rgb( ); }
    Color k( dec in_k ) const { return( cmyk( c( ), m( ), y( ), in_k, a( ) ) ); }
    Color ref k( dec in_k ) { rethis = cmyk( c( ), m( ), y( ), in_k, a( ) ); }

    dec h( ) const
    {
        if( max_rgb( ) != min_rgb( ) )
        {
            dec d_rgb = max_rgb( ) - min_rgb( );
            if( max_rgb( ) == r( ) )
            {
                return( ( g( ) - b( ) ) / d_rgb ) / 6.0;
            }
            else if( max_rgb( ) == g( ) )
            {
                return( 2.0 + ( ( b( ) - r( ) ) / d_rgb ) ) / 6.0;
            }
            else if( max_rgb( ) == b( ) )
            {
                return( 4.0 + ( ( r( ) - g( ) ) / d_rgb ) ) / 6.0;
            }
        }

        return( 0.0 );
    };

    Color h( dec in_h ) const { return( hsv( in_h, sv( ), v( ), a( ) ) ); }
    Color ref h( dec in_h ) { rethis = hsv( in_h, sv( ), v( ), a( ) ); }

    dec sv( ) const { return( max_rgb( ) ? ( ( max_rgb( ) - min_rgb( ) ) / max_rgb( ) ) : ( 0.0 ) ); }
    Color sv( dec in_s ) const { return( hsv( h( ), in_s, v( ), a( ) ) ); }
    Color ref sv( dec in_s ) { rethis = hsv( h( ), in_s, v( ), a( ) ); }

    dec v( ) const { return( max_rgb( ) ); }
    Color v( dec in_v ) const { return( hsv( h( ), sv( ), in_v, a( ) ) ); }
    Color ref v( dec in_v ) { rethis = hsv( h( ), sv( ), in_v, a( ) ); }

    dec sl( ) const
    {
        return( max_rgb( ) == min_rgb( ) ) ? ( 0.0 ) :
            ( ( less_or_equal( l( ), 0.5 ) ) ?
              ( ( max_rgb( ) - min_rgb( ) ) / ( max_rgb( ) + min_rgb( ) ) ) :
              ( ( max_rgb( ) - min_rgb( ) / ( 2.0 - ( max_rgb( ) + min_rgb( ) ) ) ) ) );
    }

    Color sl( dec in_s ) const { return( hsl( h( ), in_s, l( ), a( ) ) ); }
    Color ref sl( dec in_s ) { rethis = hsl( h( ), in_s, l( ), a( ) ); }

    dec l( ) const { return( half( max_rgb( ) + min_rgb( ) ) ); }
    Color l( dec in_l ) const { return( hsl( h( ), sl( ), in_l, a( ) ) ); }
    Color ref l( dec in_l ) { rethis = hsl( h( ), sl( ), in_l, a( ) ); }

    dec a( ) const { return( m_a ); }
    Color a( dec in_a ) const { return( rgb( r( ), g( ), b( ), in_a ) ); }
    Color ref a( dec in_a ) { rethis = rgb( r( ), g( ), b( ), in_a ); }

    dec max_rgb( ) const { return( max<dec>( { m_r, m_g, m_b } ) ); }
    dec min_rgb( ) const { return( min<dec>( { m_r, m_g, m_b } ) ); }

    default_equal( Color );

};

var_const( Color TRANSPARENT = ( Color::rgb( 1.0, 1.0, 1.0, 0.0 ) ) );

var_const( Color WHITE = ( Color::rgb( 1.0, 1.0, 1.0 ) ) );
var_const( Color GRAY_ULTRA_LIGHT = ( Color::rgb( 0.875, 0.875, 0.875 ) ) );
var_const( Color GRAY_LIGHT = ( Color::rgb( 0.75, 0.75, 0.75 ) ) );
var_const( Color GRAY_MID_LIGHT = ( Color::rgb( 0.625, 0.625, 0.625 ) ) );
var_const( Color GRAY_MID = ( Color::rgb( 0.5, 0.5, 0.5 ) ) );
var_const( Color GRAY_MID_DARK = ( Color::rgb( 0.375, 0.375, 0.375 ) ) );
var_const( Color GRAY_DARK = ( Color::rgb( 0.25, 0.25, 0.25 ) ) );
var_const( Color GRAY_ULTRA_DARK = ( Color::rgb( 0.125, 0.125, 0.125 ) ) );
var_const( Color BLACK = ( Color::rgb( 0.0, 0.0, 0.0 ) ) );

var_const( Color RED = ( Color::rgb( 1.0, 0.0, 0.0 ) ) );
var_const( Color GREEN = ( Color::rgb( 0.0, 1.0, 0.0 ) ) );
var_const( Color BLUE = ( Color::rgb( 0.0, 0.0, 1.0 ) ) );

var_const( Color CYAN = ( Color::rgb( 0.0, 1.0, 1.0 ) ) );
var_const( Color MAGENTA = ( Color::rgb( 1.0, 0.0, 1.0 ) ) );
var_const( Color YELLOW = ( Color::rgb( 1.0, 1.0, 0.0 ) ) );

class ColorSlider
{

private:

    Color m_color_start;
    Color m_color_end;

    dec m_slide = 0.0;

public:

    ColorSlider( Color cref in_start, Color cref in_end ) : m_color_start( in_start ), m_color_end( in_end ), m_slide( 0.0 ) { }

    void color_start( Color cref in_color ) { m_color_start = in_color; }
    void color_end( Color cref in_color ) { m_color_end = in_color; }

    Color cref color_start( ) const { return( m_color_start ); }
    Color cref color_end( ) const { return( m_color_end ); }

    dec slide( ) const { return( m_slide ); }
    void slide( dec in_slide ) { m_slide = in_slide; Assert( in_range_0_to_1( m_slide, true ), "slide must be [ 0-1 ]" ); }

    Color color_at( dec in_slide ) const
    {
        Assert( in_range_0_to_1( in_slide, true ), "slide must be [ 0-1 ]" );

        dec d_r = m_color_end.r( ) - m_color_start.r( );
        dec d_g = m_color_end.g( ) - m_color_start.g( );
        dec d_b = m_color_end.b( ) - m_color_start.b( );
        dec d_a = m_color_end.a( ) - m_color_start.a( );

        dec r = m_color_start.r( ) + ( d_r * in_slide );
        dec g = m_color_start.g( ) + ( d_g * in_slide );
        dec b = m_color_start.b( ) + ( d_b * in_slide );
        dec a = m_color_start.a( ) + ( d_a * in_slide );

        return( Color::rgb( r, g, b, a ) );
    }

    operator Color( ) const { return( color_at( m_slide ) ); }

    default_equal( ColorSlider );

};

class Palette
{

private:

    umap<string, varray<Color>> m_color_map;

public:

    Palette( ) { };

    virtual void init( ) { };

    uset<string> cref keys( ) const { return( m_color_map.keys( ) ); }
    umap<string, varray<Color>> cref color_map( ) const { return( m_color_map ); }

    Color cref color( string cref in_key, uint in_index = 0 ) const { Assert( valid( in_key, in_index ) ); return( m_color_map.at( in_key )[ in_index ] ); }
    varray<Color> cref colors( string cref in_key ) const { Assert( valid( in_key ) ); return( m_color_map.at( in_key ) ); }
    uint color_count( string cref in_key ) const { Assert( valid( in_key ) ); return( m_color_map.at( in_key ).size( ) ); }

    uint add( string cref in_key, Color cref in_color ) { if( !m_color_map.contains( in_key ) ) { m_color_map.insert( in_key, { } ); } m_color_map.at( in_key ).insert_back( in_color ); return( m_color_map.at( in_key ).size( ) - 1 ); }

    bool valid( string cref in_key ) const { return( m_color_map.contains( in_key ) && m_color_map.at( in_key ).size( ) ); }
    bool valid( string cref in_key, uint in_index ) const { return( valid( in_key ) && m_color_map.at( in_key ).valid_index( in_index ) ); }

    default_equal( Palette );

};

} // namespace graphics
} // namespace axn

#endif /* Color_hpp */
