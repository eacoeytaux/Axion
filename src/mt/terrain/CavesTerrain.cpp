#include "mt/terrain/CavesTerrain.hpp"

#include "mt/environment/rocks/Stalaite.hpp"

var_const( Angle HANG_ANGLE = Angle::fraction( 6.0 ) );
var_const( Angle FLOOR_ANGLE = Angle::fraction( 8.0 ) );

var_const( uint STALACTITE_PASSES = ( 1 ) );
var_const( span<Planc> STALACTITE_WIDTH = { 20.0, 40.0 } );
var_const( span<dec> STALACTITE_HEIGHT_RATIO = { 0.75, 1.75 } );

var_const( uint STALAGMITE_PASSES = ( 2 ) );
var_const( span<Planc> STALAGMITE_WIDTH = { 7.5, 17.5 } );
var_const( span<dec> STALAGMITE_HEIGHT_RATIO = { 0.35, 1.15 } );
var_const( span<Planc> STALAGMITE_GAP = { half( STALAGMITE_WIDTH.min( ) ), ( STALAGMITE_WIDTH.max( ) * 7.5 ) } );

CavesTerrain::CavesTerrain( ptr<Room> in_room, vvarray<Point> cref in_vertices ) : Terrain( in_room, in_vertices )
{
    fill_color( GRAY_DARK );

    for_each( edges, edges( ) )
    {
        for_each( edge, edges )
        {
            if( edge->normal( ).in_range( in_room->gravity( ).angle( ), HANG_ANGLE ) )
            {
                do_count( STALACTITE_PASSES )
                {
                    Line cref line = edge->line( );
                    Planc x = line.lower_bound_x( ) - Random::rPlanc( STALACTITE_WIDTH.min( ) );

                    while( x <= ( line.upper_bound_x( ) + STALACTITE_WIDTH.min( ) ) )
                    {
                        Planc width = Random::rPlanc( STALACTITE_WIDTH );
                        Planc height = width * Random::rdec( STALACTITE_HEIGHT_RATIO );

                        in_room->add_object( new Stalactite( in_room, fill_color( ), Point( x, line.y( x ) ), width, height, Random::rbool( 0.1 ) ) );

                        x += Random::rPlanc( half( width ) ) + half( STALACTITE_WIDTH.min( ) );
                    }
                }
            }
            else if( edge->normal( ).in_range( in_room->gravity( ).angle( ).flipped( ), FLOOR_ANGLE ) ) // todo
            {
                do_count( STALAGMITE_PASSES )
                {
                    Line cref line = edge->line( );
                    Planc x = line.lower_bound_x( ) - Random::rPlanc( STALAGMITE_WIDTH.min( ) );

                    while( x <= ( line.upper_bound_x( ) + STALAGMITE_WIDTH.min( ) ) )
                    {
                        Planc width = Random::rPlanc( STALAGMITE_WIDTH );
                        Planc height = width * Random::rdec( STALAGMITE_HEIGHT_RATIO );

                        in_room->add_object( new Stalagmite( in_room, fill_color( ), Point( x, line.y( x ) ), width, height ) );

                        x += Random::rPlanc( half( width ) ) + Random::rPlanc( STALAGMITE_GAP );
                    }
                }
            }
        }
    }

    //traverse_x( span<Planc>( 25.0, 500.0 ), [ & ] ( Point cref in_pt, ptr<const Terrain::Edge> in_edge )
    //{
    //    if( in_edge->normal( ).in_range( in_room->gravity( ).angle( ), HANG_ANGLE ) )
    //    {
    //        in_room->add_object( new Stalactite( in_room, fill_color( ), in_pt, 24.0, 50.0 ) );
    //    }
    //} );

    //crop( );
}
