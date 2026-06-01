#include "mt/terrain/PlainsTerrain.hpp"

var_const( Planc DEPTH_COLOR_LENGTH = ( 400.0 ) ); // todo

var_const( Angle GRASS_MAX_ANGLE = ( Angle::fraction( 8.0 ) ) );

var_const( Planc GRASS_TUFTS_BASE = ( 5.0 ) );
var_const( Angle GRASS_TUFTS_MAX_ANGLE = ( Angle::fraction( 9.0 ) ) );
var_const( Span<Planc> GRASS_TUFTS_BASE_LENGTH = { 9.0, 15.0 } );
var_const( Span<Planc> GRASS_TUFTS_TIP_LENGTH = { 8.0, 10.0 } );
var_const( Span<dec> GRASS_TUFTS_TIP_SWAY_RATIO = { 0.2, 0.4 } );
var_const( Planc GRASS_TUFTS_OUTLINE_THICKNESS = ( 3.0 ) );

var_const( Planc BUSH_OFFSET = ( 10.0 ) );
var_const( Span<Planc> BUSH_RADIUS_BACK = { 16.0, 42.0 } );
var_const( Span<Planc> BUSH_RADIUS_FRONT = { 7.0, 25.0 } );

PlainsTerrain::PlainsTerrain( ptr<Room> in_room, vvarray<Point> cref in_vertices ) : Terrain( in_room, in_vertices )
{
    persist_render( true );

    varray<Polygon> dirt_colored;
    varray<Polygon> dirt_black;

    varray<Polygon> grass_front;
    varray<Polygon> grass_back;

    varray<Polygon> bushes_front;
    varray<Polygon> bushes_back;

    Planc world_bottom = in_room->bounds( ).lower_bound_y( );

    for_each( edges, edges( ) )
    {
        for_each( edge, edges )
        {
            if( abs( edge->normal( ).min_distance( in_room->gravity( ).angle( ).flipped( ) ) ) < GRASS_MAX_ANGLE )
            {
                Point v1 = edge->vertex1( )->position( );
                Point v2 = edge->vertex2( )->position( );

                Vector v( v1, v2 );

                Angle normal = edge->normal( );

                Vector v_right = v.rotated_right( false ).normalized( );

                Planc length;
                Planc max_length = v.magnitude( );

                v.normalize( );

                length = 0.0;
                while( length < max_length )
                {
                    Planc radius = Random::rPlanc( BUSH_RADIUS_BACK );

                    bushes_back.insert_back( Polygon::circle( radius, ( v * length ) + VectorY( BUSH_OFFSET ) ) );

                    length += radius;
                }

                length = 0.0;
                while( length < max_length )
                {
                    Planc radius = Random::rPlanc( BUSH_RADIUS_BACK );

                    bushes_back.insert_back( Polygon::circle( radius + 5.0, ( v * length ) ) ); // todo
                    bushes_front.insert_back( Polygon::circle( radius, ( v * length ) ) );

                    length += radius;
                }

                dirt_colored.insert_back(
                    Polygon( { v2,
                               v1,
                               v1 + VectorA( normal.flipped( ), DEPTH_COLOR_LENGTH ),
                               v2 + VectorA( normal.flipped( ), DEPTH_COLOR_LENGTH ) } ) );

                dirt_black.insert_back(
                    Polygon( { v2 + Vector( 0, -DEPTH_COLOR_LENGTH ),
                               v1 + Vector( 0, -DEPTH_COLOR_LENGTH ),
                               v1.y( world_bottom ),
                               v2.y( world_bottom ) } ) );

                lambdaf( generate_grass )( varray<Polygon> ref in_grass )
                {
                    Vector v = edge->vector( );
                    Planc magnitude = v.magnitude( );
                    Angle normal = edge->normal( ).flipped( );

                    Planc tip_length = Random::rPlanc( GRASS_TUFTS_TIP_LENGTH );
                    Planc base_length = Random::rPlanc( GRASS_TUFTS_BASE_LENGTH );
                    Planc d = base_length;

                    Vector base_offset = VectorA( normal, P0 ); // GRASS_TUFTS_BASE );

                    bool first = true;
                    bool last = false;

                    do_until_break( )
                    {
                        Vector v1 = v;
                        v1.magnitude( d );

                        Vector v2 = v;
                        v2.magnitude( base_length );

                        Vector v3 = -v2;

                        Vector v4 = v;
                        v4.magnitude( tip_length );
                        v4.rotate_right( false );

                        Vector v5 = v4 + VectorA( Random::rAngle( ), tip_length * Random::rPlanc( GRASS_TUFTS_TIP_SWAY_RATIO ) );

                        Vector v6 = v1 + v3;
                        if( !first ) { v6 += base_offset; }

                        Vector v7 = v1 + v2;
                        if( !last ) { v7 += base_offset; }

                        Vector v8 = v1 + v5 + base_offset;

                        in_grass.insert_back( Polygon( { v6, v8, v7 } ) );

                        break_if( last );
                        first = false;

                        tip_length = Random::rPlanc( GRASS_TUFTS_TIP_LENGTH );
                        base_length = Random::rPlanc( GRASS_TUFTS_BASE_LENGTH );
                        Planc dd = Random::rPlanc( GRASS_TUFTS_BASE_LENGTH.min( ), base_length );

                        if( ( magnitude - ( d + dd ) ) < GRASS_TUFTS_BASE_LENGTH.min( ) )
                        {
                            last = true;
                            dd = magnitude - d - GRASS_TUFTS_BASE_LENGTH.min( );
                            base_length = magnitude - ( d + dd );
                        }

                        d += dd;
                    }
                };

                generate_grass( grass_front );
                // generate_grass( grass_back );
            }
        }
    }

    //for_each( vertices, vertices( ) )
    //{
    //    for_each( vertex, vertices )
    //    {
    //        continue_if( !vertex->edge1( ) || !vertex->edge2( ) );

    //        Point base = vertex->position( );
    //        Angle normal = vertex->normal( ).flipped( );

    //        Planc tip_length = Random::rPlanc( GRASS_TUFTS_TIP_LENGTH );
    //        Vector offset = VectorA( Random::rAngle( ), tip_length * Random::rPlanc( GRASS_TUFTS_TIP_SWAY_RATIO ) );
    //        Point tip = base + VectorA( normal, tip_length ) + offset;

    //        Point intersect1 = base;
    //        Point intersect2 = base;

    //        Angle base_offset = normal;

    //        Angle base_offset1 = base_offset;
    //        Angle base_offset2 = base_offset;

    //        if( ptr<Terrain::Edge> edge = vertex->edge1( ) )
    //        {
    //            Line edge_line = edge->line( );

    //            Vector edge_intersect = VectorA( normal - ( RIGHT * 1.5 ), tip );
    //            intersect1 = edge_line.intersection( Line( edge_intersect ) );

    //            base_offset1 = edge->normal( ).flipped( );
    //        }

    //        if( ptr<Terrain::Edge> edge = vertex->edge2( ) )
    //        {
    //            Line edge_line = edge->line( );

    //            Vector edge_intersect = VectorA( normal + ( RIGHT * 1.5 ), tip );
    //            intersect2 = edge_line.intersection( Line( edge_intersect ) );

    //            base_offset2 = edge->normal( ).flipped( );
    //        }

    //        Polygon grass = Polygon( { base,
    //                                   intersect1,
    //                                   intersect1 + VectorA( base_offset1, GRASS_TUFTS_BASE ),
    //                                   tip + VectorA( base_offset, GRASS_TUFTS_BASE ),
    //                                   intersect2 + VectorA( base_offset2, GRASS_TUFTS_BASE ),
    //                                   intersect2 } );

    //        grass_front.insert_back( grass );
    //        grass_back.insert_back( grass );
    //    }
    //}

    // for_each( polygon, bushes_back ) draw( PLANT_GREEN_4, polygon.expanded( GRASS_TUFTS_OUTLINE_THICKNESS ) );
    // for_each( polygon, bushes_front ) draw( PLANT_GREEN_4, polygon.expanded( GRASS_TUFTS_OUTLINE_THICKNESS ) );
    // for_each( polygon, bushes_front ) draw( PLANT_GREEN_3, polygon.expanded( GRASS_TUFTS_OUTLINE_THICKNESS ) );

    // for_each( polygon, dirt_black ) draw( BLACK, polygon );
    // for_each( polygon, dirt_colored ) draw( { DIRT_COLOR, Colors::DIRT_COLOR, BLACK, BLACK }, polygon );

    fill_terrain( DIRT_COLOR_2 );

    for_each( polygon, grass_back ) draw( PLANT_GREEN_2, polygon.expanded( GRASS_TUFTS_OUTLINE_THICKNESS ) );
    for_each( polygon, grass_front ) draw( PLANT_GREEN_2, polygon.expanded( GRASS_TUFTS_OUTLINE_THICKNESS ) );
    for_each( polygon, grass_front ) draw( PLANT_GREEN_1, polygon );

    crop_terrain( );
}

bool PlainsTerrain::grassy( const uint in_type ) const
{
    return( true );
}

bool PlainsTerrain::creates_dust( const uint in_type ) const
{
    return( grassy( in_type ) );
}

Color PlainsTerrain::dust_color( const uint in_type ) const
{
    if( grassy( in_type ) )
    {
        return( DIRT_COLOR_1 );
    }
}