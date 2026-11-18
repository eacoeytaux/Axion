#include "mt/background/MountainRange.hpp"

var_const( span<Planc> MOUNTAIN_HEIGHT = { 1200, 3000 } );
var_const( span<Planc> MOUNTAIN_BASE_LENGTH = { 4000, 7500 } );
var_const( span<Planc> MOUNTAIN_OFFSET = { 500, 1500 } );

MountainRange::MountainRange( ptr<Room> in_room, Planc cref in_base_bottom, dec in_z, Color cref in_color ) : Object( in_room, in_z )
{
    background( true );

    persist_render( true );

    stationary( true );

    Planc bottom = room_bounds_z( ).lower_bound_y( );

    Planc lo_x = room_bounds_z( ).lower_bound_x( );
    Planc hi_x = room_bounds_z( ).upper_bound_x( );

    Point mountain_top = Point( lo_x - half( Random::rPlanc( MOUNTAIN_BASE_LENGTH ) ), in_base_bottom );

    Point mountain_base_lo = mountain_top;
    Point mountain_base_hi = mountain_top;

    MinMax<Planc> base_x( { lo_x, hi_x } );

    while( mountain_top.x( ) < hi_x )
    {
        Planc mountain_base = Random::rPlanc( MOUNTAIN_BASE_LENGTH );

        mountain_top.x( mountain_top.x( ) + Random::rPlanc( MOUNTAIN_OFFSET ) );
        mountain_top.y( in_base_bottom + Random::rPlanc( MOUNTAIN_HEIGHT ) );

        mountain_base_lo.x( mountain_top.x( ) - half( mountain_base ) );
        mountain_base_hi.x( mountain_top.x( ) + half( mountain_base ) );

        base_x.update( mountain_base_lo.x( ) );
        base_x.update( mountain_base_hi.x( ) );

        draw( in_color, Polygon( { mountain_top, mountain_base_lo, mountain_base_hi } ) );
    }

    draw( in_color, Polygon( {
        Point( base_x.min( ), bottom ),
        Point( base_x.max( ), bottom ),
        Point( base_x.max( ), in_base_bottom ),
        Point( base_x.min( ), in_base_bottom ) } ) );
}
