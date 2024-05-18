#ifndef Line_hpp
#define Line_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"
#include "Vector.hpp"
#include "Transform.hpp"

namespace axn
{
namespace geometry
{

class Line
{
public:
    virtual ~Line( ) { }
    Line( );
    Line( const Coordinate & c1, const Coordinate & c2 );
    Line( const Vector & v );

    const Coordinate & c1( ) const;
    const Coordinate & c2( ) const;

    Angle angle( ) const;
    Vector vector( ) const;

    Line & transform( const Transform & t );

    Line & move( const Vector & );
    Line & stretch( const Vector & );
    Line & scale( dec scale, const Coordinate & origin = ORIGIN );
    Line & rotate( const Angle & angle, const Coordinate & origin = ORIGIN );
    Line & mirror( const Vector & axis );
    Line & mirror_x( ) { return mirror( X_HAT ); }
    Line & mirror_y( ) { return mirror( Y_HAT ); }

    bool vertical( ) const;
    bool horizontal( ) const;
    virtual Planc length( ) const;
    Planc m( ) const;
    Planc b( ) const;

    Planc x( const Planc & y ) const;
    Planc y( const Planc & x ) const;

    Coordinate right( ) const;
    Coordinate left( ) const;
    Coordinate high( ) const;
    Coordinate low( ) const;

    virtual bool in_box( const Coordinate &, bool inclusive = true ) const;
    bool on( const Coordinate & ) const;
    bool above( const Coordinate &, bool inclusive = false ) const;
    bool below( const Coordinate &, bool inclusive = false ) const;
    bool intersects( const Line &, bool inclusive = true ) const;
    Coordinate intersection( const Line & ) const;

    bool operator==( const Line & ) const;
    bool operator!=( const Line & ) const;

    Line operator+( const Vector & ) const;
    Line & operator+=( const Vector & );
    Line operator-( const Vector & ) const;
    Line & operator-=( const Vector & );

private:
    mutable Coordinate m_c1, m_c2;
    mutable Planc m_m, m_b;
    mutable bool m_vertical, m_horizontal;

    Line & evaluate( );
};

} // namespace geometry
} // namespace axn

#endif /* Line_hpp */
