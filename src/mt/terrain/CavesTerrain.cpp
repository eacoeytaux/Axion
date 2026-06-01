#include "mt/terrain/CavesTerrain.hpp"

CavesTerrain::CavesTerrain( ptr<Room> in_room, vvarray<Point> cref in_vertices ) : Terrain( in_room, in_vertices )
{
    persist_render( true );

    Planc world_bottom = in_room->bounds( ).lower_bound_y( );

    for_each( edges, edges( ) )
    {
        for_each( edge, edges )
        {

        }
    }

    fill_terrain( GRAY_DARK );

    crop_terrain( );
}
