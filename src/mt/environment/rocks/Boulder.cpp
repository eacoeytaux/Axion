#include "mt/environment/rocks/Boulder.hpp"

var_const( Span<Planc> WIDTH_RATIO = { 0.75, 0.85 } );
var_const( Span<Planc> TOP_WIDTH_RATIO = { 0.25, 0.35 } );
var_const( Span<Planc> OFFSET_RATIO = { 0.05, 0.1 } );

var_const( Color COLOR = ( GRAY_MID ) );

Boulder::Boulder( ptr<Room> in_room, Point cref in_base, Planc cref in_scale, dec in_z ) : Object( in_room, in_base, in_z )
{
    background( true );

    persist_render( true );

    stationary( true );

    draw( COLOR, Polygon( Arc::semi_ccw( in_scale, in_base ).path( 21 ).points( ) ).stretch( VectorY( 2.5 ) ) );
}
