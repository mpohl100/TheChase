#include "Player.h"
#include "Util.h"

namespace chase {

Player::Player(double knows, size_t index)
  : knows_(knows)
  , index_(index)
{}

bool Player::answer(int num) const
{
    return eventHappens(knows_, num);
}

size_t Player::deduceStartingStep(GamePlan const& gamePlan, evol::Rng const& rng) const
{
    auto it = gamePlan.percentages.find(index());
    if(it != gamePlan.percentages.end())
    {
        int num = rng.fetchUniform(0, 100, 1).top();
        return it->second.startingStep(num);
    }
    return 5; // by default stay
}

double Player::equity() const
{
    return knows_;
}

size_t Player::index() const
{
    return index_;
}

}