#pragma once

#pragma once

#include <string>
#include <memory>
#include <map>

namespace chase{

class Chase;

struct GamePlan { // Chromosome class
    
    void crossover(GamePlan const& other);
    void mutate();
    std::string toString() const;

    struct Percentages{
        double gamble = 0.2;
        double stay = 0.6;
        // double safety = 0.2 is calcualted implicitly
        size_t startingStep(int num) const;
        void normalize();
        std::string toString() const;
    };

    //       Player index
    std::map<size_t, Percentages> percentages;
    std::shared_ptr<Chase> chase;
};

}