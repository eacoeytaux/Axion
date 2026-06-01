#include "axn/world/SplashScreen.hpp"

#include "axn/world/Room.hpp"
#include "axn/world/Object.hpp"

var_const( ticks MAX_SPLASH_DURATION_SEC = ( 5.0 ) );

var_const( Planc LETTER_THICKNESS = ( 5.0 ) );
var_const( Planc LETTER_HEIGHT_RATIO = ( 1.75 ) );
var_const( Planc LETTER_WIDTH = ( METER * 2.5 ) );
var_const( Planc LETTER_HEIGHT = ( LETTER_WIDTH * LETTER_HEIGHT_RATIO ) );
var_const( Planc LETTER_SPACING = ( LETTER_WIDTH * 0.2 ) );
var_const( Color LETTER_COLOR = ( WHITE ) );

class Logo : public Object
{

public:

    Logo( ptr<Room> in_room ) : Object( in_room ) { }

protected:

    void render( ) override
    {
        Object::render( );

        Point letter_center;

        letter_center = ORIGIN;

        // I
        draw( LETTER_COLOR, Line( PointY( -half( LETTER_HEIGHT ) ), PointY( half( LETTER_HEIGHT ) ) ) + letter_center, LETTER_THICKNESS );

        draw( LETTER_COLOR, Polygon::circle( half( LETTER_THICKNESS ), PointY( half( LETTER_HEIGHT ) ) + letter_center ) );
        draw( LETTER_COLOR, Polygon::circle( half( LETTER_THICKNESS ), PointY( -half( LETTER_HEIGHT ) ) + letter_center ) );

        letter_center += VectorX( half( LETTER_WIDTH ) + LETTER_SPACING );

        // O
        draw( LETTER_COLOR, Polygon::circle( half( LETTER_WIDTH + half( LETTER_THICKNESS ) ), letter_center ).stretch( VectorY( LETTER_HEIGHT_RATIO ) ), LETTER_THICKNESS );

        letter_center += VectorX( LETTER_WIDTH + LETTER_SPACING );

        // N
        draw( LETTER_COLOR, Line( Point( -half( LETTER_WIDTH ), -half( LETTER_HEIGHT ) ), Point( -half( LETTER_WIDTH ), half( LETTER_HEIGHT ) ) ) + letter_center, LETTER_THICKNESS );
        draw( LETTER_COLOR, Line( Point( -half( LETTER_WIDTH ), half( LETTER_HEIGHT ) ), Point( half( LETTER_WIDTH ), -half( LETTER_HEIGHT ) ) ) + letter_center, LETTER_THICKNESS );
        draw( LETTER_COLOR, Line( Point( half( LETTER_WIDTH ), -half( LETTER_HEIGHT ) ), Point( half( LETTER_WIDTH ), half( LETTER_HEIGHT ) ) ) + letter_center, LETTER_THICKNESS );

        draw( LETTER_COLOR, Polygon::circle( half( LETTER_THICKNESS ), Point( half( LETTER_WIDTH ), half( LETTER_HEIGHT ) ) + letter_center ) );
        draw( LETTER_COLOR, Polygon::circle( half( LETTER_THICKNESS ), Point( half( LETTER_WIDTH ), -half( LETTER_HEIGHT ) ) + letter_center ) );
        draw( LETTER_COLOR, Polygon::circle( half( LETTER_THICKNESS ), Point( -half( LETTER_WIDTH ), half( LETTER_HEIGHT ) ) + letter_center ) );
        draw( LETTER_COLOR, Polygon::circle( half( LETTER_THICKNESS ), Point( -half( LETTER_WIDTH ), -half( LETTER_HEIGHT ) ) + letter_center ) );

        letter_center = ORIGIN;

        letter_center -= VectorX( half( LETTER_WIDTH ) + LETTER_SPACING );

        // X
        draw( LETTER_COLOR, Line( Point( -half( LETTER_WIDTH ), -half( LETTER_HEIGHT ) ), Point( half( LETTER_WIDTH ), half( LETTER_HEIGHT ) ) ) + letter_center, LETTER_THICKNESS );
        draw( LETTER_COLOR, Line( Point( -half( LETTER_WIDTH ), half( LETTER_HEIGHT ) ), Point( half( LETTER_WIDTH ), -half( LETTER_HEIGHT ) ) ) + letter_center, LETTER_THICKNESS );

        draw( LETTER_COLOR, Polygon::circle( half( LETTER_THICKNESS ), Point( half( LETTER_WIDTH ), half( LETTER_HEIGHT ) ) + letter_center ) );
        draw( LETTER_COLOR, Polygon::circle( half( LETTER_THICKNESS ), Point( half( LETTER_WIDTH ), -half( LETTER_HEIGHT ) ) + letter_center ) );
        draw( LETTER_COLOR, Polygon::circle( half( LETTER_THICKNESS ), Point( -half( LETTER_WIDTH ), half( LETTER_HEIGHT ) ) + letter_center ) );
        draw( LETTER_COLOR, Polygon::circle( half( LETTER_THICKNESS ), Point( -half( LETTER_WIDTH ), -half( LETTER_HEIGHT ) ) + letter_center ) );

        letter_center -= VectorX( LETTER_WIDTH + LETTER_SPACING );

        // A
        draw( LETTER_COLOR, Line( PointX( -half( half( LETTER_WIDTH ) ) ), PointX( half( half( LETTER_WIDTH ) ) ) ) + letter_center, LETTER_THICKNESS );
        draw( LETTER_COLOR, Line( Point( -half( LETTER_WIDTH ), -half( LETTER_HEIGHT ) ), PointY( half( LETTER_HEIGHT ) ) ) + letter_center, LETTER_THICKNESS );
        draw( LETTER_COLOR, Line( Point( half( LETTER_WIDTH ), -half( LETTER_HEIGHT ) ), PointY( half( LETTER_HEIGHT ) ) ) + letter_center, LETTER_THICKNESS );

        draw( LETTER_COLOR, Polygon::circle( half( LETTER_THICKNESS ), Point( half( LETTER_WIDTH ), -half( LETTER_HEIGHT ) ) + letter_center ) );
        draw( LETTER_COLOR, Polygon::circle( half( LETTER_THICKNESS ), Point( -half( LETTER_WIDTH ), -half( LETTER_HEIGHT ) ) + letter_center ) );
        draw( LETTER_COLOR, Polygon::circle( half( LETTER_THICKNESS ), PointY( half( LETTER_HEIGHT ) ) + letter_center ) );
    }
};

class SplashRoom : public Room
{

public:

    SplashRoom( ptr<World> in_world ) : Room( in_world ) { };

protected:

    virtual void create( ) override
    {
        camera( )->target_offset_ratio_y( 0.0 );

        bounds( FixedRectangle( Engine::screen_width( ), Engine::screen_height( ) ) );

        add_object( new Logo( this ) );
    }
};

bool SplashScreen::quit( ) const
{
    return( m_complete || ( age( ) > ( dec( MAX_SPLASH_DURATION_SEC ) * dec( Engine::FPS ) ) ) );
}

void SplashScreen::create( )
{
    World::create( );

    add_room( new SplashRoom( this ) );
}

void SplashScreen::input( list<ptr<Input>> cref in_inputs )
{
    World::input( in_inputs );

    for_each( input, in_inputs )
    {
        if( cast_as( mouse_input, MouseInput, input ) )
        {
            MouseInput::Button button = mouse_input->button;
            MouseInput::Dynamic dynamic = mouse_input->dynamic;

            bool move = dynamic == MouseInput::MOVE;
            bool pressed = dynamic == MouseInput::PRESSED;
            bool released = dynamic == MouseInput::RELEASED;

            if( pressed )
            {
                complete( );
            }
        }
        else if( cast_as( key_input, KeyInput, input ) )
        {
            KeyInput::Key key = key_input->key;
            KeyInput::Dynamic dynamic = key_input->dynamic;

            bool pressed = dynamic == KeyInput::PRESSED;
            bool released = dynamic == KeyInput::RELEASED;
            bool held = dynamic == KeyInput::HELD;
            bool down = pressed || held;

            if( pressed )
            {
                complete( );
            }
        }
        else if( cast_as( controller_input, ControllerInput, input ) )
        {
            if( cast_as( button_input, ControllerButtonInput, controller_input ) )
            {
                ControllerButtonInput::Button button = button_input->button;
                ControllerButtonInput::Dynamic dynamic = button_input->dynamic;

                bool pressed = dynamic == ControllerButtonInput::PRESSED;

                if( pressed )
                {
                    complete( );
                }
            }
        }
    }
}
