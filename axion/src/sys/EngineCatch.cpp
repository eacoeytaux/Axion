#include "Engine.hpp"

#ifdef AXN_TEST
#define CATCH_CONFIG_MAIN
#include "Catch.hpp"

namespace axn {

bool Engine::run_tests_eng( ) {
    bool passed = true;
    Logger::pause( true );
    Assert::set_test_mode( true );
    cout << "RUNNING CATCH TESTS "
            "==========================================================="
         << endl
         << endl;
    int catch_argc = 1;
    char* catch_argv[ 1 ] = { (char*)"/" }; // why pass "/"? idk it works why question it
    passed = !( ::catch_main( catch_argc, catch_argv ) );
    if( passed )
        cout << "TESTS PASSED "
                "=============================================================="
                "===="
             << endl
             << endl;
    else
        cout << "TESTS FAILED "
                "=============================================================="
                "===="
             << endl
             << endl;
    Assert::set_test_mode( false );
    Logger::pause( false );
    return passed;
}

} // namespace axn

#endif
