#include "axn/system/Engine.hpp"

#if defined( AXN_TESTS )

// catch defines
#define DO_NOT_USE_WMAIN
#define CATCH_CONFIG_MAIN
#include "axn/tests/Catch.hpp"

#include "axn/tests/Tests.hpp"
#include "axn/tests/TestsUtil.hpp"
#include "axn/tests/TestsGeometry.hpp"

namespace axn
{

bool Engine::run_tests_eng( )
{
    Logger::pause( true );
    Assert::enable( false );

    cout << endl << "RUNNING CATCH TESTS ===========================================================" << endl << endl;

    int catch_argc = 1;
    char * catch_argv[ 1 ] = { chars( "/" ) }; // why pass "/"? idk but it works so why question it

    bool passed = !( ::catch_main( catch_argc, catch_argv ) );

    cout << "TESTS " << ( passed ? "PASSED" : "FAILED" ) << " ==================================================================" << endl << endl;

    Assert::enable( true );
    Logger::pause( false );

    return( passed );
}

} // namespace axn

#endif