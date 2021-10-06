#pragma once

#include <vector>
#include <concepts>
#include <cmath>

namespace stoch{

std::vector<int> calc_divisors(size_t numChoices, size_t depth);

template<std::semiregular T>
std::vector<std::vector<T>> all_paths(std::vector<T> const& choices, size_t depth )
{
    auto divisors = calc_divisors(choices.size(), depth);
    std::vector<std::vector<T>> ret;
    for(size_t i = 0; i < size_t(std::pow(double(choices.size()), int(depth))); ++i)
    {
        std::vector<T> path;
        // numChoices = 4, depth = 3
        // divisors = [1,4,16]
        for(size_t j = 0; j < divisors.size(); ++j)
        {
            size_t index = i / divisors[j];
            // i = 0: path = [0,0,0]
            // i = 1: path = [1,0,0]
            // i = 2: path = [2,0,0]
            // i = 3: path = [3,0,0]
            // i = 4: path = [0,1,0]
            // i = 5: path = [1,1,0]
            // i = 6: path = [2,1,0]
            // i = 7: path = [3,1,0]
            // i = 8: path = [0,2,0]
            // i = 9: path = [1,2,0]
            // i = 10: path = [2,2,0]
            // i = 11: path = [3,2,0]
            // i = 12: path = [0,3,0]
            // ...
            path.push_back(choices[index % choices.size()]);
        }
        ret.push_back(path);
    }
    return ret;
}

}