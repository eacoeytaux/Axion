#include "MountainRange.hpp"
#include "World.hpp"

using mtmercy::MountainRange;

namespace
{
const Span<Planc> MOUNTAIN_HEIGHT = { 1200, 3000 };
const Span<Planc> MOUNTAIN_BASE_LENGTH = { 4000, 7500 };
const Span<Planc> MOUNTAIN_OFFSET = { 500, 1500 };
} // namespace

MountainRange::MountainRange( Room * room, cPlanc _base_bottom, cdec _z, Color cref _color ) : Object( room )
{
    background( true );

    persist_render( true );

    stationary( true );

    z( _z );

    Camera * camera = room->camera( );
    FixedRectangle bounds = room->bounds( );

    // todo what if camera specs change?
    Planc offset_z = ( z( ) * camera->min_zoom( ) );
    Planc offset_x = ( half( camera->width( ) ) / ( z( ) * camera->min_zoom( ) ) );
    Planc offset_y = ( half( camera->height( ) ) / ( z( ) * camera->min_zoom( ) ) );

    Planc start_x = bounds.lower_bound_x( ) - offset_x;
    Planc end_x = bounds.upper_bound_x( ) + offset_x;

    Planc bottom = bounds.lower_bound_y( ) - offset_y;

    Planc half_mountain_base_max = half( MOUNTAIN_BASE_LENGTH.max( ) );

    Coordinate mountain_base_left( start_x, _base_bottom );
    Coordinate mountain_base_right( start_x, _base_bottom );
    Coordinate mountain_top( start_x, _base_bottom );

    draw( _color, Polygon( { Coordinate( start_x - half_mountain_base_max, _base_bottom ),
                             Coordinate( start_x - half_mountain_base_max, bottom ),
                             Coordinate( end_x + half_mountain_base_max, bottom ),
                             Coordinate( end_x + half_mountain_base_max, _base_bottom ) } ) );

    while( mountain_top.x( ) < end_x )
    {
        mountain_top.y( _base_bottom + Random::rPlanc( MOUNTAIN_HEIGHT ) );

        Planc mountain_base = Random::rPlanc( MOUNTAIN_BASE_LENGTH );
        mountain_base_left.x( mountain_top.x( ) - half( mountain_base ) );
        mountain_base_right.x( mountain_top.x( ) + half( mountain_base ) );

        draw( _color, Polygon( { mountain_top, mountain_base_left, mountain_base_right } ) );

        mountain_top.x( mountain_top.x( ) + Random::rPlanc( MOUNTAIN_OFFSET ) );
    }
}
