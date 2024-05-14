#include "MountainRange.hpp"

#include "World.hpp"

using mtmercy::MountainRange;

namespace
{
// TODO
const Planc BOTTOM = -10000;
const Planc START_X = -100000;
const Planc END_X = 100000;
const Span<Planc> MOUNTAIN_HEIGHT = { 1200, 3000 };
const Span<Planc> MOUNTAIN_BASE_LENGTH = { 4000, 7500 };
const Span<Planc> MOUNTAIN_OFFSET = { 500, 1500 };
} // namespace

MountainRange::MountainRange( World * world, const Planc _base_bottom, const double _z, const Color & _color ) : Object( world )
{
    persist_drawing( true );

    z( _z );
    stationary( true );

    Coordinate mountain_base_left( START_X, _base_bottom );
    Coordinate mountain_base_right( START_X, _base_bottom );
    Coordinate mountain_top( START_X, _base_bottom );

    draw( _color, Polygon( { Coordinate( START_X, _base_bottom ), Coordinate( START_X, BOTTOM ), Coordinate( END_X, BOTTOM ), Coordinate( END_X, _base_bottom ) } ) );

    while( mountain_base_right.x( ) < END_X )
    {
        mountain_top.x( mountain_top.x( ) + Random::rPlanc( MOUNTAIN_OFFSET ) );
        mountain_top.y( _base_bottom + Random::rPlanc( MOUNTAIN_HEIGHT ) );

        Planc mountain_base = Random::rPlanc( MOUNTAIN_BASE_LENGTH );
        mountain_base_left.x( mountain_top.x( ) - half( mountain_base ) );
        mountain_base_right.x( mountain_top.x( ) + half( mountain_base ) );

        draw( _color, Polygon( { mountain_top, mountain_base_left, mountain_base_right } ) );
    }
}
