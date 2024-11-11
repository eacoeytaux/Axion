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

class Line : public Transformable
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

    virtual Line & transform( const Transform & t );

    bool vertical( ) const;
    bool horizontal( ) const;

    Planc m( ) const;
    Planc b( ) const;

    Planc length( ) const;

    Coordinate right( ) const;
    Coordinate left( ) const;
    Coordinate high( ) const;
    Coordinate low( ) const;

    Planc x( const Planc & y ) const;
    Planc y( const Planc & x ) const;

    bool on( const Coordinate & ) const;
    bool above( const Coordinate &, bool inclusive = false ) const;
    bool below( const Coordinate &, bool inclusive = false ) const;
    bool in_box( const Coordinate &, bool inclusive = true ) const;
    bool intersects( const Line &, bool inclusive = true ) const;
    Coordinate intersection( const Line & ) const;

    Line operator+( const Vector & ) const;
    Line & operator+=( const Vector & );
    Line operator-( const Vector & ) const;
    Line & operator-=( const Vector & );

    default_equal( Line );

private:
    Coordinate m_c1 = ORIGIN;
    Coordinate m_c2 = ORIGIN;

    Planc m_m = 0.0;
    Planc m_b = 0.0;

    bool m_vertical = false;
    bool m_horizontal = false;

    Line & evaluate( );
};

} // namespace geometry
} // namespace axn

#endif /* Line_hpp */
