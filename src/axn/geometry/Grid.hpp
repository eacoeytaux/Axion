#ifndef Grid_hpp
#define Grid_hpp

#include "axn/geometry/Planc.hpp"
#include "axn/geometry/Point.hpp"
#include "axn/geometry/Angle.hpp"
#include "axn/geometry/Vector.hpp"
#include "axn/geometry/Line.hpp"
#include "axn/geometry/Path.hpp"
#include "axn/geometry/Curve.hpp"
#include "axn/geometry/Triangle.hpp"
#include "axn/geometry/FixedRectangle.hpp"
#include "axn/geometry/Polygon.hpp"
#include "axn/geometry/Transform.hpp"

namespace axn
{
namespace geometry
{

class Grid // todo? : public Transformable
{

private:

    FixedRectangle m_bounds;

    Polygon m_cell;
    vvarray<Point> m_grid;

    // for rectangles
    Grid( FixedRectangle cref in_bounds, Planc cref in_width, Planc cref in_height, Vector cref in_offset, bool stepped ) : m_bounds( in_bounds ), m_cell( Polygon::rectangle( in_width, in_height ) )
    {
        const Planc width = in_width;
        const Planc height = in_height;

        // TODO use offset
        const Vector offset = Vector( remainder( in_offset.dx( ), width ), remainder( in_offset.dy( ), height ) );

        Point center = bounds( ).top_left( );

        do_while
        {
            m_grid.insert_back( );

            do_while
            {
                m_grid.back( ).insert_back( center );

                center.x( center.x( ) + width );
            }
            while( center.x( ) <= bounds( ).upper_bound_x( ) - half( width ) );

            center.x( offset.dx( ) );
            center.y( center.y( ) + height );

            if( stepped )
            {
                center.x( center.x( ) + negated( width, is_even( m_grid.size( ) ) ) );
            }
        }
        while( center.y( ) <= bounds( ).upper_bound_y( ) - half( height ) );
    }

    // for hexagons
    Grid( FixedRectangle cref in_bounds, Planc cref in_side, Vector cref in_offset ) : m_bounds( in_bounds ), m_cell( Polygon::equilateral( 6, in_side ) )
    {
        const Planc width = x2( in_side );
        const Planc height = in_side * sqrt( 3.0 );

        // TODO use offset
        // todo needs to account for corners
        const Vector offset = Vector( remainder( in_offset.dx( ), width ), remainder( in_offset.dy( ), height ) );

        Point center = bounds( ).top_left( );

        do_while
        {
            m_grid.insert_back( );

            do_while
            {
                m_grid.back( ).insert_back( center );

                center.x( center.x( ) + x2( width ) );
            }
            while( center.x( ) <= bounds( ).upper_bound_x( ) - half( width ) );

            center.x( offset.dx( ) );
            center.y( center.y( ) + half( height ) );

            if( is_even( m_grid.size( ) ) )
            {
                center.x( center.x( ) + half( width ) );
            }
        }
        while( center.y( ) <= bounds( ).upper_bound_y( ) - half( height ) );
    }

    vvarray<Point> cref grid( ) const { return( m_grid ); }

public:

    static Grid rectangle( FixedRectangle cref in_bounds, Planc cref in_width, Planc cref in_height, Vector cref in_offset = V0 ) { return( Grid( in_bounds, in_width, in_height, in_offset, false ) ); }
    static Grid rectangle_stepped( FixedRectangle cref in_bounds, Planc cref in_width, Planc cref in_height, Vector cref in_offset = V0 ) { return( Grid( in_bounds, in_width, in_height, in_offset, true ) ); }

    static Grid square( FixedRectangle cref in_bounds, Planc cref in_width, Vector cref in_offset = V0 ) { return( rectangle( in_bounds, in_width, in_width, in_offset ) ); }
    static Grid square_stepped( FixedRectangle cref in_bounds, Planc cref in_width, Vector cref in_offset = V0 ) { return( rectangle_stepped( in_bounds, in_width, in_width, in_offset ) ); }

    static Grid hexagon( FixedRectangle cref in_bounds, Planc cref in_side, Vector cref in_offset = V0 ) { return( Grid( in_bounds, in_side, in_offset ) ); }

    bool in_bounds( Point cref in_pt, bool in_inclusive = true ) const { return( bounds( ).contains( in_pt, in_inclusive ) ); }

    FixedRectangle cref bounds( ) const { return( m_bounds ); }

    vvarray<Point> cref centers( ) const { return( grid( ) ); }

    Polygon cref cell( ) const { return( m_cell ); }
    Polygon cref cell( uint in_x, uint in_y ) const { return( cell( ) + grid( )[ in_x ][ in_y ] ); }

    vvarray<Polygon> cref cells( ) const { vvarray<Polygon> cells; for_range( i, centers( ).size( ) ) { cells.insert_back( ); for_range( j, centers( )[ i ].size( ) ) { cells.back( ).insert_back( cell( ) + centers( )[ i ][ j ] ); } } return( cells ); }

    equal_default( Grid );

};

} // namespace geometry
} // namespace axn

#endif /* Grid_hpp */
