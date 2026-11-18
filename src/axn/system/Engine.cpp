#include "axn/system/Engine.hpp"

#include "axn/world/World.hpp"
#include "axn/world/SplashScreen.hpp"

#define check_quit( func ) func; break_if( quit_break )

#define try_catch( func ) try { func; } catch_error( errored = true; breakpoint; ); macro_end
#define try_catch_return( func, error ) try_catch( func ); return_if( errored, error ); macro_end

const dec ControllerJoystickInput::DEAD_ZONE = 0.25;

const uint Engine::FPS = 60;

bool errored = false;
bool quit_break = false;

#if defined( AXN_DEBUG_CONTROLS )
bool stepping = false; void Engine::step( ) { stepping = true; }
#endif

Error Engine::quit( ) { quit_break = true; return( no_error ); }

void Engine::update_eng( )
{
    #if defined( AXN_DEBUG )
    static uint loop_count = 0; loop_count++;
    #endif
}

Error Engine::run( ptr<World> in_world )
{
    return_if( is_null( in_world ), error_system );

    try_catch_return( Settings::init( ), error_not_init );

    #if defined( AXN_DEBUG )
    try_catch_return( Assert::init( ), error_not_init );
    try_catch_return( Logger::init( ), error_not_init );
    #endif

    try_catch_return( Random::seed( 0 ), error_not_init );

    list<ptr<Input>> inputs;

    #if defined( AXN_DEBUG )
    struct input_log { uint cycle; list<ptr<Input>> inputs; input_log( uint in_cycle, list<ptr<Input>> in_inputs ) : cycle( in_cycle ), inputs( in_inputs ) { } };
    list<input_log> logged_inputs;
    #endif

    lambdaf( clear_inputs )( )
    {
        #if defined( AXN_DEBUG )
        inputs.clear( ); // do not delete, instead keep for input log
        #else
        safe_delete( inputs );
        #endif
    };

    quit_break = false;
    errored = false;

    uint cycle_count = 0;

    lambdaf( run_eng )( )
    {
        init_eng( in_world->name( ) );

        lambdaf( run_world )( ptr<World> in_world )
        {
            in_world->init( );

            while( !quit_break && !in_world->quit( ) )
            {
                int start_ms = current_ms_eng( );

                cycle_count++;

                clear_inputs( );
                input_eng( inputs );

                #if defined( AXN_DEBUG )
                logged_inputs.insert_back( input_log( cycle_count, inputs ) );
                #endif

                in_world->input( inputs );

                if( !paused( ) )
                {
                    check_quit( in_world->update( ) );
                    check_quit( update_eng( ) );
                }
                else
                {
                    in_world->pause( true );

                    #if defined( AXN_DEBUG )
                    if( stepping )
                    {
                        stepping = false;
                        check_quit( in_world->update( ) );
                        check_quit( update_eng( ) );
                    }
                    #endif
                }

                check_quit( in_world->render( ) );
                check_quit( render_eng( ) );

                int ms_passed = ( current_ms_eng( ) ) - start_ms;
                int ms_wait = ( 1000.0 / dec( FPS ) ) - ms_passed;

                if( is_pos( ms_wait ) ) { wait_eng( ms_wait ); }
            }

            try_catch( safe_delete( in_world ) );
        };

        #if !defined( AXN_DEBUG )
        run_world( new SplashScreen( ) );
        #endif

        run_world( in_world );
    };

    #if defined( AXN_DEBUG )
    #if defined( AXN_TESTS )
    Debug::bbreakpoint = false;
    try_catch( run_tests_eng( ) );
    #endif
    Debug::bbreakpoint = true;
    #endif

    try_catch( run_eng( ) );

    try_catch( clear_inputs( ) );

    #if defined( AXN_DEBUG )
    for_each( logged_input, logged_inputs ) { try_catch( safe_delete( logged_input.inputs ) ); }
    #endif

    try_catch( close_eng( ) );

    #if defined( AXN_DEBUG )
    try_catch( Logger::close( ) );
    #endif

    return( errored ? error_system : no_error );
}
