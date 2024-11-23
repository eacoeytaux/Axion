#include "Engine.hpp"
#include "World.hpp"

namespace axn
{

#ifdef AXN_DEBUG
// define debug global statics here
bool Debug::active = false;

bool b_step = false;
void Engine::step( ) { b_step = true; }
#endif

#define check_quit( f ) \
    f;                  \
                        \
    if( quit_break )    \
    {                   \
        break;          \
    }

#define try_catch_error( f ) \
    try                      \
    {                        \
        f;                   \
    }                        \
    catch( ... )             \
    {                        \
        errored = true;      \
    }

#define try_return_error( f, err ) \
    try                            \
    {                              \
        f;                         \
    }                              \
    catch( ... )                   \
    {                              \
        return err;                \
    }

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
        return error_system;
    }

    running = true;

#ifdef AXN_DEBUG
    try_return_error( Logger::init( ), error_not_init );
#endif

    try_return_error( Random::seed( ), error_not_init );

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
                if( b_step )
                {
                    check_quit( world->update( ); );
                    b_step = false;
                }
#endif
                world->pause( true );
            }

            check_quit( world->render( ); );
            check_quit( render_eng( ); );

            int ms = ( 1000.0 / (dec)FPS ) - ( current_ticks_eng( ) - start_ticks );

            if( ms > 0 )
            {
                // Log( INFO_LOG, "waiting ms ............. ( %i )", _ms );
                wait_eng( ms );
            }
            else
            {
                // Log( INFO_LOG, "missed ms .............. ( %i )", -ms );
            }
        }
    }
    catch( ... )
    {
        errored = true;
    }

    try_catch_error( clear_inputs( ) );
    try_catch_error( safe_delete( world ); );

#ifdef AXN_DEBUG
    try_catch_error( Logger::close( ) );
#endif

    try_catch_error( close_eng( ) );

    running = false;

    return errored ? error_system : no_error;
}

} // namespace axn
