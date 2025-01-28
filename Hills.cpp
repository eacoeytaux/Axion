#include "Hills.hpp"
#include "World.hpp"

using mtmercy::Hills;

namespace
{

cPlanc HILL_RADIUS = 1000.0;
cPlanc HILL_SCALE_X = 2.0;
const Span<Planc> HILL_OFFSET_X = { HILL_RADIUS * 0.8, HILL_RADIUS * 1.75 };
const Span<Planc> HILL_OFFSET_Y = { 0.0, HILL_RADIUS * 0.25 };
cPlanc TREE_HEIGHT = 225.0;
cPlanc TREE_BASE = 150.0;
const Span<uint> INNER_TREE_COUNT = { 20, 30 };
const Span<uint> TOP_TREE_COUNT = { 8, 12 };
const Angle TOP_TREE_ANGLE = RIGHT / 1.5;
cPlanc TREE_LINE_BASE = HILL_RADIUS;
cPlanc TREE_LINE_PENUMBRA = TREE_LINE_BASE * 2.0;
cuint TREE_LINE_PENUMBRA_PASSES = 2;

const Color GRASS_COLOR = Color::rgb( 0x00EE00 );
const Color TREE_COLOR = Color::rgb( 0x00CC00 );
const Color TREE_2_COLOR = Color::rgb( 0x00AA00 );
}

Hills::Hills( Room * room, cPlanc _base_bottom, cdec _z ) : Object( room )
{
    background( true );

    persist_render( true );

    stationary( true );

    z( _z );

    Camera * camera = room->camera( );
    FixedRectangle bounds = room->bounds( );

    // todo what if camera specs change?
    Planc offset_x = ( half( camera->width( ) ) / ( z( ) * camera->min_zoom( ) ) );
    Planc offset_y = ( half( camera->height( ) ) / ( z( ) * camera->min_zoom( ) ) );

    Planc start_x = bounds.lower_bound_x( ) - offset_x;
    Planc end_x = bounds.upper_bound_x( ) + offset_x;

    Planc bottom = bounds.lower_bound_y( ) - offset_y;

    draw( GRASS_COLOR, Polygon( { Coordinate( start_x, _base_bottom ),
                                  Coordinate( start_x, bottom ),
                                  Coordinate( end_x, bottom ),
                                  Coordinate( end_x, _base_bottom ) } ) );

    Polygon tree( { Coordinate( 0.0, TREE_HEIGHT ), Coordinate( -half( TREE_BASE ), 0.0 ), Coordinate( half( TREE_BASE ), 0.0 ) } );
    Polygon hill = Polygon::circle( HILL_RADIUS, Coordinate( 0.0, _base_bottom ) ).stretch( VectorX( HILL_SCALE_X ) );

    varray<Vector> centers = { };

    Planc center_x = start_x;
    while( center_x < end_x )
    {
        Vector offset( center_x, Random::rPlanc( HILL_OFFSET_Y ) );
        centers.insert_back( offset );

        draw( GRASS_COLOR, Polygon( hill ).move( offset ) );

        center_x += Random::rPlanc( HILL_OFFSET_X ) * HILL_SCALE_X;
    }

    for_each( center, centers )
    {
        uint tree_count = Random::rPlanc( INNER_TREE_COUNT );
        do_count( tree_count )
        {
            Vector offset_vector = VectorA( Random::rAngle( ), Random::rPlanc( HILL_RADIUS ) );
            Coordinate offset_coordinate = Transform::stretch( VectorX( HILL_SCALE_X ) ).apply( offset_vector );
            offset_coordinate += VectorY( _base_bottom );
            offset_coordinate += center;

            draw( TREE_COLOR, Polygon( tree ).move( offset_coordinate ) );
        }

        uint top_tree_count = Random::rPlanc( TOP_TREE_COUNT );
        do_count( top_tree_count )
        {
            Vector offset_vector = VectorA( RIGHT + Random::negated( Random::rAngle( TOP_TREE_ANGLE ) ), HILL_RADIUS );
            Coordinate offset_coordinate = Transform::stretch( VectorX( HILL_SCALE_X ) ).apply( offset_vector );
            offset_coordinate += VectorY( _base_bottom );
            offset_coordinate += center;

            //draw( TREE_COLOR, Polygon( tree ).move( offset_coordinate ) );
        }
    }

    auto draw_tree_line = [ & ] ( Planc cref _base, Planc cref _penumbra, Color cref _color )
    {
        draw( _color, Polygon( { Coordinate( start_x, _base ),
                                  Coordinate( start_x, bottom ),
                                  Coordinate( end_x, bottom ),
                                  Coordinate( end_x, _base ) } ) );

        auto fill_tree_line = [ & ] ( Planc cref _y_raise, uint loops = 1 )
        {
            for_range( i, loops )
            {
                Planc center_x = start_x + Random::rPlanc( TREE_BASE );
                while( center_x <= end_x )
                {
                    center_x += Random::rPlanc( TREE_BASE );

                    Planc y = 0.0;
                    if( _y_raise )
                    {
                        y = _y_raise - ( log( Random::rPlanc( _y_raise ) + 1.0, _y_raise ) * _y_raise );
                    }

                    draw( _color, Polygon( tree ).move( Vector( center_x, _base + y ) ) );
                }
            }
        };

        fill_tree_line( 0.0 );
        fill_tree_line( TREE_LINE_PENUMBRA, TREE_LINE_PENUMBRA_PASSES );
    };

    draw_tree_line( _base_bottom - TREE_LINE_BASE, TREE_LINE_PENUMBRA, TREE_COLOR );
    //draw_tree_line( _base_bottom - TREE_LINE_BASE - TREE_LINE_PENUMBRA, TREE_LINE_PENUMBRA, TREE_2_COLOR );
}
