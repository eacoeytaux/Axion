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
        // C! stepped rows are moved half a cell, like bricks
        fill( in_width, in_height, in_width, in_height, ( stepped ? half( in_width ) : P0 ), in_offset );
    }

    // for hexagons
    Grid( FixedRectangle cref in_bounds, Planc cref in_side, Vector cref in_offset ) : m_bounds( in_bounds ), m_cell( Polygon::equilateral( 6, in_side ) )
    {
        // C! equilateral( ) puts a corner at the top, so a hexagon is side * sqrt( 3 ) wide and two sides tall,
        // C! rows are one and a half sides apart, and every other row is moved half a hexagon so they fit together
        const Planc width = ( in_side * sqrt( 3.0 ) );

        fill( width, ( in_side * 1.5 ), width, x2( in_side ), half( width ), in_offset );
    }

    // C! cells sit on a lattice through the bottom left corner of the bounds, moved by in_offset; a row of centers is in_step_x apart,
    // C! rows are in_step_y apart, and every odd row is moved in_row_shift along. Every cell that overlaps the bounds is kept,
    // C! including the ones only partly inside at the edges and corners
    void fill( Planc cref in_step_x, Planc cref in_step_y, Planc cref in_cell_width, Planc cref in_cell_height, Planc cref in_row_shift, Vector cref in_offset )
    {
        const dec step_x = dec( in_step_x );
        const dec step_y = dec( in_step_y );

        const dec half_width = half( dec( in_cell_width ) );
        const dec half_height = half( dec( in_cell_height ) );

        const dec lower_x = dec( bounds( ).lower_bound_x( ) );
        const dec lower_y = dec( bounds( ).lower_bound_y( ) );
        const dec upper_x = dec( bounds( ).upper_bound_x( ) );
        const dec upper_y = dec( bounds( ).upper_bound_y( ) );

        // C! with positive steps and finite bounds, the row and column loops below always reach the upper bounds
        return_if( !is_pos( step_x ) || !is_pos( step_y ) );
        return_if( !::isfinite( lower_x ) || !::isfinite( lower_y ) || !::isfinite( upper_x ) || !::isfinite( upper_y ) || !::isfinite( dec( in_offset.dx( ) ) ) || !::isfinite( dec( in_offset.dy( ) ) ) );

        // C! the lowest row whose cells reach into the bounds, counted on the lattice so the odd rows stay the same ones whatever the offset
        dec row = floor( ( ( lower_y - half_height ) - ( lower_y + half_height + dec( in_offset.dy( ) ) ) ) / step_y );

        do_until_break( )
        {
            dec y = lower_y + half_height + dec( in_offset.dy( ) ) + ( row * step_y );

            break_if( ( y - half_height ) >= upper_y );

            if( ( y + half_height ) > lower_y )
            {
                dec shift = ( is_odd( row ) ? dec( in_row_shift ) : 0.0 );
                dec x_start = lower_x + half_width + dec( in_offset.dx( ) ) + shift;

                dec column = floor( ( ( lower_x - half_width ) - x_start ) / step_x );

                m_grid.insert_back( );

                do_until_break( )
                {
                    dec x = x_start + ( column * step_x );

                    break_if( ( x - half_width ) >= upper_x );

                    if( ( x + half_width ) > lower_x )
                    {
                        m_grid.back( ).insert_back( Point( x, y ) );
                    }

                    ++column;
                }
            }

            ++row;
        }
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

    vvarray<Polygon> cells( ) const { vvarray<Polygon> cells; for_range( i, centers( ).size( ) ) { cells.insert_back( ); for_range( j, centers( )[ i ].size( ) ) { cells.back( ).insert_back( cell( ) + centers( )[ i ][ j ] ); } } return( cells ); }

    equal_default( Grid );

};

} // namespace geometry
} // namespace axn

#endif /* Grid_hpp */
