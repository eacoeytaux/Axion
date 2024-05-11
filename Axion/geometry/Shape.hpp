#ifndef Shape_hpp
#define Shape_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"
#include "Vector.hpp"
#include "Transform.hpp"
#include "Line.hpp"

namespace axn
{
namespace geometry
{

class Shape
{
public:
    static const uint CIRCLE_PRECISION;

    virtual ~Shape( ) { }
    Shape( const varray<Coordinate> & coordinates = { ORIGIN, ORIGIN, ORIGIN } );
    static Shape triangle( const Coordinate & c1, const Coordinate & c2, const Coordinate & c3 );
    static Shape rectangle( const Planc & width, const Planc & height, const Coordinate & center = ORIGIN, Angle rotation = Angle( 0 ) );
    static Shape square( const Planc & width, const Coordinate & center = ORIGIN, Angle rotation = Angle( 0 ) );
    static Shape equilateral( uint side_count, const Planc & radius = 1.0, const Coordinate & center = ORIGIN, Angle rotation = Angle( 0 ) );
    static Shape circle( const Planc & radius = 1.0, const Coordinate & center = ORIGIN );
    static Shape expand( const Shape & shape, Planc expansion );

    // TODO add local vs transformed
    Transform transform( ) const;
    const varray<Coordinate> & coordinates( ) const;
    const varray<Coordinate> & coordinates_raw( ) const;
    const varray<Line> & lines( ) const;
    const varray<Shape> & triangles( ) const;
    const varray<Shape> & convex_partitions( ) const;
    Shape convex_hull( ) const;

    Planc area( ) const;
    uint sides( ) const;
    bool convex( ) const;

    Planc bound_width( ) const { return upper_bound_x( ) - lower_bound_x( ); }
    Planc bound_height( ) const { return upper_bound_y( ) - lower_bound_y( ); }

    Planc upper_bound_x( ) const;
    Planc upper_bound_y( ) const;
    Planc lower_bound_x( ) const;
    Planc lower_bound_y( ) const;

    bool contains( const Coordinate & ) const;
    bool intersects( const Line & line ) const { return ( intersection( line ).size( ) > 0 ); }
    varray<Line> intersection( const Line & line ) const;

    Shape & transform( const Transform & t );

    Shape & move( const Vector & );
    Shape & stretch( const Vector & );
    Shape & scale( double scale, const Coordinate & origin = ORIGIN );
    Shape & rotate( const Angle & angle, const Coordinate & origin = ORIGIN );
    Shape & mirror( const Vector & axis );
    Shape & mirror_x( ) { return mirror( X_HAT ); }
    Shape & mirror_y( ) { return mirror( Y_HAT ); }

    Shape operator+( const Vector & ) const;
    Shape & operator+=( const Vector & );
    Shape operator-( const Vector & ) const;
    Shape & operator-=( const Vector & );

    bool operator==( const Shape & _shape ) const;
    bool operator!=( const Shape & _shape ) const { return !( *this == _shape ); }

private:
    mutable Transform m_transform;

    mutable varray<Coordinate> m_coordinates;
    mutable varray<Coordinate> m_coordinates_raw;
    mutable bool m_coordinates_dirty = true;

    mutable varray<Line> m_lines;
    mutable bool m_lines_dirty = true;

    mutable varray<Shape> m_triangles;
    mutable varray<varray<uint>> m_triangles_indices;
    mutable bool m_triangles_dirty = true;

    mutable varray<Shape> m_convex_partitions;
    mutable varray<varray<uint>> m_convex_partitions_indices;
    mutable bool m_convex_partitions_dirty = true;

    mutable varray<Coordinate> m_convex_hull;
    mutable bool m_convex_hull_dirty = true;

    mutable bool m_convex = true;
    mutable bool m_clockwise = false;
    mutable Planc m_lower_bound_x = INFINITY_POSITIVE;
    mutable Planc m_lower_bound_y = INFINITY_POSITIVE;
    mutable Planc m_upper_bound_x = INFINITY_NEGATIVE;
    mutable Planc m_upper_bound_y = INFINITY_NEGATIVE;

    void process( bool transform, bool lines, bool triangles, bool convex_partitions, bool convex_hull ) const;
    void dirty( ) const;
};

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

    operator Shape( ) const;

private:
    Planc m_width;
    Planc m_height;
    Coordinate m_center;
};

} // namespace geometry
} // namespace axn

#endif /* Shape_hpp */
