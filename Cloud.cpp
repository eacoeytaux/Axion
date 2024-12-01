#include "Cloud.hpp"
#include "World.hpp"
#include "Player.hpp"

using mtmercy::Cloud;

namespace
{
const dec PUFF_OUTLINE_RATIO = 0.64;
const Color INSIDE_COLOR = Color::rgb( 0x87CDEB );
const Color OUTSIDE_COLOR = WHITE;

const dec SCALE = 10.0;
const dec WIND_SCALE = 5.0;

const dec X_STRETCH_LARGE = 2.5;
const Span<uint> LARGE_PUFF_COUNT = { 16, 32 };
const Span<Planc> LARGE_PUFF_RADIUS = { 21.0 * SCALE, 72.0 * SCALE };
const Span<Planc> LARGE_PUFF_DISTANCE = { 24.0 * SCALE, 60.0 * SCALE };

const dec X_STRETCH_SMALL = 1.5;
const Span<uint> SMALL_PUFF_COUNT = { 1, 3 };
const Span<Planc> SMALL_PUFF_RADIUS = { 2.0 * SCALE, 9.0 * SCALE };
const Span<Planc> SMALL_PUFF_DISTANCE = { ( LARGE_PUFF_RADIUS.max( ) * PUFF_OUTLINE_RATIO ) + SMALL_PUFF_RADIUS.max( ), ( LARGE_PUFF_RADIUS.max( ) * PUFF_OUTLINE_RATIO ) + ( SMALL_PUFF_RADIUS.max( ) * 2.0 ) };
} // namespace

Cloud::Cloud( World * world ) : Object( world )
{
    background( true );

    needs_render_always( true );

    z( 0.1 );
    no_gravity( );
    terrain_boundaries( false );

    // set location

    Planc x = (Planc)half( Engine::screen_width( ) ) + max_dx( );
    if( world->player( ) )
    {
        x += world->player( )->position( ).x( );
    }
    // Coordinate location( x, ( (Planc)Random::rdec( (dec)Engine::screen_height( ), 0.0 /*-(dec)Engine::screen_height()*/ ) ).half( ) - max_dy( ).half( ) );
    Coordinate location( 0.0, 5000.0 ); // todo
    position( location );

    dec scale = Random::rdec( 0.5, 1.0 );

    varray<Puff> large_puffs;
    varray<Puff> small_puffs;

    int large_puff_count = Random::rint( LARGE_PUFF_COUNT );
    for_range( i, large_puff_count )
    {
        Puff & puff = large_puffs.insert_back( );

        puff.radius = Random::rint( LARGE_PUFF_RADIUS ) * scale;

        puff.center_offset = VectorA( Random::rAngle( ), Random::rdec( LARGE_PUFF_DISTANCE ) );
        puff.center_offset.dx( puff.center_offset.dx( ) * X_STRETCH_LARGE );
        puff.center_offset *= scale;

        int small_puff_count = Random::rint( SMALL_PUFF_COUNT );
        for_range( i, small_puff_count )
        {
            Puff & puff = small_puffs.insert_back( );

            puff.radius = Random::rint( SMALL_PUFF_RADIUS ) * scale;

            puff.center_offset = VectorA( Random::rAngle( ), Random::rdec( SMALL_PUFF_DISTANCE ) );
            puff.center_offset.dx( puff.center_offset.dx( ) * X_STRETCH_SMALL );
            puff.center_offset *= scale;
        }
    }

    for_each( puff, small_puffs ) m_cloud_drawing.draw( OUTSIDE_COLOR, Circle( puff.radius, puff.center_offset ), FILLED );
    for_each( puff, large_puffs ) m_cloud_drawing.draw( OUTSIDE_COLOR, Circle( puff.radius, puff.center_offset ), FILLED );
    for_each( puff, large_puffs ) m_cloud_drawing.draw( INSIDE_COLOR, Circle( puff.radius * PUFF_OUTLINE_RATIO, puff.center_offset ), FILLED );
}

Planc Cloud::max_dx( ) { return ( ( LARGE_PUFF_RADIUS.max( ) + LARGE_PUFF_DISTANCE.max( ) ) * X_STRETCH_LARGE ) + ( ( SMALL_PUFF_RADIUS.max( ) + SMALL_PUFF_DISTANCE.max( ) ) * X_STRETCH_SMALL ); }

Planc Cloud::max_dy( ) { return ( LARGE_PUFF_RADIUS.max( ) + LARGE_PUFF_DISTANCE.max( ) + SMALL_PUFF_RADIUS.max( ) + SMALL_PUFF_DISTANCE.max( ) ); }

void Cloud::render( )
{
    Object::render( );

    draw( m_cloud_drawing );
}

void Cloud::update( )
{
    velocity( world( )->wind( ) * WIND_SCALE );

    Object::update( );
}
