#ifndef Polygon_hpp
#define Polygon_hpp

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

class Polygon
{
public:
    static const uint CIRCLE_PRECISION;

    virtual ~Polygon( ) { }
    Polygon( const varray<Coordinate> & coordinates = { ORIGIN, ORIGIN, ORIGIN } );

    static Polygon triangle( const Coordinate & c1, const Coordinate & c2, const Coordinate & c3 );
    static Polygon rectangle( const Planc & width, const Planc & height, const Coordinate & center = ORIGIN, Angle rotation = Angle( 0 ) );
    static Polygon square( const Planc & width, const Coordinate & center = ORIGIN, Angle rotation = Angle( 0 ) );
    static Polygon circle( const Planc & radius = 1.0, const Coordinate & center = ORIGIN );
    static Polygon equilateral( uint side_count, const Planc & radius = 1.0, const Coordinate & center = ORIGIN, Angle rotation = Angle( 0 ) );
    static Polygon expand( const Polygon & polygon, Planc expansion );

    Transform transform( bool cumulative = true ) const;
    const varray<Coordinate> & coordinates( bool raw = false ) const;

    varray<Line> lines( bool raw = false ) const;
    varray<Polygon> triangles( bool raw = false ) const;
    varray<Polygon> convex_partitions( bool raw = false ) const;
    Polygon convex_hull( bool raw = false ) const;

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

    Polygon & transform( const Transform & t );

    Polygon & move( const Vector & );
    Polygon & stretch( const Vector & );
    Polygon & scale( double scale, const Coordinate & origin = ORIGIN );
    Polygon & rotate( const Angle & angle, const Coordinate & origin = ORIGIN );
    Polygon & mirror( const Vector & axis );
    Polygon & mirror_x( ) { return mirror( X_HAT ); }
    Polygon & mirror_y( ) { return mirror( Y_HAT ); }

    Polygon operator+( const Vector & ) const;
    Polygon & operator+=( const Vector & );
    Polygon operator-( const Vector & ) const;
    Polygon & operator-=( const Vector & );

    bool operator==( const Polygon & _polygon ) const;
    bool operator!=( const Polygon & _polygon ) const { return !( *this == _polygon ); }

private:
    mutable Transform m_transform;
    mutable Transform m_cumulative_transform;

    mutable varray<Coordinate> m_coordinates;
    mutable varray<Coordinate> m_coordinates_raw;
    mutable bool m_coordinates_dirty = true;

    mutable varray<Line> m_lines;
    mutable bool m_lines_dirty = true;

    mutable varray<Polygon> m_triangles;
    mutable varray<varray<uint>> m_triangles_indices;
    mutable bool m_triangles_dirty = true;

    mutable varray<Polygon> m_convex_partitions;
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

    void process( ) const;
    void process( bool transform, bool lines, bool triangles, bool convex_partitions, bool convex_hull ) const;
    void dirty( ) const;
};

} // namespace geometry
} // namespace axn

#endif /* Polygon_hpp */
