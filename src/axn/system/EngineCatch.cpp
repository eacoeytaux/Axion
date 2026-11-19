#include "axn/system/Engine.hpp"
#include "axn/world/Terrain.hpp" // C! must come before Catch, windows.h declares its own Polygon

#if defined( AXN_TESTS )

#undef ref // interferes with Catch

#define main catch_main

#define DO_NOT_USE_WMAIN
#define CATCH_CONFIG_MAIN
#include "axn/tests/Catch.hpp"

#include "axn/tests/Tests.hpp"
#include "axn/tests/TestsUtil.hpp"
#include "axn/tests/TestsGeometry.hpp"
#include "axn/tests/TestsTerrain.hpp"
#include "axn/tests/TestsMovement.hpp"
// include Test*.hpp files here

namespace axn
{

bool Engine::run_tests_eng( )
{
    Assert::push_enable( false );
    Logger::push_pause( true );

    cout << endl << "RUNNING CATCH TESTS ===========================================================" << endl << endl;

    int catch_argc = 1; chars catch_argv[ 1 ] = { chars( "/" ) }; bool passed = !( ::catch_main( catch_argc, catch_argv ) ); // why pass "/"? idk but it works so why question it

    cout << "TESTS " << ( passed ? "PASSED" : "FAILED" ) << " ==================================================================" << endl << endl;

    Logger::pop_pause( );
    Assert::pop_enable( );

    return( passed );
}

} // namespace axn

#endif