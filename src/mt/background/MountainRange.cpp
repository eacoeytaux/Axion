#include "mt/background/MountainRange.hpp"

var_const( Span<Planc> MOUNTAIN_HEIGHT = { 1200, 3000 } );
var_const( Span<Planc> MOUNTAIN_BASE_LENGTH = { 4000, 7500 } );
var_const( Span<Planc> MOUNTAIN_OFFSET = { 500, 1500 } );

MountainRange::MountainRange( ptr<Room> in_room, Planc cref in_base_bottom, dec in_z, Color cref in_color ) : Object( in_room, in_z )
{
    background( true );

    persist_render( true );

    stationary( true );

    Planc bottom = ( bounds( ).lower_bound_y( ) );

    Planc lo_x = ( bounds( ).lower_bound_x( ) );
    Planc hi_x = ( bounds( ).upper_bound_x( ) );

    Point mountain_top = Point( lo_x - half( Random::rPlanc( MOUNTAIN_BASE_LENGTH ) ), in_base_bottom );

    Point mountain_base_lo = mountain_top;
    Point mountain_base_hi = mountain_top;

    Min<Planc> base_lo_x = lo_x;
    Max<Planc> base_hi_x = hi_x;

    while( mountain_top.x( ) < hi_x )
    {
        Planc mountain_base = Random::rPlanc( MOUNTAIN_BASE_LENGTH );

        mountain_top.x( mountain_top.x( ) + Random::rPlanc( MOUNTAIN_OFFSET ) );
        mountain_top.y( in_base_bottom + Random::rPlanc( MOUNTAIN_HEIGHT ) );

        mountain_base_lo.x( mountain_top.x( ) - half( mountain_base ) );
        mountain_base_hi.x( mountain_top.x( ) + half( mountain_base ) );

        base_lo_x.update( mountain_base_lo.x( ) );
        base_hi_x.update( mountain_base_hi.x( ) );

        draw( in_color, Polygon( { mountain_top, mountain_base_lo, mountain_base_hi } ) );
    }

    draw( in_color, Polygon( {
        Point( base_hi_x, bottom ),
        Point( base_hi_x, in_base_bottom ),
        Point( base_lo_x, in_base_bottom ),
        Point( base_lo_x, bottom ) } ) );
}
