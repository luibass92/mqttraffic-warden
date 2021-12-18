#include "catch.hpp"
#include <cassert>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>
#ifdef __UNIX__
# include <unistd.h>
#endif
#if defined(_WIN32) || defined(WIN32)
# include "Winsock2.h"
#endif

unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}


TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}

