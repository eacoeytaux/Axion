#include "GrassTerrain.hpp"

using mtmercy::GrassTerrain;

namespace
{
cPlanc OUTLINE_THICKNESS = 3.0;
cPlanc DEPTH_LENGTH = 1000.0; // todo this should be more variable
cPlanc DEPTH_COLOR_LENGTH = 400.0;

const Angle GLASS_MAX_ANGLE = PI / 6.0;
cPlanc GRASS_BASE = 2.0;
const Span<Planc> GRASS_RADIUS = { 13.0, 17. };
const Span<Planc> GRASS_TIP_SWAY = { 0.25, 0.4 };

const Color GRASS_COLOR_LIGHT = Color::rgb( 0x00C000 );
const Color GRASS_COLOR_DARK = Color::rgb( 0x008000 );
const Color DIRT_COLOR = Color::rgb( 0x803000 );
const Color DUST_COLOR = DIRT_COLOR;
} // namespace

GrassTerrain::GrassTerrain( World * world, const varray<varray<Coordinate>> & _vertices ) : Terrain( world, _vertices )
{
    persist_render( true );

    varray<Polygon> grass_top;
    varray<Polygon> grass_bottom;

    for_each( edges, edges( ) )
    {
        for_each( edge, edges )
        {
            // TODO
            if( abs( edge->line( ).angle( ).radians( ) ) <= GLASS_MAX_ANGLE.radians( ) )
            {
                Coordinate v1 = edge->vertex1( )->position( );
                Coordinate v2 = edge->vertex2( )->position( );

                draw( { DIRT_COLOR, DIRT_COLOR, BLACK, BLACK },
                      Polygon( { v2,
                                 v1,
                                 v1 + Vector( 0, -DEPTH_COLOR_LENGTH ),
                                 v2 + Vector( 0, -DEPTH_COLOR_LENGTH ) } ) );

                draw( BLACK,
                      Polygon( { v2 + Vector( 0, -DEPTH_COLOR_LENGTH ),
                                 v1 + Vector( 0, -DEPTH_COLOR_LENGTH ),
                                 v1 + Vector( 0, -DEPTH_LENGTH ),
                                 v2 + Vector( 0, -DEPTH_LENGTH ) } ) );

                auto generate_grass = [ & ] ( varray<Polygon> & grass )
                {
                    Vector v = edge->vector( );
                    Planc magnitude = v.magnitude( );
                    Angle normal = edge->normal( ).flipped( );

                    Planc radius = Random::rPlanc( GRASS_RADIUS );
                    Planc d = radius;

                    Vector base_offset = Vector::A( normal, GRASS_BASE );

                    bool last = false;
                    do_until_break( )
                    {
                        Vector v1 = v;
                        v1.magnitude( d );

                        Vector v2 = v;
                        v2.magnitude( radius );

                        Vector v3 = -v2;

                        Vector v4 = v2;
                        v4.rotate( -RIGHT_ANGLE );

                        Vector v5 = v4 + Vector::A( Random::rAngle( ), radius * Random::rPlanc( GRASS_TIP_SWAY ) );

                        grass.insert_back( Polygon( { v1 + v2,
                                                      v1 + v3,
                                                      v1 + v3 + base_offset,
                                                      v1 + v5 + base_offset,
                                                      v1 + v2 + base_offset } ) );

                        if( last )
                        {
                            break;
                        }
                        else
                        {
                            radius = Random::rPlanc( GRASS_RADIUS );
                            Planc dd = Random::rPlanc( GRASS_RADIUS.min( ), radius );

                            if( magnitude - ( d + dd ) < GRASS_RADIUS.min( ) )
                            {
                                last = true;
                                dd = magnitude - d - GRASS_RADIUS.min( );
                                radius = magnitude - ( d + dd );
                            }

                            d += dd;
                        }
                    }
                };

                generate_grass( grass_top );
                generate_grass( grass_bottom );
            }
        }
    }

    for_each( vertices, vertices( ) )
    {
        for_each( vertex, vertices )
        {
            if( !vertex->edge1( ) || !vertex->edge2( ) )
                continue;

            Coordinate base = vertex->position( );
            Angle normal = vertex->normal( );

            Planc radius = Random::rPlanc( GRASS_RADIUS );
            Vector offset = Vector::A( Random::rAngle( ), radius * Random::rPlanc( GRASS_TIP_SWAY ) );
            Coordinate tip = base + Vector::A( normal, radius, offset );

            Coordinate intersect1 = base;
            Coordinate intersect2 = base;

            Angle base_offset = normal;

            Angle base_offset1 = base_offset;
            Angle base_offset2 = base_offset;

            if( TerrainEdge * edge = vertex->edge1( ) )
            {
                Line edge_line = edge->line( );

                Vector edge_intersect = Vector::A( normal - ( RIGHT_ANGLE * 1.5 ), tip );
                intersect1 = edge_line.intersection( Line( edge_intersect ) );

                base_offset1 = edge->normal( ).flipped( );
            }

            if( TerrainEdge * edge = vertex->edge2( ) )
            {
                Line edge_line = edge->line( );

                Vector edge_intersect = Vector::A( normal + ( RIGHT_ANGLE * 1.5 ), tip );
                intersect2 = edge_line.intersection( Line( edge_intersect ) );

                base_offset2 = edge->normal( ).flipped( );
            }

            grass_top.insert_back( Polygon( { base,
                                              intersect1,
                                              intersect1 + Vector::A( base_offset1, GRASS_BASE ),
                                              tip + Vector::A( base_offset, GRASS_BASE ),
                                              intersect2 + Vector::A( base_offset2, GRASS_BASE ),
                                              intersect2 } ) );

            grass_bottom.insert_back( Polygon( { base,
                                                 intersect1,
                                                 intersect1 + Vector::A( base_offset1, GRASS_BASE ),
                                                 tip + Vector::A( normal, OUTLINE_THICKNESS ) + Vector::A( base_offset, GRASS_BASE ),
                                                 intersect2 + Vector::A( base_offset2, GRASS_BASE ),
                                                 intersect2 } ) );
        }
    }

    for_each( polygon, grass_bottom ) draw( GRASS_COLOR_DARK, Polygon::expand( polygon, OUTLINE_THICKNESS ) );
    for_each( polygon, grass_top ) draw( GRASS_COLOR_DARK, Polygon::expand( polygon, OUTLINE_THICKNESS ) );
    for_each( polygon, grass_top ) draw( GRASS_COLOR_LIGHT, polygon );
}

Color cref GrassTerrain::dust_color( ) const
{
    return DUST_COLOR;
}
