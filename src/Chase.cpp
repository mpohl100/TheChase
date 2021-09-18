#include "Chase.h"
#include "Rng.h"

#include <range/v3/all.hpp>
#include <algorithm>
#include <iostream>
#include <cmath>
namespace chase{

void say_hello(std::string name)
{
    std::cout << "Hello, " << name << "!" << '\n';
}

Player::Player(double knows)
  : knows_(knows)
{}

bool eventHappens(double percentage, int num)
{
    return int(percentage * 100) <= num;
}

bool Player::answer(int num) const
{
    return eventHappens(knows_, num);
}

bool Player::answerPossibilities(int num) const
{
    // we have a 1/3 chance of guessing correctly
    return eventHappens(knows_ + (1.0 - knows_) / 3, num);
}

size_t Player::deduceStartingStep(GamePlan const& gamePlan, evol::Rng const& rng) const
{
    auto it = gamePlan.percentages.find(knows_);
    if(it != gamePlan.percentages.end())
    {
        int num = rng.fetchUniform(0, 100, 1).top();
        return it->second.startingStep(num);
    }
    return 5; // by default stay
}

void GamePlan::crossover(GamePlan const& other)
{
    for(auto& [equity, percentage] : percentages)
    {
        if(auto it = other.percentages.find(equity); it != other.percentages.end())
        {
            percentage.gamble += it->second.gamble;
            percentage.gamble /= 2.0;
            percentage.stay += it->second.stay;
            percentage.stay /= 2.0;
        }
    }
}

void GamePlan::mutate()
{
    evol::Rng rng;
    size_t elementToMutate = rng.fetchUniform(0, percentages.size()-1, 1).top();
    auto it = percentages.begin();
    std::advance(it, elementToMutate);
    bool modifyGamble = rng.fetchUniform(0,1,1).top() == 0;
    int modifyPercentage = rng.fetchUniform(-10,10,1).top();
    if(modifyGamble)
        it->second.gamble += modifyPercentage / 100.0;
    else
        it->second.stay += modifyPercentage / 100.0;
    it->second.normalize();
}

std::string GamePlan::toString() const
{
    std::string ret = "\n";
    for( const auto& [equity, percentage] : percentages)
        ret += "player(" + std::to_string(equity*100) + "): " + percentage.toString() + "\n"; 
    return ret;
}

size_t GamePlan::Percentages::startingStep(int num) const
{
    if(eventHappens(gamble, num))
        return 6;
    else if(eventHappens(gamble+stay, num))
        return 5;
    else
        return 4;
}

void GamePlan::Percentages::normalize()
{
    if(gamble < 0) gamble = 0;
    if(stay < 0) stay = 0;
    if(gamble + stay > 1)
    {
        gamble *= 1.0 / (gamble + stay);
        stay *= 1.0 / (gamble + stay); 
    }
}

std::string GamePlan::Percentages::toString() const
{
    std::string ret;
    ret += "gamble(" + std::to_string(gamble*100) + "%), ";
    ret += "stay(" + std::to_string(stay*100) + "%), ";
    ret += "safety(" + std::to_string((1-gamble-stay)*100) + "%)";
    return ret;
 }

Chase::Chase(double candidateChance, size_t numPlayers, size_t numRounds)
    : numRounds_(numRounds)
{
    for(size_t i = 0; i < numPlayers; ++i)
        candidates_.emplace_back(candidateChance);
    chaser_ = Player(0.8);
}


double Chase::score(GamePlan const& gamePlan)
{
    double ret = 0;
    evol::Rng rng;
    for(size_t i = 0; i < numRounds_; ++i)
        ret += play(gamePlan, rng);
    return ret / double(numRounds_);
}

double Chase::play(GamePlan const& gamePlan, evol::Rng const& rng)
{
    double result = 0;
    std::vector<std::reference_wrapper<const Player>> finalPlayers;
    for(const Player& candidate : candidates_)
    {
        double playerAmount = playQuickRound(candidate, rng);
        if(playEscapeRound(candidate, gamePlan, rng))
        {
            result += playerAmount;
            finalPlayers.push_back(candidate);
        }
    }
    return playFinalRound(result, finalPlayers, rng);
}

double Chase::playQuickRound(Player const& player, evol::Rng const& rng)
{
    size_t numQuestions = size_t(int(std::round(rng.fetchNormal(10.5, 0.5, 1).top())));
    std::stack randomProbabilites = rng.fetchUniform(0, 100, numQuestions);
    size_t numAnswered = 0;
    while(not randomProbabilites.empty())
        if(player.answer(randomProbabilites.top()))
            ++numAnswered;
    return numAnswered * 500.0; // 500€
}

bool Chase::playEscapeRound(Player const& candidate, GamePlan const& gamePlan, evol::Rng const& rng)
{
    size_t stepChaser = 8;
    size_t stepCandidate = candidate.deduceStartingStep(gamePlan, rng);
    std::stack<int> randomProbabilites = rng.fetchUniform(0, 100, 25); // 25 Zufallszahlen sollten erstmal garantiert reichen
    auto fetchProb = [&randomProbabilites, &rng](){
        if(randomProbabilites.empty())
            randomProbabilites = rng.fetchUniform(0, 100, 25);
        return randomProbabilites.top(); 
    };
    while(stepChaser > 0 or stepCandidate > 0 or stepChaser > stepCandidate)
    {
        // play candidate question
        if(candidate.answerPossibilities(fetchProb()))
            stepCandidate--;

        // play chaser question
        if(chaser_.answerPossibilities(fetchProb()))
            stepChaser--;
    }
    return stepCandidate == 0 and stepChaser > 0;
}

std::pair<int, int> Chase::playPlayersFinal(std::vector<std::reference_wrapper<const Player>> const& finalPlayers, evol::Rng const& rng, double expectedNumQuestions, double stdDev)
{
    int playerScore = 0;
    size_t numPlayerQuestions = rng.fetchNormal(expectedNumQuestions, stdDev, 1).top();
    std::stack<int> randomProbabilites = rng.fetchUniform(0, 100 , numPlayerQuestions*finalPlayers.size());
    for(size_t i = 0; i < numPlayerQuestions; ++i)
    {
        size_t count = std::count_if(finalPlayers.begin(), finalPlayers.end(), [&randomProbabilites](const auto& player){
            return player.get().answer(randomProbabilites.top());
        });
        if(count > 0)
            playerScore++;
    }
    return { playerScore, numPlayerQuestions };
}

double Chase::playFinalRound(double gainedAmount, std::vector<std::reference_wrapper<const Player>> const& finalPlayers, evol::Rng const& rng)
{
    // play candidates final
    int playerScore = finalPlayers.size(); // candidates start with the amount of candidates as points
    playerScore += playPlayersFinal(finalPlayers, rng, 20.0, 2.0).first;
    // play chaser final
    std::vector<std::reference_wrapper<const Player>> chaserCont = { chaser_ };
    auto[chaserScore, chaserNumQuestions] = playPlayersFinal(chaserCont, rng, 22.0, 2.0); // chasers answers more quickly, therefore more questions

    chaserScore -= playPlayersFinal(finalPlayers, rng, double(chaserNumQuestions-chaserScore), 0.0).first;
    return playerScore > chaserScore ? gainedAmount : 0.0;
}

}