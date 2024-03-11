#include "Line.hpp"

#include "Vector.hpp"

AXN_NAMESPACES
using axn::geometry::Line;

Line::Line( const Coordinate & c1, const Coordinate & c2 ) : m_c1( c1 ), m_c2( c2 ) {
    evaluate( );
}

void Line::evaluate( ) const {
    planc x1 = m_c1.x( );
    planc y1 = m_c1.y( );
    planc x2 = m_c2.x( );
    planc y2 = m_c2.y( );

    planc x_high, y_high, x_low, y_low;

    if( x1 <= x2 ) {
        x_low = x1;
        x_high = x2;
        if( ( x1 == x2 ) && ( y1 > y2 ) ) { // if is vertical
            y_low = y2;
            y_high = y1;
        } else {
            y_low = y1;
            y_high = y2;
        }
    } else {
        x_low = x2;
        y_low = y2;
        x_high = x1;
        y_high = y1;
    }

    if( x_low != x_high ) {
        if( y_low != y_high ) {
            m_vertical = false;
            m_horizontal = false;
            planc dx = ( x2 - x1 );
            planc dy = ( y2 - y1 );
            m_m = dy / dx;
            m_b = y_low - ( m_m * x_low );
        } else {
            m_vertical = false;
            m_horizontal = true;
            m_m = 0;
            m_b = y1;
        }
    } else {
        m_vertical = true;
        m_horizontal = false;
        m_m = 0;
        m_b = 0;
    }
}

const Coordinate & Line::c1( ) const { return m_c1; }
Line & Line::c1( const Coordinate & c ) {
    m_c1 = c;
    evaluate( );
    return *this;
}

const Coordinate & Line::c2( ) const { return m_c2; }
Line & Line::c2( const Coordinate & c ) {
    m_c2 = c;
    evaluate( );
    return *this;
}

Angle Line::angle( ) const { return Angle( c1( ), c2( ) ); }
Line & Line::rotate( const Angle & a, const Coordinate & origin ) {
    Vector v1( origin, c1( ) );
    v1.rotate( a );
    c1( v1.destination( ) );

    Vector v2( origin, c2( ) );
    v2.rotate( a );
    c2( v2.destination( ) );
    
    evaluate( );
    return *this;
}

Line & Line::mirror_x( ) {
    mirror( Vector( (planc)1.f, (planc)0.f ) );
    return *this;
}
Line & Line::mirror_y( ) {
    mirror( Vector( (planc)0.f, (planc)1.f ) );
    return *this;
}
Line & Line::mirror( const Vector & _axis ) {
    Coordinate c1_mirrored = c1( );
    c1_mirrored.mirror( _axis );
    c1( c1_mirrored );
    Coordinate c2_mirrored = c2( );
    c2_mirrored.mirror( _axis );
    c2( c2_mirrored );
    evaluate( );
    return *this;
}

bool Line::vertical( ) const { return m_vertical; }
bool Line::horizontal( ) const { return m_horizontal; }

planc Line::m( ) const { return m_m; }
planc Line::b( ) const { return m_b; }

planc Line::length( ) const { return c1( ).distance( c2( ) ); }

planc Line::x( planc y ) const {
    if( horizontal( ) ) {

    } else if( vertical( ) ) {
        if( in_range<planc>( y, c1( ).y( ), c2( ).y( ) ) ) {
            return c1( ).x( );
        }
    } else {
        planc x = ( y - m_b ) / m_m;
        if( in_box( Coordinate( x, y ) ) ) {
            return x;
        }
    }

    return -0; // no intersection found
}

planc Line::y( planc x ) const {
    if( vertical( ) ) {

    } else if( horizontal( ) ) {
        if( in_range<planc>( x, c1( ).x( ), c2( ).x( ) ) ) {
            return c1( ).y( );
        }
    } else {
        planc y = ( m_m * x ) + m_b;
        if( in_box( Coordinate( x, y ) ) ) {
            return y;
        }
    }

    return -0; // no intersection found
}

Coordinate Line::right( ) const {
    if( c1( ).x( ) >= c2( ).x( ) )
        return c1( );
    else
        return c2( );
}

Coordinate Line::left( ) const {
    if( c1( ).x( ) <= c2( ).x( ) )
        return c1( );
    else
        return c2( );
}

Coordinate Line::high( ) const {
    if( c1( ).y( ) >= c2( ).y( ) )
        return c1( );
    else
        return c2( );
}

Coordinate Line::low( ) const {
    if( c1( ).y( ) <= c2( ).y( ) )
        return c1( );
    else
        return c2( );
}

bool Line::in_box( const Coordinate & _c ) const {
    return in_range<planc>( _c.x( ), left( ).x( ), right( ).x( ) ) &&
           in_range<planc>( _c.y( ), low( ).y( ), high( ).y( ) );
}

inline planc line_eq( const Line & _l, const Coordinate & _c ) {
    planc x1 = _l.c1( ).x( );
    planc y1 = _l.c1( ).y( );
    planc x2 = _l.c2( ).x( );
    planc y2 = _l.c2( ).y( );
    planc eq = ( ( ( x2 - x1 ) * ( _c.y( ) - y1 ) ) - ( ( y2 - y1 ) * ( _c.x( ) - x1 ) ) );
    return eq;
}

bool Line::on( const Coordinate & _c ) const {
    planc eq = line_eq( *this, _c );
    return ( eq == 0.f );
}

bool Line::above( const Coordinate & _c, const bool _inclusive ) const {
    planc eq = line_eq( *this, _c );
    if( _inclusive && ( eq == 0.f ) )
        return true;
    else
        return ( eq < 0.f );
}

bool Line::below( const Coordinate & _c, const bool _inclusive ) const {
    planc eq = line_eq( *this, _c );
    if( _inclusive && ( eq == 0.f ) )
        return true;
    else
        return ( eq > 0.f );
}

bool Line::intersects( const Line & _line ) const {
    if( vertical( ) && _line.vertical( ) ) {
        return ( c1( ).x( ) == _line.c1( ).x( ) );
    } else if( horizontal( ) && _line.horizontal( ) ) {
        return ( c1( ).y( ) == _line.c1( ).y( ) );
    } else if( ( m_m == _line.m_m ) && ( m_b == _line.m_b ) ) {
        return true;
    }

    planc x_intersect;
    planc y_intersect;

    if( vertical( ) ) {
        x_intersect = low( ).x( );
        y_intersect = ( _line.horizontal( ) ?
            _line.low( ).y( ) : ( low( ).x( ) * _line.m_m ) + _line.m_b );
    } else if( _line.vertical( ) ) {
        x_intersect = _line.low( ).x( );
        y_intersect = ( horizontal( ) ?
            low( ).y( ) : ( _line.low( ).x( ) * m_m ) + m_b );
    } else if( horizontal( ) ) {
        x_intersect = ( low( ).y( ) - _line.m_b ) / _line.m_m;
        y_intersect = low( ).y( );
    } else if( _line.horizontal( ) ) {
        x_intersect = ( _line.low( ).y( ) - m_b ) / m_m;
        y_intersect = _line.low( ).y( );
    } else {
        x_intersect = ( _line.m_b - m_b ) / ( m_m - _line.m_m );
        y_intersect = ( x_intersect * m_m ) + m_b;
    }

    Coordinate intersect( x_intersect, y_intersect );
    return ( in_box( intersect ) && _line.in_box( intersect ) );
}

Coordinate Line::intersection( const Line & _line ) const {
    if( vertical( ) && _line.vertical( ) ) {
        if( c1( ).x( ) == _line.c1( ).x( ) ) {
            return c1( );
        }
    } else if( horizontal( ) && _line.horizontal( ) ) {
        if( c1( ).y( ) == _line.c1( ).y( ) ) {
            return c1( );
        }
    } else if( ( m_m == _line.m_m ) && ( m_b == _line.m_b ) ) {
        if( in_range( c1( ).x( ), _line.left( ).x( ), _line.right( ).x( ) ) ) {
            return c1( );
        }
        if( in_range( _line.c1( ).x( ), low( ).x( ), high( ).x( ) ) ) {
            return _line.c1( );
        }
    } else {
        planc x_intersect;
        planc y_intersect;

        if( vertical( ) ) {
            x_intersect = low( ).x( );
            y_intersect = ( _line.horizontal( ) ?
                _line.low( ).y( ) : ( low( ).x( ) * _line.m_m ) + _line.m_b );
        } else if( _line.vertical( ) ) {
            x_intersect = _line.low( ).x( );
            y_intersect = ( horizontal( ) ?
                           low( ).y( ) : ( _line.low( ).x( ) * m_m ) + m_b );
        } else if( horizontal( ) ) {
            x_intersect = ( low( ).y( ) - _line.m_b ) / _line.m_m;
            y_intersect = low( ).y( );
        } else if( _line.horizontal( ) ) {
            x_intersect = ( _line.low( ).y( ) - m_b ) / m_m;
            y_intersect = _line.low( ).y( );
        } else {
            x_intersect = ( _line.m_b - m_b ) / ( m_m - _line.m_m );
            y_intersect = ( x_intersect * m_m ) + m_b;
        }

        Coordinate intersect( x_intersect, y_intersect );
        //if( in_box( intersect ) && _line.in_box( intersect ) ) { // TODO MAKE LINE SEGMENT
            return intersect;
        //}
    }

    return Coordinate( NEGATIVE_INFINITY, NEGATIVE_INFINITY );
}

bool Line::operator==( const Line & _line ) const {
    return ( ( high( ) == _line.high( ) ) && ( low( ) == _line.low( ) ) ) ||
           ( ( right( ) == _line.right( ) ) && ( left( ) == _line.left( ) ) );
}

bool Line::operator!=( const Line & _line ) const { return !( *this == _line ); }

Line Line::operator+( const Vector & _v ) const { return Line( c1( ) + _v, c2( ) + _v ); }

Line & Line::operator+=( const Vector & _v ) {
    *this = Line( c1( ) + _v, c2( ) + _v );
    return *this;
}

Line Line::operator-( const Vector & _v ) const { return Line( c1( ) - _v, c2( ) - _v ); }

Line & Line::operator-=( const Vector & _v ) {
    *this = Line( c1( ) - _v, c2( ) - _v );
    return *this;
}
