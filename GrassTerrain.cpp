#include "GrassTerrain.hpp"

using mtmercy::GrassTerrain;

namespace
{

cPlanc OUTLINE_THICKNESS = 3.0;

cPlanc DEPTH_COLOR_LENGTH = 400.0;

cPlanc GRASS_BASE = 5.0;
cAngle GLASS_MAX_ANGLE = ( TAU / 9.0 );
const Span<Planc> GRASS_BASE_LENGTH = { 9.0, 15.0 };
const Span<Planc> GRASS_TIP_LENGTH = { 8.0, 10.0 };
const Span<dec> GRASS_TIP_SWAY_RATIO = { 0.2, 0.4 };

cPlanc BUSH_OFFSET = 10.0;
const Span<Planc> BUSH_RADIUS_BACK = { 16.0, 42.0 };
const Span<Planc> BUSH_RADIUS_FRONT = { 7.0, 25.0 };

} // namespace

GrassTerrain::GrassTerrain( Room * room, const varray<varray<Coordinate>> & _vertices ) : Terrain( room, _vertices )
{
    persist_render( true );

    varray<Polygon> dirt_colored;
    varray<Polygon> dirt_black;

    varray<Polygon> grass_front;
    varray<Polygon> grass_back;

    varray<Polygon> bushes_front;
    varray<Polygon> bushes_back;

    Planc world_bottom = room->bounds( ).lower_bound_y( );

    for_each( edges, edges( ) )
    {
        for_each( edge, edges )
        {
            // if( abs( edge->line( ).angle( ).radians( ) ) <= GLASS_MAX_ANGLE.radians( ) )
            {
                Coordinate v1 = edge->vertex1( )->position( );
                Coordinate v2 = edge->vertex2( )->position( );
                Vector v( v1, v2 );

                Vector v_right = v;
                v_right.rotate( -RIGHT );
                v_right.normalize( );

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
                    Planc radius = Random::rPlanc( BUSH_RADIUS_FRONT );
                    bushes_back.insert_back( Polygon::circle( radius + 5.0, ( v * length ) ) );
                    bushes_front.insert_back( Polygon::circle( radius, ( v * length ) ) );
                    length += radius;
                }

                dirt_colored.insert_back(
                    Polygon( { v2,
                               v1,
                               v1 + Vector( 0, -DEPTH_COLOR_LENGTH ),
                               v2 + Vector( 0, -DEPTH_COLOR_LENGTH ) } ) );

                dirt_black.insert_back(
                    Polygon( { v2 + Vector( 0, -DEPTH_COLOR_LENGTH ),
                               v1 + Vector( 0, -DEPTH_COLOR_LENGTH ),
                               v1.y( world_bottom ),
                               v2.y( world_bottom ) } ) );

                auto generate_grass = [ & ] ( varray<Polygon> & grass )
                {
                    Vector v = edge->vector( );
                    Planc magnitude = v.magnitude( );
                    Angle normal = edge->normal( ).flipped( );

                    Planc tip_length = Random::rPlanc( GRASS_TIP_LENGTH );
                    Planc base_length = Random::rPlanc( GRASS_BASE_LENGTH );
                    Planc d = base_length;

                    Vector base_offset = VectorA( normal, GRASS_BASE );

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
                        v4.rotate( -RIGHT );

                        Vector v5 = v4 + VectorA( Random::rAngle( ), tip_length * Random::rPlanc( GRASS_TIP_SWAY_RATIO ) );

                        Vector v6 = v1 + v3;
                        if( !first ) { v6 += base_offset; }

                        Vector v7 = v1 + v2;
                        if( !last ) { v7 += base_offset; }

                        Vector v8 = v1 + v5 + base_offset;

                        grass.insert_back( Polygon( { v1 + v2,
                                                      v1 + v3,
                                                      v6,
                                                      v8,
                                                      v7 } ) );

                        if( last ) { break; }
                        first = false;

                        tip_length = Random::rPlanc( GRASS_TIP_LENGTH );
                        base_length = Random::rPlanc( GRASS_BASE_LENGTH );
                        Planc dd = Random::rPlanc( GRASS_BASE_LENGTH.min( ), base_length );

                        if( magnitude - ( d + dd ) < GRASS_BASE_LENGTH.min( ) )
                        {
                            last = true;
                            dd = magnitude - d - GRASS_BASE_LENGTH.min( );
                            base_length = magnitude - ( d + dd );
                        }

                        d += dd;
                    }
                };

                generate_grass( grass_front );
                generate_grass( grass_back );
            }
        }
    }

    for_each( vertices, vertices( ) )
    {
        for_each( vertex, vertices )
        {
            if( !vertex->edge1( ) || !vertex->edge2( ) )
            {
                continue;
            }

            Coordinate base = vertex->position( );
            Angle normal = vertex->normal( ).flipped( );

            Planc tip_length = Random::rPlanc( GRASS_TIP_LENGTH );
            Vector offset = VectorA( Random::rAngle( ), tip_length * Random::rPlanc( GRASS_TIP_SWAY_RATIO ) );
            Coordinate tip = base + VectorA( normal, tip_length ) + offset;

            Coordinate intersect1 = base;
            Coordinate intersect2 = base;

            Angle base_offset = normal;

            Angle base_offset1 = base_offset;
            Angle base_offset2 = base_offset;

            if( TerrainEdge * edge = vertex->edge1( ) )
            {
                Line edge_line = edge->line( );

                Vector edge_intersect = VectorA( normal - ( RIGHT * 1.5 ), tip );
                intersect1 = edge_line.intersection( Line( edge_intersect ) );

                base_offset1 = edge->normal( ).flipped( );
            }

            if( TerrainEdge * edge = vertex->edge2( ) )
            {
                Line edge_line = edge->line( );

                Vector edge_intersect = VectorA( normal + ( RIGHT * 1.5 ), tip );
                intersect2 = edge_line.intersection( Line( edge_intersect ) );

                base_offset2 = edge->normal( ).flipped( );
            }

            Polygon grass = Polygon( { base,
                                       intersect1,
                                       intersect1 + VectorA( base_offset1, GRASS_BASE ),
                                       tip + VectorA( base_offset, GRASS_BASE ),
                                       intersect2 + VectorA( base_offset2, GRASS_BASE ),
                                       intersect2 } );

            grass_front.insert_back( grass );
            grass_back.insert_back( grass );
        }
    }

    for_each( polygon, bushes_back ) draw( Colors::PLANT_GREEN_4, Polygon::expand( polygon, OUTLINE_THICKNESS ) );
    for_each( polygon, bushes_front ) draw( Colors::PLANT_GREEN_4, Polygon::expand( polygon, OUTLINE_THICKNESS ) );
    for_each( polygon, bushes_front ) draw( Colors::PLANT_GREEN_3, Polygon::expand( polygon, OUTLINE_THICKNESS ) );

    for_each( polygon, dirt_black ) draw( BLACK, polygon );
    for_each( polygon, dirt_colored ) draw( { Colors::DIRT_COLOR, Colors::DIRT_COLOR, BLACK, BLACK }, polygon );

    for_each( polygon, dirt_colored ) bind( polygon );

    for_each( polygon, grass_back ) draw( Colors::PLANT_GREEN_2, Polygon::expand( polygon, OUTLINE_THICKNESS ) );
    for_each( polygon, grass_front ) draw( Colors::PLANT_GREEN_2, Polygon::expand( polygon, OUTLINE_THICKNESS ) );
    for_each( polygon, grass_front ) draw( Colors::PLANT_GREEN_1, polygon );

    clear_bounds( );
}

Color cref GrassTerrain::dust_color( ) const
{
    return Colors::DIRT_COLOR;
}
