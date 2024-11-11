#ifndef Polygon_hpp
#define Polygon_hpp

#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"
#include "Vector.hpp"
#include "Transform.hpp"
#include "Line.hpp"
#include "Path.hpp"

namespace axn
{
namespace geometry
{

class Polygon : public Transformable
{
public:
    static uint circle_precision( dec perimeter = TAU );

    virtual ~Polygon( ) { }

    Polygon( );
    Polygon( const varray<Coordinate> & coordinates, bool assume_is_convex = false );
    Polygon( const varray<Coordinate> & coordinates, const Transform &, bool assume_is_convex = false );
    
    transform_functions( Polygon );
    
    static Polygon equilateral( uint side_count, const Planc & radius = 1.0, const Coordinate & center = ORIGIN, Angle rotation = Angle( 0 ) );
    static Polygon expand( const Polygon & polygon, const Planc & expansion );

    const varray<Coordinate> & coordinates( bool raw = false ) const;

    Path perimeter( bool raw = false ) const;
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

    bool contains( const Coordinate & coordinate, bool inclusive = true ) const;
    bool intersects( const Line & line ) const { return ( intersection( line ).size( ) > 0 ); }
    varray<Line> intersection( const Line & line ) const;

    Polygon operator+( const Vector & ) const;
    Polygon & operator+=( const Vector & );
    Polygon operator-( const Vector & ) const;
    Polygon & operator-=( const Vector & );

    bool operator==( const Polygon & _polygon ) const;
    default_non_equal( Polygon );

private:
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
    mutable varray<uint> m_convex_hull_indices;
    mutable bool m_convex_hull_dirty = true;

    mutable bool m_convex = true;
    mutable Planc m_lower_bound_x = P0;
    mutable Planc m_lower_bound_y = P0;
    mutable Planc m_upper_bound_x = P0;
    mutable Planc m_upper_bound_y = P0;
    
    virtual const Polygon & dirty( ) const override;
    void process( bool transform, bool lines, bool triangles, bool convex_partitions, bool convex_hull ) const;
};

class Triangle : public Polygon
{
public:
    virtual ~Triangle( ) { }
    Triangle( const Coordinate & c1, const Coordinate & c2, const Coordinate & c3 );
};

class Rectangle : public Polygon
{
public:
    virtual ~Rectangle( ) { }
    Rectangle( const Planc & width, const Planc & height, const Coordinate & center = ORIGIN, Angle rotation = Angle( 0 ) );
};

class Square : public Polygon
{
public:
    virtual ~Square( ) { }
    Square( const Planc & width_and_height, const Coordinate & center = ORIGIN, Angle rotation = Angle( 0 ) );
};

class Circle : public Polygon
{
public:
    virtual ~Circle( ) { }
    Circle( const Planc & radius = 1.0, const Coordinate & center = ORIGIN );
};

} // namespace geometry
} // namespace axn

#endif /* Polygon_hpp */
