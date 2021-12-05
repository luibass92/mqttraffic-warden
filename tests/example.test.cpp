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

//TEST_CASE("JSON-RPC", "[General tests]")
//{
//	jsonrpcpp::entity_ptr entity =	jsonrpcpp::Parser::do_parse(R"({"jsonrpc":"2.0","method" : "single_read","id" : "123","params" : {"pkts":1,"resTopic" : "res","variables" : [{"name":"var1","address" : "<ADDRESS_STRING>","dataType" : "<DATA_TYPE>"}]}})");
//	jsonrpcpp::request_ptr request = std::dynamic_pointer_cast<jsonrpcpp::Request>(entity);
//	decodeRead(request);
//}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}

//TEST_CASE("Configurations", "[Read Configuration File]") {
//	// read configuration
//    structs::Config l_config = io::config::getConfig();
//	REQUIRE(l_config.basicTopic == "comau/smartopc");
//}
