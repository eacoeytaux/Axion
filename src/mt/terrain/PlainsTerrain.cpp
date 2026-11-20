#include "mt/terrain/PlainsTerrain.hpp"

var_const( Planc DEPTH_COLOR_LENGTH = ( 400.0 ) ); // todo

var_const( Angle GRASS_MAX_ANGLE = ( Angle::fraction( 8.0 ) ) );

var_const( Planc GRASS_TUFTS_BASE = ( 5.0 ) );
var_const( Angle GRASS_TUFTS_MAX_ANGLE = ( Angle::fraction( 9.0 ) ) );
var_const( span<Planc> GRASS_TUFTS_BASE_LENGTH = { 9.0, 15.0 } );
var_const( span<Planc> GRASS_TUFTS_TIP_LENGTH = { 8.0, 10.0 } );
var_const( span<dec> GRASS_TUFTS_TIP_SWAY_RATIO = { 0.2, 0.4 } );
var_const( Planc GRASS_TUFTS_OUTLINE_THICKNESS = ( 3.0 ) );
var_const( Planc GRASS_CORNER_OVERHANG = ( 4.0 ) ); // C! how far the grass at the end of a grassy stretch reaches past the terrain
var_const( Angle GRASS_CORNER_LEAN = ( Angle::fraction( 8.0 ) ) ); // C! how far that grass's spike leans out from straight into the ground

// C! the dirt under the surface: DIRT_LAYERS bands, each darker and narrower than the one above it, then black in the middle
var_const( uint DIRT_LAYERS = ( 3 ) ); // C!
var_const( Planc DIRT_LAYER_WIDTH = ( 12.0 ) ); // C! the top band's width under an edge without grass
var_const( Planc DIRT_LAYER_WIDTH_GRASS = ( 24.0 ) ); // C! the top band's width under an edge with grass
var_const( dec DIRT_LAYER_SHRINK = ( 0.75 ) ); // C! each band's width, as a part of the one above it
var_const( dec DIRT_LAYER_MITER_LIMIT = ( 3.0 ) ); // C! how far a band's corner can be from its vertex, as a part of its depth there, so sharp corners don't spike

var_const( Planc BUSH_OFFSET = ( 10.0 ) );
var_const( span<Planc> BUSH_RADIUS_BACK = { 16.0, 42.0 } );
var_const( Planc BUSH_BACK_EXTRA_RADIUS = ( 5.0 ) );
var_const( span<Planc> BUSH_RADIUS_FRONT = { 7.0, 25.0 } );

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

    lambdaf( has_grass )( ptr<Terrain::Edge> in_edge ) { return( !is_null( in_edge ) && ( abs( in_edge->normal( ).min_distance( in_room->gravity( ).angle( ).flipped( ) ) ) < GRASS_MAX_ANGLE ) ); };

    for_each( edges, edges( ) )
    {
        for_each( edge, edges )
        {
            if( has_grass( edge ) )
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

                    bushes_back.insert_back( Polygon::circle( radius + BUSH_BACK_EXTRA_RADIUS, ( v * length ) ) );
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

    // C! the grass at the vertices: a spike where two grassy edges meet, filling the gap between theirs, and at the end of a grassy
    // C! stretch (the next edge has no grass, or there isn't one) a spike reaching a little past the terrain
    varray<Polygon> grass_corners;
    varray<Polygon> grass_corner_crops;

    for_each( vertices, vertices( ) )
    {
        for_each( vertex, vertices )
        {
            ptr<Terrain::Edge> edge1 = vertex->edge1( );
            ptr<Terrain::Edge> edge2 = vertex->edge2( );

            const bool c_grass1 = has_grass( edge1 );
            const bool c_grass2 = has_grass( edge2 );
            continue_if( !c_grass1 && !c_grass2 );

            const Point c_at = vertex->position( );
            const Planc c_tip_length = Random::rPlanc( GRASS_TUFTS_TIP_LENGTH );
            const Vector c_sway = VectorA( Random::rAngle( ), c_tip_length * Random::rPlanc( GRASS_TUFTS_TIP_SWAY_RATIO ) );

            if( c_grass1 && c_grass2 )
            {
                const Vector c_back = edge1->vector( ).normalized( ) * min( Random::rPlanc( GRASS_TUFTS_BASE_LENGTH ), half( edge1->vector( ).magnitude( ) ) );
                const Vector c_front = edge2->vector( ).normalized( ) * min( Random::rPlanc( GRASS_TUFTS_BASE_LENGTH ), half( edge2->vector( ).magnitude( ) ) );
                const Angle c_down = ( VectorA( edge1->normal( ), 1.0 ) + VectorA( edge2->normal( ), 1.0 ) ).angle( ).flipped( ); // C! between the two edges' spikes

                grass_front.insert_back( Polygon( { c_at - c_back, c_at + VectorA( c_down, c_tip_length ) + c_sway, c_at + c_front } ) );
            }
            else
            {
                ptr<Terrain::Edge> edge = ( c_grass1 ? edge1 : edge2 );

                const Vector c_along = edge->vector( ).normalized( );
                const Vector c_out = ( c_grass1 ? c_along : -c_along ); // C! along the edge, away from it
                const Planc c_base = min( Random::rPlanc( GRASS_TUFTS_BASE_LENGTH ), half( edge->vector( ).magnitude( ) ) );
                const Angle c_down = edge->normal( ).flipped( );
                const Angle c_tip_angle = ( c_grass1 ? ( c_down + GRASS_CORNER_LEAN ) : ( c_down - GRASS_CORNER_LEAN ) ); // C! leaning out, past the end

                const Point c_inner = c_at - ( c_out * c_base );
                const Point c_outer = c_at + ( c_out * GRASS_CORNER_OVERHANG );
                const Point c_tip = c_at + VectorA( c_tip_angle, c_tip_length ) + c_sway;

                grass_corners.insert_back( Polygon( c_grass1 ? varray<Point>( { c_inner, c_tip, c_outer } ) : varray<Point>( { c_outer, c_tip, c_inner } ) ) ); // C! in the edge's direction, like the other spikes

                // C! below the edge's line, out as far as the spike and its outline can reach, so the outline doesn't show above the ground
                const Planc c_reach = GRASS_TUFTS_BASE_LENGTH.max( ) + GRASS_CORNER_OVERHANG + ( GRASS_TUFTS_TIP_LENGTH.max( ) * ( 1.0 + GRASS_TUFTS_TIP_SWAY_RATIO.max( ) ) ) + GRASS_TUFTS_OUTLINE_THICKNESS;
                const Vector c_below = VectorA( c_down, c_reach );

                grass_corner_crops.insert_back( Polygon( { c_at - ( c_along * c_reach ), c_at + ( c_along * c_reach ), c_at + ( c_along * c_reach ) + c_below, c_at - ( c_along * c_reach ) + c_below } ) );
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

    fill_color( DIRT_COLOR_2 );

    // C! how far under in_edge the band in_layer starts (the black middle after the last one)
    lambdaf( dirt_depth )( ptr<Terrain::Edge> in_edge, uint in_layer )
    {
        Planc width = ( has_grass( in_edge ) ? DIRT_LAYER_WIDTH_GRASS : DIRT_LAYER_WIDTH );
        Planc depth = P0;

        do_count( in_layer ) { depth += width; width *= DIRT_LAYER_SHRINK; }

        return( depth );
    };

    // C! where a band's edge meets under in_at: where the edges each side of it, moved down to their depths, cross
    lambdaf( dirt_corner )( ptr<Terrain::Edge> in_before, ptr<Terrain::Edge> in_after, Point cref in_at, uint in_layer )
    {
        return_if( is_null( in_before ) && is_null( in_after ), in_at );

        if( is_null( in_before ) || is_null( in_after ) ) // C! the end of an open chain: straight down from it
        {
            ptr<Terrain::Edge> edge = ( is_null( in_before ) ? in_after : in_before );
            return( in_at + VectorA( edge->normal( ).flipped( ), dirt_depth( edge, in_layer ) ) );
        }

        const Planc c_depth_before = dirt_depth( in_before, in_layer );
        const Planc c_depth_after = dirt_depth( in_after, in_layer );

        const Point c_p1 = in_at + VectorA( in_before->normal( ).flipped( ), c_depth_before );
        const Point c_p2 = in_at + VectorA( in_after->normal( ).flipped( ), c_depth_after );
        const Vector c_d1 = in_before->vector( ).normalized( );
        const Vector c_d2 = in_after->vector( ).normalized( );

        const dec c_cross = ( dec( c_d1.dx( ) ) * dec( c_d2.dy( ) ) ) - ( dec( c_d1.dy( ) ) * dec( c_d2.dx( ) ) );
        return_if( is_zero( c_cross ), midpoint( c_p1, c_p2 ) ); // C! the edges carry straight on

        const dec c_t = ( ( dec( c_p2.x( ) - c_p1.x( ) ) * dec( c_d2.dy( ) ) ) - ( dec( c_p2.y( ) - c_p1.y( ) ) * dec( c_d2.dx( ) ) ) ) / c_cross;
        const Point c_crossing = c_p1 + ( c_d1 * c_t );

        const Planc c_deepest = max( c_depth_before, c_depth_after );
        return_if( ( in_at.distance_to( c_crossing ) <= ( c_deepest * DIRT_LAYER_MITER_LIMIT ) ), c_crossing );

        // C! too sharp a corner: straight down between the two edges instead
        return( in_at + VectorA( ( VectorA( in_before->normal( ), 1.0 ) + VectorA( in_after->normal( ), 1.0 ) ).angle( ).flipped( ), c_deepest ) );
    };

    for_range( chain, vertices( ).size( ) )
    {
        varray<ptr<Terrain::Vertex>> cref chain_vertices = vertices( )[ chain ];
        continue_if( ( chain >= terrain( ).size( ) ) || ( chain_vertices.size( ) <= 1 ) );

        const bool c_loop = ( edges( )[ chain ].size( ) == chain_vertices.size( ) );
        varray<Point> cref terrain_pts = terrain( )[ chain ].points( );

        for_range( i, DIRT_LAYERS )
        {
            const uint c_layer = i + 1; // C! the band under the top one, which is the fill

            varray<Point> pts;
            for_each_const( vertex, chain_vertices ) { pts.insert_back( dirt_corner( vertex->edge1( ), vertex->edge2( ), vertex->position( ), c_layer ) ); }

            if( !c_loop && ( terrain_pts.size( ) >= ( chain_vertices.size( ) + 2 ) ) ) // C! an open chain closes around the room's bounds, as its fill does
            {
                pts.insert_back( terrain_pts[ terrain_pts.size( ) - 2 ] );
                pts.insert_back( terrain_pts.back( ) );
            }

            draw( DIRT_COLOR_2.v( DIRT_COLOR_2.v( ) * one_minus( dec( c_layer ) / dec( DIRT_LAYERS ) ) ), Polygon( pts ) ); // C! black for the last
        }
    }

    for_each( poly, grass_back ) { draw( PLANT_GREEN_2, poly.expanded( GRASS_TUFTS_OUTLINE_THICKNESS ) ); }
    for_each( poly, grass_front ) { draw( PLANT_GREEN_2, poly.expanded( GRASS_TUFTS_OUTLINE_THICKNESS ) ); }
    for_each( poly, grass_front ) { draw( PLANT_GREEN_1, poly ); }

    crop( );

    // C! drawn after the crop, in a drawing of their own, so they can reach past the terrain
    Drawing corners;
    for_each( poly, grass_corners ) { corners.draw( PLANT_GREEN_2, poly.expanded( GRASS_TUFTS_OUTLINE_THICKNESS ) ); }
    for_each( poly, grass_corners ) { corners.draw( PLANT_GREEN_1, poly ); }
    for_each( poly, grass_corner_crops ) { corners.add_bounds( poly ); }
    draw( corners );
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

    return( CLEAR );
}
