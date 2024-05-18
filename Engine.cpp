#include "Engine.hpp"

#include "World.hpp"

namespace axn
{

#ifdef AXN_DEBUG
// define debug global statics here
bool Debug::active = false;

bool STEP = false;
void Engine::step( ) { STEP = true; }
#endif

#define check_quit( f ) \
    f;                  \
    if( quit_break )    \
        break;

bool errored = false;
bool quit_break = false;
error Engine::quit( )
{
    quit_break = true;
    return no_error;
}

error Engine::run( World * world, const string _app_name )
{
    if( !Assert( world, "World does not exist!" ) )
    {
        return error_not_init;
    }

    static bool running = false;
    if( !Assert( !running, "Engine is already running!" ) )
    {
        return error_todo;
    }
    running = true;

#ifdef AXN_DEBUG
    try
    {
        Logger::init( );
    }
    catch( ... )
    {
        return error_todo;
    }
#endif

    try
    {
        Random::rseed( );
    }
    catch( ... )
    {
        return error_todo;
    }

    varray<Input *> inputs;
    auto clear_inputs = [ & ]( )
    {
        for_each( input, inputs ) { safe_delete( input ); }
        inputs.clear( );
    };

    errored = false;
    quit_break = false;

    try
    {
        init_eng( _app_name );
        world->init( );

        while( !quit_break )
        {
            uint start_ticks = current_ticks_eng( );

            input_eng( inputs, world );
            world->input( inputs );
            clear_inputs( );

            if( !paused( ) )
            {
                check_quit( world->update( ); );
            }
            else
            {
#ifdef AXN_DEBUG
                if( STEP )
                {
                    check_quit( world->update( ); );
                    STEP = false;
                }
#endif
                world->pause( true );
            }

            check_quit( world->render( ); );
            check_quit( render_eng( ); );

            wait_eng( (int)( max( 0.0, ( 1000.0 / (dec)FPS ) - ( current_ticks_eng( ) - start_ticks ) ) ) );
        }
    }
    catch( ... )
    {
        errored = true;
    }

    clear_inputs( );

    try
    {
        safe_delete( world );
    }
    catch( ... )
    {
        errored = true;
    }

#ifdef AXN_DEBUG
    try
    {
        Logger::close( );
    }
    catch( ... )
    {
        errored = true;
    }
#endif

    try
    {
        close_eng( );
    }
    catch( ... )
    {
        errored = true;
    }

    running = false;

    return errored ? error_todo : no_error;
}

} // namespace axn
