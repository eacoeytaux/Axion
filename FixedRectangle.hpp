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

    Coordinate top_right( ) const;
    Coordinate top_left( ) const;
    Coordinate bottom_left( ) const;
    Coordinate bottom_right( ) const;

    Coordinate top( ) const { return top_right( ); }
    Coordinate bottom( ) const { return bottom_left( ); }

    Planc upper_bound_x( ) const;
    Planc upper_bound_y( ) const;
    Planc lower_bound_x( ) const;
    Planc lower_bound_y( ) const;

    bool contains( const Coordinate & coordinate, bool inclusive = true ) const;
    bool intersects( const Line & line ) const;
    varray<Line> intersection( const Line & line ) const;

    Planc area( ) const;

    FixedRectangle & expand( const Planc & d_width_and_height ) { return expand( d_width_and_height, d_width_and_height ); }
    FixedRectangle & shrink( const Planc & d_width_and_height ) { return shrink( d_width_and_height, d_width_and_height ); }
    FixedRectangle & expand( const Planc & d_width, const Planc & d_height );
    FixedRectangle & shrink( const Planc & d_width, const Planc & d_height );
    FixedRectangle & expand_width( const Planc & d_width );
    FixedRectangle & shrink_width( const Planc & d_width );
    FixedRectangle & expand_height( const Planc & d_height );
    FixedRectangle & shrink_height( const Planc & d_height );

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
