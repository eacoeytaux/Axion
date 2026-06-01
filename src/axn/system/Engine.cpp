#include "axn/system/Engine.hpp"

#include "axn/world/World.hpp"
#include "axn/world/SplashScreen.hpp"

namespace axn
{

dec ControllerJoystickInput::DEAD_ZONE = 0.25;

#if defined( AXN_DEBUG )
// define debug global statics here
bool Assert::active = true;
bool Debug::active = false;
bool Debug::bbreakpoint = true;
#endif

#if defined( AXN_DEBUG_CONTROLS )
bool b_step = false;
void Engine::step( ) { b_step = true; }
#endif

#define check_quit( func ) func; break_if( quit_break )

#define try_catch_error( func ) try { func; } catch( exception e ) { Log( ERROR_LOG, "error - %s", e.what( ) ); errored = true; } true
#define try_return_error( func, err ) try_catch_error( func ); return_if( errored, err )

bool errored = false;
bool quit_break = false;

error Engine::quit( )
{
    quit_break = true;

    return( no_error );
}

void Engine::update_eng( )
{
    #if defined( AXN_DEBUG )
    static uint loop_count = 0;
    loop_count++;
    #endif
}

error Engine::run( ptr<World> in_world )
{
    return_if( is_null( in_world ), error_system );

    try_return_error( Settings::init( ), error_not_init );

    #if defined( AXN_DEBUG )
    try_return_error( Assert::init( ), error_not_init );
    try_return_error( Logger::init( ), error_not_init );
    #endif

    try_return_error( Random::seed( 0 ), error_not_init );

    list<ptr<Input>> inputs;

    #if defined( AXN_DEBUG )
    list<list<ptr<Input>>> inputss;
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

    lambdaf( run_eng )( )
    {
        init_eng( in_world->name( ) );

        lambdaf( run_world )( ptr<World> in_world )
        {
            in_world->init( );

            while( !quit_break && !in_world->quit( ) )
            {
                int start_ms = current_ms_eng( );

                clear_inputs( );
                input_eng( inputs );

                #if defined( AXN_DEBUG )
                inputss.insert_back( inputs );
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
                    if( b_step )
                    {
                        b_step = false;
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

            try_catch_error( safe_delete( in_world ) );
        };

        #if !defined( AXN_DEBUG )
        run_world( new SplashScreen( ) );
        #endif

        run_world( in_world );
    };

    #if defined( AXN_DEBUG )
    #if defined( AXN_TESTS )
    Debug::bbreakpoint = false;
    try_catch_error( run_tests_eng( ) );
    #endif
    Debug::bbreakpoint = true;
    #endif

    try_catch_error( run_eng( ) );

    try_catch_error( clear_inputs( ) );

    #if defined( AXN_DEBUG )
    for_each( inputs, inputss ) { try_catch_error( safe_delete( inputs ) ); }
    #endif

    try_catch_error( close_eng( ) );

    #if defined( AXN_DEBUG )
    try_catch_error( Logger::close( ) );
    #endif

    return( errored ? error_system : no_error );
}

} // namespace axn
