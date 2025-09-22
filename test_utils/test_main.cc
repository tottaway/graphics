// This file provides the main function for Catch2 tests
// It should be included as a dependency in all test targets
#include <catch2/catch_session.hpp>

int main(int argc, char* argv[]) {
    return Catch::Session().run(argc, argv);
}