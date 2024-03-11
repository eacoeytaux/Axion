#include "Engine.hpp"

AXN_NAMESPACES
#include "World.hpp"

namespace axn {

#ifdef AXN_DEBUG
// define debug global statics here
bool Debug::active = false;
bool Assert::b_test_mode = false;
#endif

bool quit_break = false;
err Engine::quit( ) {
    quit_break = true;
    return no_error;
}

err Engine::run( World* world, const string _app_name ) {
    if( !Assert( world, "World does not exist!" ) )
        return error_not_init;

    static bool running = false;
    if( !Assert( !running, "Engine is already running!" ) )
        return error_todo;
    running = true;

    quit_break = false;

    bool errored = false;
    try {

        Random::seed( );
        Logger::init( );

#ifdef AXN_TEST
        Assert( run_tests_eng( ), "testing failed!" );
#endif

        init_eng( _app_name );
        world->init( );

        while( !quit_break ) {
            uint start_ticks = current_ticks_eng( );

            static varray<Input*> inputs;
            for_each( input, inputs ) { delete input; }
            inputs.clear( );
            input_eng( world, inputs );
            if( quit_break )
                break;
            update_eng( world );
            if( quit_break )
                break;
            world->input( inputs );
            if( quit_break )
                break;
            world->update( );
            if( quit_break )
                break;
            world->render( );
            if( quit_break )
                break;
            render_eng( world );
            if( quit_break )
                break;

            wait_eng( max<int>( 0, (int)( 1000.f / (float)FPS ) -
                                       (int)( current_ticks_eng( ) - start_ticks ) ) );
        }

    } catch( ... ) {
        errored = true;
    }

    try {
        delete world;
    } catch( ... ) {
        errored = true;
    }
    try {
        Logger::close( );
    } catch( ... ) {
        errored = true;
    }
    try {
        close_eng( );
    } catch( ... ) {
        errored = true;
    }

    running = false;

    return errored ? error_todo : no_error;
}

} // namespace axn
