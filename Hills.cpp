#include "Hills.hpp"
#include "World.hpp"

using mtmercy::Hills;

namespace
{
const Planc HILL_RADIUS = 1000.0;
const Planc HILL_SCALE_X = 2.0;
const Span<Planc> HILL_OFFSET_X = { HILL_RADIUS * 0.8, HILL_RADIUS * 1.75 };
const Span<Planc> HILL_OFFSET_Y = { ZERO, HILL_RADIUS * 0.25 };
const Planc TREE_HEIGHT = 225.0;
const Planc TREE_BASE = 150.0;
const Span<uint> INNER_TREE_COUNT = { 20, 30 };
const Span<uint> TOP_TREE_COUNT = { 8, 12 };
const Angle TOP_TREE_ANGLE = RIGHT_ANGLE / 1.5;
const Planc TREE_LINE_BASE = HILL_RADIUS;
const Planc TREE_LINE_PENUMBRA = TREE_LINE_BASE * 2.0;
const uint TREE_LINE_PENUMBRA_PASSES = 2;

const Color GRASS_COLOR = Color::rgb( 0x00EE00 );
const Color TREE_COLOR = Color::rgb( 0x00CC00 );
const Color TREE_2_COLOR = Color::rgb( 0x00AA00 );
}

Hills::Hills( World * world, const Planc _base_bottom, const dec _z ) : Object( world )
{
    background( true );
    
    persist_render( true );

    stationary( true );

    z( _z );
    
    Camera * camera = world->camera( );
    FixedRectangle bounds = world->bounds( );

    // todo what if camera specs change?
    Planc offset_z = ( z( ) * camera->min_zoom( ) );
    Planc offset_x = ( half( camera->width( ) ) / ( z( ) * camera->min_zoom( ) ) );
    Planc offset_y = ( half( camera->height( ) ) / ( z( ) * camera->min_zoom( ) ) );
    
    Planc start_x = bounds.lower_bound_x( ) - offset_x;
    Planc end_x = bounds.upper_bound_x( ) + offset_x;
    
    Planc bottom = bounds.lower_bound_y() - offset_y;
    
    draw( GRASS_COLOR, Polygon( { Coordinate( start_x, _base_bottom ),
                                  Coordinate( start_x, bottom ),
                                  Coordinate( end_x, bottom ),
                                  Coordinate( end_x, _base_bottom ) } ) );
    
    Polygon tree( { Coordinate( ZERO, TREE_HEIGHT ), Coordinate( -half( TREE_BASE ), ZERO ), Coordinate( half( TREE_BASE ), ZERO ) } );
    Polygon hill = Circle( HILL_RADIUS, Coordinate( ZERO, _base_bottom ) ).stretch( VectorX( HILL_SCALE_X ) );
    
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
        for_range( i, tree_count )
        {
            VectorA offset_vector( Random::rAngle( ), Random::rPlanc( HILL_RADIUS ) );
            Coordinate offset_coordinate = StretchTransform( VectorX( HILL_SCALE_X ) ).transform( offset_vector );
            offset_coordinate += VectorY( _base_bottom );
            offset_coordinate += center;
            
            draw( TREE_COLOR, Polygon( tree ).move( offset_coordinate ) );
        }
        
        uint top_tree_count = Random::rPlanc( TOP_TREE_COUNT );
        for_range( i, top_tree_count )
        {
            VectorA offset_vector( Random::rAngle( -TOP_TREE_ANGLE, TOP_TREE_ANGLE ) + RIGHT_ANGLE, HILL_RADIUS );
            Coordinate offset_coordinate = StretchTransform( VectorX( HILL_SCALE_X ) ).transform( offset_vector );
            offset_coordinate += VectorY( _base_bottom );
            offset_coordinate += center;
            
            //draw( TREE_COLOR, Polygon( tree ).move( offset_coordinate ) );
        }
    }
    
    auto draw_tree_line = [ & ]( const Planc & _base, const Planc & _penumbra, const Color & _color )
    {
        draw( _color, Polygon( { Coordinate( start_x, _base ),
                                  Coordinate( start_x, bottom ),
                                  Coordinate( end_x, bottom ),
                                  Coordinate( end_x, _base ) } ) );
        
        auto fill_tree_line = [ & ]( const Planc & _y_raise, uint loops = 1 )
        {
            for_range( i, loops )
            {
                Planc center_x = start_x + Random::rPlanc( TREE_BASE );
                while( center_x <= end_x )
                {
                    center_x += Random::rPlanc( TREE_BASE );
                    
                    Planc y = ZERO;
                    if ( _y_raise )
                    {
                        y = _y_raise - ( log_base( _y_raise, Random::rPlanc( _y_raise ) + ONE ) * _y_raise );
                    }
                    
                    draw( _color, Polygon( tree ).move( Vector( center_x, _base + y ) ) );
                }
            }
        };
        
        fill_tree_line( ZERO );
        fill_tree_line( TREE_LINE_PENUMBRA, TREE_LINE_PENUMBRA_PASSES );
    };
    
    draw_tree_line( _base_bottom - TREE_LINE_BASE, TREE_LINE_PENUMBRA, TREE_COLOR );
    //draw_tree_line( _base_bottom - TREE_LINE_BASE - TREE_LINE_PENUMBRA, TREE_LINE_PENUMBRA, TREE_2_COLOR );
}
