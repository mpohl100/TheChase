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
        std::string toStringShort() const;
    };
    Percentages percentage;

    struct Data{
        double amountInitial = 0.0;
        double amountGained = 0.0;
        double amountAvg = 0.0;
        double percentageWon = 0.0;
        std::string toStringShort() const;
    };
    mutable Data data;
};

}