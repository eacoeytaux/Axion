#ifndef Shape_hpp
#define Shape_hpp

#include "axn.hpp"
#include "Planc.hpp"
#include "Coordinate.hpp"
#include "Angle.hpp"
#include "Vector.hpp"
#include "Transform.hpp"
#include "Line.hpp"

namespace axn {
namespace geometry {

//class Triangle;
//class Polygon;

class Shape {
public:
    virtual ~Shape( ) {}
    Shape( const varray<Coordinate> & coordinates = { ORIGIN, ORIGIN, ORIGIN } );
    // creates equilateral
    static Shape circle( const planc & radius, const Coordinate & center = ORIGIN );
    static Shape triangle( const Coordinate & c1, const Coordinate & c2, const Coordinate & c3 );
    static Shape rectangle( const planc & width, const planc & height, const Coordinate & center = ORIGIN, Angle rotation = Angle( 0 ) );
    static Shape equilateral( int side_count, const planc & radius, const Coordinate & center = ORIGIN, Angle rotation = Angle( 0 ) );

    const varray<Coordinate> & coordinates( ) const;
    const varray<Line> & lines( ) const;
    const varray<Shape> & triangles( ) const;

    planc area( ) const;
    uint sides( ) const;
    bool convex( ) const;
    Shape convex_hull( ) const;

    planc bound_width( ) const { return upper_bound_x( ) - lower_bound_x( ); }
    planc bound_height( ) const { return upper_bound_y( ) - lower_bound_y( ); }
    float upper_bound_x( ) const;
    float upper_bound_y( ) const;
    float lower_bound_x( ) const;
    float lower_bound_y( ) const;

    bool contains( const Coordinate & ) const;
    bool intersects( const Line & line ) const {
        return ( intersection( line ).size( ) > 0 );
    }
    varray<Line> intersection( const Line & line ) const;

    Shape & transform( Transform & t ) {
        for_each( coordinate, m_coordinates ) { coordinate = t.transform( coordinate ); }
        return *this;
    }

    Shape & move( const Vector & );
    Shape & stretch( const Vector & );
    Shape & scale( float scale, const Coordinate & origin = ORIGIN );
    Shape & rotate( const Angle & angle, const Coordinate & origin = ORIGIN );
    Shape & mirror( const Vector & axis );
    Shape & mirror_x( ) { return mirror( X_HAT ); }
    Shape & mirror_y( ) { return mirror( Y_HAT ); }
    
    Shape & expand( planc distance );

    Shape operator+( const Vector & ) const;
    Shape & operator+=( const Vector & );
    Shape operator-( const Vector & ) const;
    Shape & operator-=( const Vector & );
    
    bool operator==( const Shape & _shape ) const;
    bool operator!=( const Shape & _shape ) const { return !( *this == _shape ); }

private:
    Transform m_transform;

    mutable varray<Coordinate> m_coordinates;
    mutable varray<Line> m_lines;
    mutable varray<Shape> m_triangles;
    
    bool m_convex = true;
    bool m_clockwise = false;
    float m_lower_bound_x = POSITIVE_INFINITY;
    float m_lower_bound_y = POSITIVE_INFINITY;
    float m_upper_bound_x = NEGATIVE_INFINITY;
    float m_upper_bound_y = NEGATIVE_INFINITY;
    
    void evaluate( ) const;
    varray<Shape> triangularize( const varray<Coordinate> & ) const;
    mutable bool m_dirty = true;
    bool dirty( ) const { return m_dirty; };
    const Shape & dirty( bool dirty ) const {
        m_dirty = dirty;
        return *this;
    }
};

/*
class Shape {
public:
    virtual ~Shape( ) {}
    Shape( ) { dirty( true ); }

    virtual const varray<Coordinate> & coordinates( ) const = 0;
    virtual const varray<Line> & lines( ) const = 0;
    // virtual const varray<Triangle> & triangles() const = 0;

    virtual planc area( ) const = 0;
    virtual uint sides( ) const { return coordinates( ).size( ); };
    virtual bool convex( ) const = 0;

    virtual planc width( ) const { return upper_bound_x( ) - lower_bound_x( ); }
    virtual planc height( ) const { return upper_bound_y( ) - lower_bound_y( ); }
    virtual float upper_bound_x( ) const = 0;
    virtual float upper_bound_y( ) const = 0;
    virtual float lower_bound_x( ) const = 0;
    virtual float lower_bound_y( ) const = 0;

    virtual bool contains( const Coordinate & ) const = 0;
    virtual bool intersects( const Line & line ) const {
        return ( intersection( line ).size( ) > 0 );
    }
    virtual varray<Line> intersection( const Line & line ) const = 0;

    virtual Shape & transform( Transform & t ) {
        varray<Coordinate> new_coordinates = coordinates( );
        for_each( coordinate, new_coordinates ) { coordinate = t.transform( coordinate ); }
        return *this;
    }

    virtual Shape & move( const Vector & ) = 0;
    virtual Shape & stretch( const Vector & ) = 0;
    virtual Shape & scale( float scale, const Coordinate & origin = ORIGIN ) = 0;
    virtual Shape & rotate( const Angle & angle, const Coordinate & origin = ORIGIN ) = 0;
    virtual Shape & mirror( const Vector & axis ) = 0;
    Shape & mirror_x( ) {
        return mirror( X_HAT );
        return *this;
    }
    Shape & mirror_y( ) {
        return mirror( Y_HAT );
        return *this;
    }

    bool operator!=( const Shape & _shape ) const { return !( *this == _shape ); }
    virtual bool operator==( const Shape & _shape ) const {
        if( coordinates( ).size( ) != _shape.coordinates( ).size( ) )
            return false;
        for_range( coordinates( ).size( ) ) if( coordinates( )[ i ] !=
                                                _shape.coordinates( )[ i ] ) return false;
        return true;
    }

    virtual operator Polygon( ) const = 0;

protected:
    Transform m_transform;

    mutable varray<Coordinate> m_coordinates;
    mutable bool m_coordinates_dirty = true;
    bool coordinates_dirty( ) const { return m_coordinates_dirty; };
    const Shape & coordinates_dirty( bool dirty ) const {
        m_coordinates_dirty = dirty;
        return *this;
    }

    mutable varray<Line> m_lines;
    mutable bool m_lines_dirty = true;
    bool lines_dirty( ) const { return m_lines_dirty; };
    const Shape & lines_dirty( bool dirty ) const {
        m_lines_dirty = dirty;
        return *this;
    }

    //    mutable varray<Triangle> m_triangles;
    //    mutable bool m_triangles_dirty = true;
    //    bool triangles_dirty() const { return m_triangles_dirty; };
    //    const Shape & triangles_dirty(bool dirty) const { m_triangles_dirty =
    //    dirty; return *this; }

    const Shape & dirty( bool b = true ) const {
        coordinates_dirty( b );
        lines_dirty( b ); // triangles_dirty(b );
        return *this;
    }
};

class Circle : public Shape {
public:
    static const uint PRECISION = 32;

    virtual ~Circle( ) {}
    Circle( const planc & radius = 0.f, const Coordinate & center = ORIGIN );

    Coordinate center( ) const;
    Circle & center( const Coordinate & center );
    planc radius( ) const;
    Circle & radius( const planc & radius );
    planc diameter( ) const;
    Circle & diameter( const planc & diameter );

    Circle & move( const Vector & vector ) override;
    Circle & stretch( const Vector & axis ) override {
        scale( axis.magnitude( ) );
        return *this;
    }
    Circle & scale( float scale, const Coordinate & origin = ORIGIN ) override;
    Circle & rotate( const Angle & angle, const Coordinate & origin = ORIGIN ) override;
    Circle & mirror( const Vector & axis ) override;

    float lower_bound_x( ) const override;
    float lower_bound_y( ) const override;
    float upper_bound_x( ) const override;
    float upper_bound_y( ) const override;
    planc area( ) const override;
    uint sides( ) const override;
    bool convex( ) const override;

    bool contains( const Coordinate & ) const override;
    varray<Line> intersection( const Line & line ) const override;

    planc x_high( const planc & y ) const;
    planc x_low( const planc & y ) const;
    planc y_high( const planc & x ) const;
    planc y_low( const planc & x ) const;

    const varray<Coordinate> & coordinates( ) const override;
    const varray<Line> & lines( ) const override;
    // const varray<Triangle> & triangles() const override;
    varray<Triangle> triangles( uint precision ) const;
    varray<Line> arc( const Angle &, const Angle & = Angle( ) ) const;

    planc convert_to_radians( const planc & distance ) const;
    planc convert_to_distance( const planc & radians ) const;

    Circle operator+( const Vector & ) const;
    Circle & operator+=( const Vector & );
    Circle operator-( const Vector & ) const;
    Circle & operator-=( const Vector & );

    bool operator==( const Circle & ) const;
    bool operator!=( const Circle & ) const;

    operator Polygon( ) const override;

private:
    planc m_radius;
    Coordinate m_center;
};

class Triangle : public Shape {
public:
    virtual ~Triangle( ) {}
    Triangle( const Coordinate & c1 = ORIGIN, const Coordinate & c2 = ORIGIN,
              const Coordinate & c3 = ORIGIN );
    Triangle( const Vector & offset1, const Vector & offset2, const Vector & offset3,
              const Coordinate & origin );

    Coordinate c1( ) const;
    Triangle & c1( const Coordinate & );
    Coordinate c2( ) const;
    Triangle & c2( const Coordinate & );
    Coordinate c3( ) const;
    Triangle & c3( const Coordinate & );

    Line line1( ) const;
    Line line2( ) const;
    Line line3( ) const;

    const varray<Coordinate> & coordinates( ) const override;
    const varray<Line> & lines( ) const override;
    // const varray<Triangle> & triangles() const override;

    planc area( ) const override;
    uint sides( ) const override;
    bool convex( ) const override;

    float lower_bound_x( ) const override;
    float lower_bound_y( ) const override;
    float upper_bound_x( ) const override;
    float upper_bound_y( ) const override;

    bool contains( const Coordinate & ) const override;
    varray<Line> intersection( const Line & line ) const override;

    Triangle & move( const Vector & ) override;
    Triangle & stretch( const Vector & axis ) override;
    Triangle & scale( float scale, const Coordinate & origin = ORIGIN ) override;
    Triangle & rotate( const Angle & angle, const Coordinate & origin = ORIGIN ) override;
    Triangle & mirror( const Vector & axis ) override;

    Triangle operator+( const Vector & ) const;
    Triangle & operator+=( const Vector & );
    Triangle operator-( const Vector & ) const;
    Triangle & operator-=( const Vector & );

    bool operator==( const Triangle & ) const;
    bool operator!=( const Triangle & ) const;

    operator Polygon( ) const override;
};

class Rectangle : public Shape {
public:
    virtual ~Rectangle( ) {}
    Rectangle( const planc & width = 0.f, const planc & height = 0.f,
               const Coordinate & center = ORIGIN, const Angle & rotation = Angle( ) );

    static Rectangle from_coordinates( const Coordinate & farthest, const Coordinate & closest,
                                       const Angle & rotation = Angle( ) );

    Coordinate center( ) const;
    Rectangle & center( const Coordinate & center );
    planc width( ) const override;
    Rectangle & width( const planc & );
    planc height( ) const override;
    Rectangle & height( const planc & );
    Angle rotation( ) const;

    Rectangle & move( const Vector & vector ) override;
    Rectangle & stretch( const Vector & axis ) override;
    Rectangle & scale( float scale, const Coordinate & origin = ORIGIN ) override;
    Rectangle & rotate( const Angle & angle, const Coordinate & origin = ORIGIN ) override;
    Rectangle & mirror( const Vector & axis ) override;

    Coordinate top_right( ) const;
    Coordinate top_left( ) const;
    Coordinate bottom_left( ) const;
    Coordinate bottom_right( ) const;
    Line top( ) const;
    Line left( ) const;
    Line bottom( ) const;
    Line right( ) const;
    float lower_bound_x( ) const override;
    float lower_bound_y( ) const override;
    float upper_bound_x( ) const override;
    float upper_bound_y( ) const override;
    planc diagonal_length( ) const;
    planc area( ) const override;
    uint sides( ) const override;
    bool convex( ) const override;

    bool contains( const Coordinate & ) const override;
    bool intersects( const Line & line ) const override;
    varray<Line> intersection( const Line & line ) const override;

    const varray<Coordinate> & coordinates( ) const override;
    const varray<Line> & lines( ) const override;
    // const varray<Triangle> & triangles() const override;

    struct Split : public varray<Rectangle> {
        Split( const Rectangle & top_right, const Rectangle & top_left,
               const Rectangle & bottom_left, const Rectangle & bottom_right );

        Rectangle top_right( );
        Rectangle top_left( );
        Rectangle bottom_left( );
        Rectangle bottom_right( );
    };
    Split split( const Coordinate & split_center ) const;

    Rectangle operator+( const Vector & ) const;
    Rectangle & operator+=( const Vector & );
    Rectangle operator-( const Vector & ) const;
    Rectangle & operator-=( const Vector & );

    bool operator==( const Rectangle & ) const;
    bool operator!=( const Rectangle & ) const;

    operator Polygon( ) const override;

private:
    Coordinate m_center;
    planc m_width, m_height;
    Angle m_rotation;
};

class RectangleFixed : public Rectangle {
public:
    virtual ~RectangleFixed( ) { };
    RectangleFixed( planc width = 0.f, planc height = 0.f, const Coordinate & center = ORIGIN )
        : Rectangle( width, height, center ) {}

    static RectangleFixed from_coordinates( const Coordinate & farthest,
                                            const Coordinate & closest ) {
        Rectangle r = Rectangle::from_coordinates( farthest, closest );
        return RectangleFixed( r.width( ), r.height( ), r.center( ) );
    }

    struct SplitFixed : public varray<RectangleFixed> {
        SplitFixed( const RectangleFixed & top_right, const RectangleFixed & top_left,
                    const RectangleFixed & bottom_left, const RectangleFixed & bottom_right );

        RectangleFixed top_right( );
        RectangleFixed top_left( );
        RectangleFixed bottom_left( );
        RectangleFixed bottom_right( );
    };
    SplitFixed split( const Coordinate & split_center ) const;

private:
    RectangleFixed & rotate( const Angle & angle,
                             const Coordinate & origin = ORIGIN ) override {
        return *this;
    }
};

class Square : public Rectangle {
public:
    virtual ~Square( ) {}
    Square( const planc & width = 0.f, const Coordinate & center = ORIGIN,
            const Angle & angle = Angle( ) );

    Square & width( const planc & );
    Square & height( const planc & );
};

class SquareFixed : public Square {
public:
    SquareFixed( const planc & width = 0.f, const Coordinate & center = ORIGIN )
        : Square( width, center ) {}

private:
    SquareFixed & rotate( const Angle & angle, const Coordinate & origin = ORIGIN ) override {
        return *this;
    }
};

class Polygon : public Shape {
public:
    Polygon( const varray<Coordinate> & coordinates = { ORIGIN, ORIGIN, ORIGIN } );
    Polygon( int side_count, const planc & radius, const Coordinate & center = ORIGIN,
             Angle rotation = Angle( 0 ) ); // creates equilateral

    Polygon & move( const Vector & vector ) override;
    Polygon & stretch( const Vector & axis ) override;
    Polygon & scale( float scale, const Coordinate & origin = ORIGIN ) override;
    Polygon & rotate( const Angle & angle, const Coordinate & origin = ORIGIN ) override;
    Polygon & mirror( const Vector & axis ) override;

    planc area( ) const override;
    uint sides( ) const override;

    bool convex( ) const override;
    Polygon convex_hull( ) const;

    float lower_bound_x( ) const override;
    float lower_bound_y( ) const override;
    float upper_bound_x( ) const override;
    float upper_bound_y( ) const override;

    bool contains( const Coordinate & ) const override;
    varray<Line> intersection( const Line & line ) const override;

    const varray<Coordinate> & coordinates( ) const override;
    const varray<Line> & lines( ) const override;
    const varray<Triangle> & triangles( ) const;

    Polygon operator+( const Vector & ) const;
    Polygon & operator+=( const Vector & );
    Polygon operator-( const Vector & ) const;
    Polygon & operator-=( const Vector & );

    bool operator==( const Polygon & ) const;
    bool operator!=( const Polygon & ) const;

    operator Polygon( ) const override;

private:
    bool m_convex = true;
    bool m_clockwise = false;
    float m_lower_bound_x = POSITIVE_INFINITY;
    float m_lower_bound_y = POSITIVE_INFINITY;
    float m_upper_bound_x = NEGATIVE_INFINITY;
    float m_upper_bound_y = NEGATIVE_INFINITY;

    mutable varray<Triangle> m_triangles;
    mutable bool m_triangles_dirty = true;
    bool triangles_dirty( ) const { return m_triangles_dirty; };
    const Shape & triangles_dirty( bool dirty ) const {
        m_triangles_dirty = dirty;
        return *this;
    }
    varray<Triangle> triangularize( const varray<Coordinate> & ) const;
};
*/

} // namespace geometry
} // namespace axn

#endif /* Shape_hpp */
