#include "mt/background/Cloud.hpp"

#include "axn/world/Player.hpp"

var_const( dec WIND_RATIO = ( 5.0 ) );

var_const( dec PUFF_OUTLINE_RATIO = ( 0.64 ) );

var_const( dec X_STRETCH_LARGE = ( 2.5 ) );
var_const( Span<uint> LARGE_PUFF_COUNT = { 16, 32 } );
var_const( Span<Planc> LARGE_PUFF_RADIUS = { 210.0, 720.0 } );
var_const( Span<Planc> LARGE_PUFF_DISTANCE = { 240.0, 600.0 } );

var_const( dec X_STRETCH_SMALL = ( 1.5 ) );
var_const( Span<uint> SMALL_PUFF_COUNT = { 1, 3 } );
var_const( Span<Planc> SMALL_PUFF_RADIUS = { 20.0, 90.0 } );
var_const( Span<Planc> SMALL_PUFF_DISTANCE = { ( LARGE_PUFF_RADIUS.max( ) * PUFF_OUTLINE_RATIO ) + SMALL_PUFF_RADIUS.max( ), ( LARGE_PUFF_RADIUS.max( ) * PUFF_OUTLINE_RATIO ) + x2( SMALL_PUFF_RADIUS.max( ) ) } );

var_const( Color INSIDE_COLOR = ( Color::rgb( 0x87CDEB ) ) );
var_const( Color OUTSIDE_COLOR = ( WHITE ) );

Cloud::Cloud( ptr<Room> in_room ) : Object( in_room )
{
    background( true );

    needs_render_always( true );

    z( 0.1 );
    no_gravity( );
    terrain_bound( false );

    dec scale = Random::rdec( 0.5, 1.0 );

    varray<Circle> large_puffs;
    varray<Circle> small_puffs;

    do_count( Random::rint( LARGE_PUFF_COUNT ) )
    {
        Circle ref puff = large_puffs.insert_back( );

        Point offset = VectorA( Random::rAngle( ), Random::rdec( LARGE_PUFF_DISTANCE ) );

        offset.x( offset.x( ) * X_STRETCH_LARGE );
        offset *= scale;

        puff.center( offset );
        puff.radius( Random::rPlanc( LARGE_PUFF_RADIUS ) * scale );

        do_count( Random::rint( SMALL_PUFF_COUNT ) )
        {
            Circle ref spuff = small_puffs.insert_back( );

            Point offset = puff.center( ) + VectorA( Random::rAngle( ), Random::rdec( SMALL_PUFF_DISTANCE ) );

            offset.x( offset.x( ) * X_STRETCH_SMALL );
            offset *= scale;

            spuff.center( offset );
            spuff.radius( Random::rPlanc( SMALL_PUFF_RADIUS ) * scale );
        }
    }

    for_each( puff, small_puffs ) m_cloud_drawing.draw( OUTSIDE_COLOR, puff, FILLED );
    for_each( puff, large_puffs ) m_cloud_drawing.draw( OUTSIDE_COLOR, puff, FILLED );
    for_each( puff, large_puffs ) m_cloud_drawing.draw( INSIDE_COLOR, puff * PUFF_OUTLINE_RATIO, FILLED );
}

Planc Cloud::max_dx( ) { return( ( LARGE_PUFF_RADIUS.max( ) + LARGE_PUFF_DISTANCE.max( ) ) * X_STRETCH_LARGE ) + ( ( SMALL_PUFF_RADIUS.max( ) + SMALL_PUFF_DISTANCE.max( ) ) * X_STRETCH_SMALL ); }
Planc Cloud::max_dy( ) { return( LARGE_PUFF_RADIUS.max( ) + LARGE_PUFF_DISTANCE.max( ) + SMALL_PUFF_RADIUS.max( ) + SMALL_PUFF_DISTANCE.max( ) ); }

void Cloud::render( )
{
    Object::render( );

    draw( m_cloud_drawing );
}

void Cloud::update( )
{
    velocity( room( )->wind( ) * WIND_RATIO );

    Object::update( );
}
