#include "mt/background/Hills.hpp"

var_const( Planc HILL_RADIUS = ( 1000.0 ) );
var_const( Planc HILL_SCALE_X = ( 2.0 ) );
var_const( Span<Planc> HILL_OFFSET_X = { HILL_RADIUS * 0.8, HILL_RADIUS * 1.75 } );
var_const( Span<Planc> HILL_OFFSET_Y = { 0.0, HILL_RADIUS * 0.25 } );
var_const( Planc TREE_HEIGHT = ( 225.0 ) );
var_const( Planc TREE_BASE = ( 150.0 ) );
var_const( Span<uint> INNER_TREE_COUNT = { 20, 30 } );
var_const( Span<uint> TOP_TREE_COUNT = { 8, 12 } );
var_const( Angle TOP_TREE_ANGLE = ( Angle::fraction( 6.0 ) ) );
var_const( Planc TREE_LINE_BASE = ( HILL_RADIUS ) );
var_const( Planc TREE_LINE_PENUMBRA = ( TREE_LINE_BASE * 2.0 ) );
var_const( uint TREE_LINE_PENUMBRA_PASSES = ( 2 ) );

var_const( Color GRASS_COLOR = ( Color::rgb( 0x00EE00 ) ) );
var_const( Color TREE_COLOR_2 = ( Color::rgb( 0x00CC00 ) ) );
var_const( Color TREE_COLOR_1 = ( Color::rgb( 0x00AA00 ) ) );

Hills::Hills( ptr<Room> in_room, Planc cref in_base_bottom, dec in_z ) : Object( in_room, in_z )
{
    background( true );

    persist_render( true );

    stationary( true );

    Planc lo_x = ( bounds( ).lower_bound_x( ) );
    Planc hi_x = ( bounds( ).upper_bound_x( ) );

    Planc bottom = ( bounds( ).lower_bound_y( ) );

    Polygon tree( { PointY( TREE_HEIGHT ), PointX( -half( TREE_BASE ) ), PointX( half( TREE_BASE ) ) } );
    Polygon hill = Polygon::circle( HILL_RADIUS, PointY( in_base_bottom ) ).stretch( VectorX( HILL_SCALE_X ) );

    varray<Vector> centers = { };

    Planc center_x = lo_x;
    while( center_x < hi_x )
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
            Point offset_point = Transform::stretch( VectorX( HILL_SCALE_X ) ).apply( offset_vector );
            offset_point += VectorY( in_base_bottom );
            offset_point += center;

            draw( TREE_COLOR_1, Polygon( tree ).move( offset_point ) );
        }

        uint top_tree_count = Random::rPlanc( TOP_TREE_COUNT );
        do_count( top_tree_count )
        {
            Vector offset_vector = VectorA( RIGHT + Random::negated( Random::rAngle( TOP_TREE_ANGLE ) ), HILL_RADIUS );
            Point offset_point = Transform::stretch( VectorX( HILL_SCALE_X ) ).apply( offset_vector );
            offset_point += VectorY( in_base_bottom );
            offset_point += center;

            // draw( TREE_COLOR_1, Polygon( tree ).move( offset_point ) );
        }
    }

    lambdaf( draw_tree_line )( Planc cref in_base, Planc cref in_penumbra, Color cref in_color )
    {
        draw( in_color, Polygon( { Point( lo_x, in_base ),
                                   Point( lo_x, bottom ),
                                   Point( hi_x, bottom ),
                                   Point( hi_x, in_base ) } ) );

        lambdaf( fill_tree_line )( Planc cref in_y_raise, uint in_loops = 1 )
        {
            for_range( i, in_loops )
            {
                Planc center_x = lo_x + Random::rPlanc( TREE_BASE );
                while( center_x <= hi_x )
                {
                    center_x += Random::rPlanc( TREE_BASE );

                    Planc y = 0.0;
                    if( in_y_raise )
                    {
                        y = in_y_raise - ( log( Random::rPlanc( in_y_raise ) + 1.0, in_y_raise ) * in_y_raise );
                    }

                    draw( in_color, Polygon( tree ).move( Vector( center_x, in_base + y ) ) );
                }
            }
        };

        fill_tree_line( 0.0 );
        fill_tree_line( TREE_LINE_PENUMBRA, TREE_LINE_PENUMBRA_PASSES );
    };

    draw_tree_line( ( in_base_bottom - TREE_LINE_BASE ), TREE_LINE_PENUMBRA, TREE_COLOR_1 );
    // draw_tree_line( ( in_base_bottom - TREE_LINE_BASE - TREE_LINE_PENUMBRA ), TREE_LINE_PENUMBRA, TREE_COLOR_2 );
}
