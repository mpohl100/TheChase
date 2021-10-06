#include "Stochastics.h"

#include <algorithm>

namespace stoch{

std::vector<int> calc_divisors(size_t numChoices, size_t depth)
{
    std::vector<int> divisors;
    int div = 1;
    for(size_t i = 0; i < depth; ++i)
    {
        divisors.push_back(div);
        div *= int(numChoices);
    }
    std::reverse(divisors.begin(), divisors.end());
    return divisors;
}
}