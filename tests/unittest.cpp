#include <catch2/catch.hpp>

#include "Chase.h"

namespace {

TEST_CASE("Lib", "[lib]"){
    SECTION("Example"){
        chase::say_hello("World");
        CHECK(true);
    }
}

}