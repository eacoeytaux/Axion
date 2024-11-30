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
    
    static Polygon equilateral( uint side_count, const Planc & radius = 1.0, const Coordinate & center = ORIGIN, Angle rotation = Angle( 0 ) );
    
    static Polygon expand( const Polygon & polygon, const Planc & expansion );

    virtual ~Polygon( ) { }

    Polygon( const varray<Coordinate> & coordinates = { }, const Transform & = IDENTITY_TRANSFORM );
    
    transform_functions( Polygon );

    const varray<Coordinate> & coordinates( ) const;

    Path perimeter( ) const;
    
    varray<Polygon> triangles( ) const;
    varray<Polygon> convex_partitions( ) const;

    Planc area( ) const;
    uint sides( ) const;
    bool convex( ) const;

    Planc bound_width( ) const { return upper_bound_x( ) - lower_bound_x( ); }
    Planc bound_height( ) const { return upper_bound_y( ) - lower_bound_y( ); }

    Planc upper_bound_x( ) const;
    Planc upper_bound_y( ) const;
    Planc lower_bound_x( ) const;
    Planc lower_bound_y( ) const;

    bool contains( const Coordinate & coordinate, bool border_inclusive = true ) const;
    bool intersects( const Line & line ) const { return intersection( line ).size( ); }
    varray<Line> intersection( const Line & line ) const;

    Polygon operator+( const Vector & ) const;
    Polygon & operator+=( const Vector & );
    Polygon operator-( const Vector & ) const;
    Polygon & operator-=( const Vector & );

    bool operator==( const Polygon & _polygon ) const;
    default_non_equal( Polygon );

#ifdef AXN_DEBUG
    const varray<Coordinate> & coordinates_raw( ) const { return m_coordinates_raw; }
#endif
    
private:
    varray<Coordinate> m_coordinates_raw;
    mutable varray<Coordinate> m_coordinates;
    mutable bool m_coordinates_dirty = true;

    mutable Planc m_lower_bound_x = P0;
    mutable Planc m_lower_bound_y = P0;
    mutable Planc m_upper_bound_x = P0;
    mutable Planc m_upper_bound_y = P0;
    
    bool m_convex;
    
    bool m_processed = false;
    void process( const varray<Coordinate> & );
    
    void apply_transform( ) const;
    
    virtual const Polygon & dirty( ) const override;
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
    Circle( const Planc & radius = ONE, const Coordinate & center = ORIGIN );
};

} // namespace geometry
} // namespace axn

#endif /* Polygon_hpp */
