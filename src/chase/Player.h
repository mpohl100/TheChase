#pragma once

#include "GamePlan.h"

#include <evol/Rng.h>

namespace chase{

class Player{
public:
    Player(double knows, size_t index);
    
    Player() = default;
    Player(Player const&) = default;
    Player& operator=(Player const&) = default;
    Player(Player&&) = default;
    Player& operator=(Player&&) = default;

    bool answer(int num) const;
    size_t deduceStartingStep(GamePlan const& gamePlan, evol::Rng const& rng) const;
    double equity() const;
    size_t index() const;

private:
    double knows_ = 0.0;
    size_t index_ = 0;
};

}