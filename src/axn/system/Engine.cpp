#include "axn/system/Engine.hpp"

#include "axn/world/World.hpp"
#include "axn/world/Room.hpp"
#include "axn/world/SplashScreen.hpp"
#include "axn/world/LoadingScreen.hpp"
#include "axn/graphics/Camera.hpp"

#define return_error return( errored ? error_system : no_error )

#define check_quit( func ) func; break_if( quitting )

#define try_catch( func ) try { func; } catch_error( errored = true; breakpoint; ); macro_end
#define try_catch_return( func, error ) try_catch( func ); return_if( errored, error ); macro_end

bool errored = false;
bool quitting = false;

#if defined( AXN_DEBUG_CONTROLS )
bool stepping = false; void Engine::step( ) { stepping = true; }
#endif

const uint Engine::FPS = ( 60 );

const dec ControllerJoystickInput::DEAD_ZONE = ( 0.25 );

var_const( string RESOURCE_FOLDER = ( "res/" ) ); // C! the folder the resource files are in
var_const( uint RESOURCE_SEARCH_DEPTH = ( 8 ) ); // C! how many folders up from the executable to look for it, when it isn't in the working directory

var_const( ticks SOUND_ERROR_TICKS = ( 60.0 ) ); // C! how long sound_error( ) lasts after a sound that isn't loaded is played

var_static( uint64 run_ms = ( 0 ) ); // C! when this run started, for its folder in tmp/

var_static( declmp sound_error_heard = ( 0.0 ) ); // C! how loudly the best heard of those was heard, for loudest_sound( )
var_static( Countdown sound_error_countdown );

var_static( Engine::ImageFormat screenshot_format = ( Engine::NO_IMAGE_FORMAT ) ); // C! a requested screenshot, saved after the next render
var_static( FixedRectangle screenshot_bounds ); // C! the part of the screen to keep, the whole screen when empty

#if defined( AXN_DEBUG )
var_const( string REPLAY_INPUTS = ( nostring ) ); // C! dev param: an inputs.txt to play back in place of the keyboard, mouse, and controllers, or nostring to play normally

// C! one input of a recording being replayed: the main loop pass it came in, and its serialized( ) text
struct ReplayInput
{

    uint cycle = 0;
    string text;

};
#endif

// -- input serialization -- //

section( INPUT_NAMES ) // C! each enum's values by name, in order, so a recording reads as words and is read back by finding the word
{
    var_const( varray<string> KEY_DYNAMIC_NAMES = { "NO_DYNAMIC", "RELEASED", "PRESSED", "HELD" } );
    var_const( varray<string> MOUSE_BUTTON_NAMES = { "NO_BUTTON", "LEFT_BUTTON", "RIGHT_BUTTON", "SCROLL_BUTTON" } );
    var_const( varray<string> MOUSE_DYNAMIC_NAMES = { "NO_DYNAMIC", "RELEASED", "PRESSED", "HELD", "MOVE" } );
    var_const( varray<string> CONTROLLER_BUTTON_NAMES = { "NO_BUTTON", "LEFT_UP_BUTTON", "LEFT_DOWN_BUTTON", "LEFT_LEFT_BUTTON", "LEFT_RIGHT_BUTTON", "RIGHT_UP_BUTTON", "RIGHT_DOWN_BUTTON", "RIGHT_LEFT_BUTTON", "RIGHT_RIGHT_BUTTON", "L1_BUTTON", "L2_BUTTON", "L3_BUTTON", "R1_BUTTON", "R2_BUTTON", "R3_BUTTON", "HOME_BUTTON", "START_BUTTON", "START_OPPOSITE_BUTTON", "TOUCHPAD_BUTTON" } );
    var_const( varray<string> CONTROLLER_DYNAMIC_NAMES = { "NO_DYNAMIC", "RELEASED", "PRESSED", "HELD", "MOVE" } );
    var_const( varray<string> JOYSTICK_NAMES = { "NO_JOYSTICK", "RIGHT_JOYSTICK", "LEFT_JOYSTICK" } );
}

// C! in_value's name in in_names, or its number when it has none
string input_name( varray<string> cref in_names, uint in_value ) { return( in_names.valid_index( in_value ) ? in_names[ in_value ] : to_string( in_value ) ); }

// C! the value named in_name in in_names (or written as a number), 0 when it's neither
uint input_value( varray<string> cref in_names, string cref in_name )
{
    for_range( i, in_names.size( ) ) { return_if( in_names[ i ] == in_name, i ); }
    try { return( uint( stoul( in_name ) ) ); }
    catch( ... ) { return( 0 ); }
}

ptr<Input> Input::deserialized( string cref in_text )
{
    varray<string> words = split( in_text );

    return_null_if( words.size( ) < 3 );

    try
    {
        if( words[ 0 ] == "KEY" )
        {
            return( new KeyInput( KeyInput::Key( stoi( words[ 1 ] ) ), KeyInput::Dynamic( input_value( KEY_DYNAMIC_NAMES, words[ 2 ] ) ) ) );
        }
        else if( ( words[ 0 ] == "MOUSE" ) && ( words.size( ) > 4 ) )
        {
            return( new MouseInput( MouseInput::Button( input_value( MOUSE_BUTTON_NAMES, words[ 1 ] ) ), MouseInput::Dynamic( input_value( MOUSE_DYNAMIC_NAMES, words[ 2 ] ) ), Vector( stod( words[ 3 ] ), stod( words[ 4 ] ) ) ) );
        }
        else if( words[ 0 ] == "BUTTON" )
        {
            return( new ControllerButtonInput( ControllerButtonInput::Button( input_value( CONTROLLER_BUTTON_NAMES, words[ 1 ] ) ), ControllerButtonInput::Dynamic( input_value( CONTROLLER_DYNAMIC_NAMES, words[ 2 ] ) ) ) );
        }
        else if( words[ 0 ] == "JOYSTICK" )
        {
            ControllerJoystickInput::Joystick joystick = ControllerJoystickInput::Joystick( input_value( JOYSTICK_NAMES, words[ 1 ] ) );

            return_if( words[ 2 ] == "DEAD", new ControllerJoystickInput( joystick ) );
            return_if( words.size( ) > 3, new ControllerJoystickInput( joystick, Vector( stod( words[ 2 ] ), stod( words[ 3 ] ) ) ) );
        }
    }
    catch_error( );

    return( nullptr );
}

Error Engine::run( ptr<World> in_world )
{
    return_if( is_null( in_world ), error_system );

    run_ms = milliseconds( );

    try_catch_return( Settings::init( ), error_not_init );

    #if defined( AXN_DEBUG )
    try_catch_return( Assert::init( ), error_not_init );
    try_catch_return( Logger::init( ), error_not_init );
    #endif

    uint initial_seed = 0;

    #if defined( AXN_DEBUG )
    // C! the recording of this run's inputs, and the inputs of a recording being replayed (REPLAY_INPUTS), in order
    ofstream input_record;
    queue<ReplayInput> replay_inputs;
    if( !REPLAY_INPUTS.empty( ) )
    {
        ifstream replay_file = file_in( REPLAY_INPUTS );
        string line;

        while( getline( replay_file, line ) )
        {
            varray<string> words = split( line );
            continue_if( words.empty( ) );

            if( words[ 0 ] == "SEED" ) // C! the seed the run started with
            {
                if( words.size( ) > 1 ) { initial_seed = uint( stoul( words[ 1 ] ) ); }
            }
            else if( words.size( ) > 2 )
            {
                ReplayInput replay_input;
                replay_input.cycle = uint( stoul( words[ 0 ] ) );
                for_range( i, words.size( ) - 2 ) { replay_input.text += ( is_zero( i ) ? nostring : " " ) + words[ i + 2 ]; }
                replay_inputs.push( replay_input );
            }
        }

        Log( INFO_LOG, "replaying inputs ....... ( %s, %u )", charify( REPLAY_INPUTS ), replay_inputs.size( ) );
    }
    #endif

    try_catch_return( Random::seed( initial_seed ), error_not_init );

    #if defined( AXN_DEBUG )
    input_record = file_out( run_directory( ) + "inputs.txt" );
    input_record << "SEED " << initial_seed << "\n";
    #endif

    list<ptr<Input>> inputs;

    lambdaf( clear_inputs )( ) { safe_delete( inputs ); };

    errored = false;
    quitting = false;

    uint cycle_count = 0; // C! the main loop's passes, not counting those spent loading, so a replay lines up however long loads take

    lambdaf( run_eng )( )
    {
        init_eng( in_world->name( ) );

        ptr<World> loading_screen = new LoadingScreen( ); // C! shown in place of a world while it's loading( )
        loading_screen->init( );

        lambdaf( wait_for_frame )( int in_start_ms )
        {
            int ms_passed = ( current_ms_eng( ) ) - in_start_ms;
            int ms_wait = ( 1000.0 / dec( FPS ) ) - ms_passed;

            if( is_pos( ms_wait ) ) { wait_eng( ms_wait ); }
        };

        lambdaf( render_world )( ptr<World> in_world )
        {
            in_world->render( );

            if( screenshot_format != NO_IMAGE_FORMAT )
            {
                screenshot_eng( screenshot_format, screenshot_bounds );
                screenshot_format = NO_IMAGE_FORMAT;
            }

            render_eng( );
        };

        lambdaf( update_world )( ptr<World> in_world )
        {
            in_world->update( );

            update_eng( );
        };

        lambdaf( run_world )( ptr<World> in_world )
        {
            in_world->init( );

            while( !quitting && !in_world->quit( ) )
            {
                int start_ms = current_ms_eng( );

                clear_inputs( );
                input_eng( inputs );

                if( in_world->loading( ) )
                {
                    clear_inputs( );

                    check_quit( in_world->update_loading( ) ); // C! ends the loading screen once the load is done

                    check_quit( update_world( loading_screen ) );

                    if( loading_screen->camera( ) ) { check_quit( loading_screen->camera( )->update( ) ); }

                    check_quit( render_world( loading_screen ) );
                }
                else
                {
                    cycle_count++;

                    const uint c_room_age = in_world->current_room( ) ? in_world->current_room( )->age( ) : 0;

                    #if defined( AXN_DEBUG )
                    if( !REPLAY_INPUTS.empty( ) )
                    {
                        clear_inputs( ); // C! replaying: the recording's inputs take the place of the keyboard, mouse, and controllers

                        while( !replay_inputs.empty( ) && ( replay_inputs.front( ).cycle <= cycle_count ) )
                        {
                            ptr<Input> input = Input::deserialized( replay_inputs.pop( ).text );
                            if( !is_null( input ) ) { inputs.insert_back( input ); }
                        }
                    }

                    for_each( input, inputs ) { input_record << cycle_count << " " << c_room_age << " " << input->serialized( ) << "\n"; }
                    #endif

                    in_world->input( inputs );

                    in_world->pause( paused( ) ); // C! the world follows the engine both ways (it used to only ever be paused, so unpausing left it frozen)

                    if( !paused( ) )
                    {
                        check_quit( update_world( in_world ) );
                    }
                    else
                    {
                        #if defined( AXN_DEBUG )
                        if( stepping )
                        {
                            stepping = false;
                            in_world->pause( false );
                            check_quit( update_world( in_world ) );
                            in_world->pause( true );
                        }
                        #endif
                    }

                    #if defined( AXN_DEBUG )
                    input_record.flush( ); // C! so a crash still leaves the recording up to it
                    #endif

                    in_world->play_queued_sounds( ); // C! everything the update queued, paused or not (a screenshot's click is heard while its shutter pauses the game)
                    tick_sound_error( );

                    if( in_world->camera( ) ) { check_quit( in_world->camera( )->update( ) ); } // C! even while paused, so screen effects run and one can end a pause it started

                    render_world( in_world );
                }

                break_if( quitting );

                wait_for_frame( start_ms );
            }

            try_catch( safe_delete( in_world ) );
        };

        #if !defined( AXN_DEBUG )
        run_world( new SplashScreen( ) );
        #endif

        run_world( in_world );

        try_catch( safe_delete( loading_screen ) );
    };

    #if defined( AXN_DEBUG )
    #if defined( AXN_TESTS )
    Debug::bbreakpoint = false;
    try_catch( run_tests_eng( ) );
    #endif
    Debug::bbreakpoint = true;
    #endif

    #if defined( AXN_COVERAGE )
    return_error;
    #endif

    try_catch( run_eng( ) );

    try_catch( clear_inputs( ) );

    try_catch( close_eng( ) );

    #if defined( AXN_DEBUG )
    try_catch( Logger::close( ) );
    #endif

    return_error;
}

void Engine::quit( ) { quitting = true; }

void Engine::screenshot( ImageFormat in_format, FixedRectangle cref in_bounds ) { screenshot_format = in_format; screenshot_bounds = in_bounds; }

string Engine::run_directory( string cref in_folder )
{
    if( is_zero( run_ms ) ) { run_ms = milliseconds( ); } // C! set when the run starts, this is only for anything asking before then

    string directory = "tmp/";
    make_directory_eng( directory );

    directory += "run-" + to_string( run_ms ) + "/";
    make_directory_eng( directory );

    return_if( in_folder.empty( ), directory );

    directory += in_folder + "/";
    make_directory_eng( directory );

    return( directory );
}

string Engine::resource_directory( )
{
    static_setup( string directory )
    {
        directory = RESOURCE_FOLDER; // C! when it can't be found, the working directory's, so the warnings about missing files say where they were looked for

        if( !directory_exists_eng( RESOURCE_FOLDER ) )
        {
            string above = executable_directory_eng( );

            for_range( i, RESOURCE_SEARCH_DEPTH )
            {
                break_if( above.empty( ) );

                if( directory_exists_eng( above + RESOURCE_FOLDER ) )
                {
                    directory = above + RESOURCE_FOLDER;
                    break;
                }

                above += "../";
            }
        }
    }

    return( directory );
}

Speaker ref Engine::speaker( )
{
    // todo don't like this set up
    static Speaker speaker;
    return( speaker );
}

void Engine::play_sound( SoundFX cref in_sound, declmp in_heard_volume )
{
    if( !in_sound.loaded( ) )
    {
        sound_error_heard = ( sound_error( ) ? max( dec( sound_error_heard ), dec( in_heard_volume ) ) : dec( in_heard_volume ) );
        sound_error_countdown.reset( SOUND_ERROR_TICKS );
        return;
    }

    play_sound_eng( in_sound, in_heard_volume );
}

declmp Engine::loudest_sound( ) { return( sound_error( ) ? max( dec( loudest_sound_eng( ) ), dec( sound_error_heard ) ) : dec( loudest_sound_eng( ) ) ); }

bool Engine::sound_error( ) { return( !sound_error_countdown.completed( ) ); }
void Engine::tick_sound_error( ) { sound_error_countdown.tick( ); }

void Engine::update_eng( )
{
    #if defined( AXN_DEBUG )
    static uint loop_count = 0; loop_count++;
    #endif
}

string KeyInput::serialized( ) const { return( "KEY " + to_string( int( key ) ) + " " + input_name( KEY_DYNAMIC_NAMES, dynamic ) ); }
string MouseInput::serialized( ) const { return( "MOUSE " + input_name( MOUSE_BUTTON_NAMES, button ) + " " + input_name( MOUSE_DYNAMIC_NAMES, dynamic ) + " " + serialized_dec( movement.dx( ) ) + " " + serialized_dec( movement.dy( ) ) ); }
string ControllerButtonInput::serialized( ) const { return( "BUTTON " + input_name( CONTROLLER_BUTTON_NAMES, button ) + " " + input_name( CONTROLLER_DYNAMIC_NAMES, dynamic ) ); }
string ControllerJoystickInput::serialized( ) const { return( "JOYSTICK " + input_name( JOYSTICK_NAMES, joystick ) + ( dead_zone ? string( " DEAD" ) : ( " " + serialized_dec( vector.dx( ) ) + " " + serialized_dec( vector.dy( ) ) ) ) ); }
