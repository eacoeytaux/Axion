#ifndef FixedRectangle_hpp
#define FixedRectangle_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"
#include "Vector.hpp"
#include "Transform.hpp"
#include "Line.hpp"
#include "Polygon.hpp"

namespace axn
{
namespace geometry
{

class FixedRectangle
{
public:
    virtual ~FixedRectangle( ) { }
    FixedRectangle( );
    FixedRectangle( const Planc & width, const Planc & height, const Coordinate & center = ORIGIN );
    FixedRectangle( const Coordinate & bottom, const Coordinate & top );

    const Coordinate & center( ) const;
    FixedRectangle & center( const Coordinate & );

    const Planc & width( ) const;
    FixedRectangle & width( const Planc & );

    const Planc & height( ) const;
    FixedRectangle & height( const Planc & );

    Coordinate top_left( ) const;
    Coordinate top_right( ) const;
    Coordinate bottom_left( ) const;
    Coordinate bottom_right( ) const;

    Planc upper_bound_x( ) const;
    Planc upper_bound_y( ) const;
    Planc lower_bound_x( ) const;
    Planc lower_bound_y( ) const;

    bool contains( const Coordinate & ) const;
    bool intersects( const Line & line ) const { return ( intersection( line ).size( ) > 0 ); }
    varray<Line> intersection( const Line & line ) const;

    Planc area( ) const;

    FixedRectangle operator+( const Vector & ) const;
    FixedRectangle & operator+=( const Vector & );
    FixedRectangle operator-( const Vector & ) const;
    FixedRectangle & operator-=( const Vector & );

    bool operator==( const FixedRectangle & _rect ) const;
    bool operator!=( const FixedRectangle & _rect ) const { return !( *this == _rect ); }

    operator Polygon( ) const;

private:
    Planc m_width;
    Planc m_height;
    Coordinate m_center;
};

} // namespace geometry
} // namespace axn

#endif /* FixedRectangle_hpp */
