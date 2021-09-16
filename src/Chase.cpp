#include "Chase.h"

#include <range/v3/all.hpp>

#include <iostream>

namespace chase{

void say_hello(std::string name)
{
    std::cout << "Hello, " << name << "!" << '\n';
}

Player::Player(double knows)
  : knows_(knows)
{}

bool Player::answer(int num) const
{
    return int(knows_ * 100) <= num;
}

Chase::Chase(double candidateChance, size_t numPlayers)
{
    for(size_t i = 0; i < numPlayers; ++i)
        candidates_.emplace_back(candidateChance);
    chaser_ = Player(0.8);
}

}