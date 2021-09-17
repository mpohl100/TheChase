#include "Rng.h"

namespace evol{

Rng::Rng()
    : rd_()
    , gen_(rd_())
{} 

std::stack<int> Rng::fetchUniform(int from, int to, size_t num) const
{
    std::uniform_int_distribution dist{from, to};
    std::stack<int> ret;
    for(size_t i = 0; i < num; ++i)
        ret.push(dist(gen_));
    return ret;
}

std::stack<double> Rng::fetchNormal( double expValue, double stdDev, size_t num) const
{
    std::normal_distribution dist{expValue, stdDev};
    std::stack<double> ret;
    for(size_t i = 0; i < num; ++i)
        ret.push(dist(gen_));
    return ret;
}

}

