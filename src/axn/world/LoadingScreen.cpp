#include "axn/world/LoadingScreen.hpp"

#include "axn/world/Room.hpp"
#include "axn/world/Object.hpp"

section( SPINNER )
{
    var_const( ticks SPINNER_PERIOD = ( 60.0 ) ); // C!

    var_const( uint SPINNER_DOTS = ( 12 ) ); // C!
    var_const( Planc SPINNER_RADIUS = ( METER * 1.25 ) ); // C!
    var_const( Planc SPINNER_DOT_RADIUS = ( SPINNER_RADIUS * 0.1 ) ); // C!
    var_const( Color SPINNER_DOT_COLOR = ( WHITE ) ); // C!
}

class Spinner : public Object
{

public:

    Spinner( ptr<Room> in_room ) : Object( in_room ) { needs_render_always( true ); }

protected:

    virtual void render( ) override
    {
        Object::render( );

        // C! clockwise: each dot is further round clockwise and brighter than the one before, and the whole ring turns clockwise
        for_range( i, SPINNER_DOTS )
        {
            draw( SPINNER_DOT_COLOR.a( dec( i + 1 ) / dec( SPINNER_DOTS ) ), Polygon::circle( SPINNER_DOT_RADIUS, Point( VectorA( -( Angle::fraction( dec( SPINNER_DOTS ) ) * dec( i ) ) - ( TAU * ( dec( age( ) ) / dec( SPINNER_PERIOD ) ) ), SPINNER_RADIUS ) ) ) );
        }
    }

};

class LoadingRoom : public Room
{

public:

    LoadingRoom( ptr<World> in_world ) : Room( in_world ) { }

protected:

    virtual void create( ) override
    {
        bounds( FixedRectangle( Engine::screen_width( ), Engine::screen_height( ) ) );

        camera( )->target_offset_ratio_y( 0.0 );

        add_object( new Spinner( this ) );
    }

};

void LoadingScreen::create( ) { World::create( ); add_room( new LoadingRoom( this ) ); }
