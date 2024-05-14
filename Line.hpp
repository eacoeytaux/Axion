#ifndef Line_hpp
#define Line_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"

namespace axn
{
namespace geometry
{

class Line
{
public:
    virtual ~Line( ) { }
    Line( const Coordinate & c1 = ORIGIN, const Coordinate & c2 = ORIGIN );

    const Coordinate & c1( ) const;
    Line & c1( const Coordinate & );
    const Coordinate & c2( ) const;
    Line & c2( const Coordinate & );
    Angle angle( ) const;

    Line & rotate( const Angle &, const Coordinate & = ORIGIN );
    Line & mirror( const Vector & axis );
    Line & mirror_x( );
    Line & mirror_y( );

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

    virtual bool in_box( const Coordinate & ) const;
    bool on( const Coordinate & ) const;
    bool above( const Coordinate &, bool inclusive = false ) const;
    bool below( const Coordinate &, bool inclusive = false ) const;
    bool intersects( const Line & ) const;
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
