#include <catch2/catch.hpp>

#include <chase/Stochastics.h>

#include <iostream>

namespace {

TEST_CASE("Chase", "[chase]"){
    SECTION("Stochastics"){
        auto paths = stoch::all_paths(std::vector<int>{0,1,2,3}, 3);
        for(const auto& path : paths)
        {
            for(auto num : path)
                std::cout << num << ',';
            std::cout << '\n';
        }
        CHECK(true);
    }
}

}