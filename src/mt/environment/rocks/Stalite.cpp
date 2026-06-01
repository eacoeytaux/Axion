#include "mt/environment/rocks/Stalite.hpp"

var_const( Color COLOR = ( GRAY_MID ) );

Stalite::Stalite( ptr<Room> in_room, bool in_hanging, Point cref in_base, Planc cref in_width, Planc cref in_height, dec in_z ) : Object( in_room, in_base, in_z )
{
    background( true );

    persist_render( true );

    stationary( true );

    draw( COLOR, Polygon( { PointY( in_height ), PointX( -in_width ), PointX( in_width ) } ) );
}
