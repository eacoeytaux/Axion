#include "Line.hpp"

#include "Vector.hpp"

using axn::geometry::Line;

Line::Line( const Coordinate & c1, const Coordinate & c2 ) : m_c1( c1 ), m_c2( c2 ) { evaluate( ); }

Line & Line::evaluate( )
{
    Planc x1 = m_c1.x( );
    Planc y1 = m_c1.y( );
    Planc x2 = m_c2.x( );
    Planc y2 = m_c2.y( );

    Planc x_high, y_high, x_low, y_low;

    if( x1 <= x2 )
    {
        x_low = x1;
        x_high = x2;

        if( ( x1 == x2 ) && ( y1 > y2 ) )
        { // if is vertical
            y_low = y2;
            y_high = y1;
        }
        else
        {
            y_low = y1;
            y_high = y2;
        }
    }
    else
    {
        x_low = x2;
        y_low = y2;
        x_high = x1;
        y_high = y1;
    }

    if( x_low != x_high )
    {
        if( y_low != y_high )
        {
            m_vertical = false;
            m_horizontal = false;
            Planc dx = ( x2 - x1 );
            Planc dy = ( y2 - y1 );
            m_m = dy / dx;
            m_b = y_low - ( m_m * x_low );
        }
        else
        {
            m_vertical = false;
            m_horizontal = true;
            m_m = 0;
            m_b = y1;
        }
    }
    else
    {
        m_vertical = true;
        m_horizontal = false;
        m_m = 0;
        m_b = 0;
    }

    return *this;
}

const Coordinate & Line::c1( ) const
{
    return m_c1;
}

Line & Line::c1( const Coordinate & c )
{
    m_c1 = c;
    return evaluate( );
}

const Coordinate & Line::c2( ) const
{
    return m_c2;
}

Line & Line::c2( const Coordinate & c )
{
    m_c2 = c;
    return evaluate( );
}

Angle Line::angle( ) const
{
    return Angle( c1( ), c2( ) );
}

Line & Line::rotate( const Angle & a, const Coordinate & origin )
{
    Vector v1( origin, c1( ) );
    v1.rotate( a );
    c1( v1.destination( ) );

    Vector v2( origin, c2( ) );
    v2.rotate( a );
    c2( v2.destination( ) );

    return evaluate( );
}

Line & Line::mirror_x( )
{
    return mirror( Vector( 1.0, 0.0 ) );
}

Line & Line::mirror_y( )
{
    return mirror( Vector( 0.0, 1.0 ) );
}

Line & Line::mirror( const Vector & _axis )
{
    Coordinate c1_mirrored = c1( );
    c1_mirrored.mirror( _axis );
    c1( c1_mirrored );

    Coordinate c2_mirrored = c2( );
    c2_mirrored.mirror( _axis );
    c2( c2_mirrored );

    return evaluate( );
}

bool Line::vertical( ) const { return m_vertical; }
bool Line::horizontal( ) const { return m_horizontal; }

Planc Line::m( ) const { return m_m; }
Planc Line::b( ) const { return m_b; }

Planc Line::length( ) const { return c1( ).distance( c2( ) ); }

Planc Line::x( const Planc & _y ) const
{
    if( horizontal( ) )
    {
        // TODO
    }
    else if( vertical( ) )
    {
        // if( in_range( _y, c1( ).y( ), c2( ).y( ) ) ) // todo
        {
            return c1( ).x( );
        }
    }
    else
    {
        Planc x = ( _y - m_b ) / m_m;
        // if( in_box( Coordinate( x, _y ) ) ) // todo
        {
            return x;
        }
    }

    return P0; // no intersection found
}

Planc Line::y( const Planc & _x ) const
{
    if( vertical( ) )
    {
        // TODO
    }
    else if( horizontal( ) )
    {
        // if( in_range( _x, c1( ).x( ), c2( ).x( ) ) ) // todo
        {
            return c1( ).y( );
        }
    }
    else
    {
        Planc y = ( m_m * _x ) + m_b;
        // if( in_box( Coordinate( _x, y ) ) ) // todo
        {
            return y;
        }
    }

    return P0; // no intersection found
}

Coordinate Line::right( ) const
{
    if( c1( ).x( ) >= c2( ).x( ) )
        return c1( );
    else
        return c2( );
}

Coordinate Line::left( ) const
{
    if( c1( ).x( ) <= c2( ).x( ) )
        return c1( );
    else
        return c2( );
}

Coordinate Line::high( ) const
{
    if( c1( ).y( ) >= c2( ).y( ) )
        return c1( );
    else
        return c2( );
}

Coordinate Line::low( ) const
{
    if( c1( ).y( ) <= c2( ).y( ) )
        return c1( );
    else
        return c2( );
}

bool Line::in_box( const Coordinate & _c ) const { return in_range( _c.x( ), left( ).x( ), right( ).x( ) ) && in_range( _c.y( ), low( ).y( ), high( ).y( ) ); }

inline Planc line_eq( const Line & _l, const Coordinate & _c )
{
    Planc x1 = _l.c1( ).x( );
    Planc y1 = _l.c1( ).y( );
    Planc x2 = _l.c2( ).x( );
    Planc y2 = _l.c2( ).y( );
    Planc eq = ( ( ( x2 - x1 ) * ( _c.y( ) - y1 ) ) - ( ( y2 - y1 ) * ( _c.x( ) - x1 ) ) );
    return eq;
}

bool Line::on( const Coordinate & _c ) const
{
    Planc eq = line_eq( *this, _c );
    return ( eq == 0.0 );
}

bool Line::above( const Coordinate & _c, const bool _inclusive ) const
{
    Planc eq = line_eq( *this, _c );
    if( _inclusive && ( eq == 0.0 ) )
        return true;
    else
        return ( eq < 0.0 );
}

bool Line::below( const Coordinate & _c, const bool _inclusive ) const
{
    Planc eq = line_eq( *this, _c );
    if( _inclusive && ( eq == 0.0 ) )
        return true;
    else
        return ( eq > 0.0 );
}

bool Line::intersects( const Line & _line ) const
{
    if( vertical( ) && _line.vertical( ) )
    {
        return ( c1( ).x( ) == _line.c1( ).x( ) );
    }
    else if( horizontal( ) && _line.horizontal( ) )
    {
        return ( c1( ).y( ) == _line.c1( ).y( ) );
    }
    else if( ( m_m == _line.m_m ) && ( m_b == _line.m_b ) )
    {
        return true;
    }

    Planc x_intersect;
    Planc y_intersect;

    if( vertical( ) )
    {
        x_intersect = low( ).x( );
        y_intersect = ( _line.horizontal( ) ? _line.low( ).y( ) : ( low( ).x( ) * _line.m_m ) + _line.m_b );
    }
    else if( _line.vertical( ) )
    {
        x_intersect = _line.low( ).x( );
        y_intersect = ( horizontal( ) ? low( ).y( ) : ( _line.low( ).x( ) * m_m ) + m_b );
    }
    else if( horizontal( ) )
    {
        x_intersect = ( low( ).y( ) - _line.m_b ) / _line.m_m;
        y_intersect = low( ).y( );
    }
    else if( _line.horizontal( ) )
    {
        x_intersect = ( _line.low( ).y( ) - m_b ) / m_m;
        y_intersect = _line.low( ).y( );
    }
    else
    {
        x_intersect = ( _line.m_b - m_b ) / ( m_m - _line.m_m );
        y_intersect = ( x_intersect * m_m ) + m_b;
    }

    Coordinate intersect( x_intersect, y_intersect );
    return ( in_box( intersect ) && _line.in_box( intersect ) );
}

Coordinate Line::intersection( const Line & _line ) const
{
    if( vertical( ) && _line.vertical( ) )
    {
        // if( c1( ).x( ) == _line.c1( ).x( ) ) // todo
        {
            return c1( );
        }
    }
    else if( horizontal( ) && _line.horizontal( ) )
    {
        // if( c1( ).y( ) == _line.c1( ).y( ) ) // todo
        {
            return c1( );
        }
    }
    else if( ( m_m == _line.m_m ) && ( m_b == _line.m_b ) )
    {
        // if( in_range( c1( ).x( ), _line.left( ).x( ), _line.right( ).x( ) ) ) // todo
        {
            return c1( );
        }
        // if( in_range( _line.c1( ).x( ), low( ).x( ), high( ).x( ) ) ) // todo
        // {
        //    return _line.c1( );
        // }
    }
    else
    {
        Planc x_intersect;
        Planc y_intersect;

        if( vertical( ) )
        {
            x_intersect = low( ).x( );
            y_intersect = ( _line.horizontal( ) ? _line.low( ).y( ) : ( low( ).x( ) * _line.m_m ) + _line.m_b );
        }
        else if( _line.vertical( ) )
        {
            x_intersect = _line.low( ).x( );
            y_intersect = ( horizontal( ) ? low( ).y( ) : ( _line.low( ).x( ) * m_m ) + m_b );
        }
        else if( horizontal( ) )
        {
            x_intersect = ( low( ).y( ) - _line.m_b ) / _line.m_m;
            y_intersect = low( ).y( );
        }
        else if( _line.horizontal( ) )
        {
            x_intersect = ( _line.low( ).y( ) - m_b ) / m_m;
            y_intersect = _line.low( ).y( );
        }
        else
        {
            x_intersect = ( _line.m_b - m_b ) / ( m_m - _line.m_m );
            y_intersect = ( x_intersect * m_m ) + m_b;
        }

        Coordinate intersect( x_intersect, y_intersect );
        // TODO MAKE LINE SEGMENT?
        // if( in_box( intersect ) && _line.in_box( intersect )
        {
            return intersect;
        }
    }

    return COORDINATE_INFINITY_NEGATIVE;
}

bool Line::operator==( const Line & _line ) const { return ( ( high( ) == _line.high( ) ) && ( low( ) == _line.low( ) ) ) || ( ( right( ) == _line.right( ) ) && ( left( ) == _line.left( ) ) ); }

bool Line::operator!=( const Line & _line ) const { return !( *this == _line ); }

Line Line::operator+( const Vector & _v ) const { return Line( c1( ) + _v, c2( ) + _v ); }

Line & Line::operator+=( const Vector & _v ) { return *this = *this + _v; }

Line Line::operator-( const Vector & _v ) const { return *this + -_v; }

Line & Line::operator-=( const Vector & _v ) { return *this = *this - _v; }
