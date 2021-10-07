#include "Chase.h"
#include <evol/Rng.h>

#include <algorithm>
#include <iostream>
#include <cmath>
namespace chase{

void say_hello(std::string name)
{
    std::cout << "Hello, " << name << "!" << '\n';
}

Chase::Chase(double candidateChance, size_t numPlayers, size_t numRounds, double chaserFactor, bool dontPlayFinal)
    : numRounds_(numRounds)
    , chaserFactor_(chaserFactor)
    , dontPlayFinal_(dontPlayFinal)
{
    for(size_t i = 0; i < numPlayers; ++i)
        candidates_.emplace_back(candidateChance, i);
    chaser_ = Player(0.8, 0);
}




double Chase::score(GamePlan const& gamePlan) const
{
    double ret = 0;
    static evol::Rng rng;
    for(size_t i = 0; i < numRounds_; ++i)
        ret += play(gamePlan, rng);
    return ret / double(numRounds_);
}

std::vector<Player> const& Chase::candidates() const
{
    return candidates_;
}

size_t Chase::numGames() const
{
    return numGames_;
}

double Chase::play(GamePlan const& gamePlan, evol::Rng const& rng) const
{
    // copy is a must
    GameState gameState;
    numGames_++;
    // todo refactor to a single player 
    for(size_t i = 0; i < candidates_.size(); ++i)
    {
        const Player& candidate = candidates_[i];
        double playerAmount = playQuickRound(candidate, rng);
        if(playEscapeRound(candidate, gamePlan, rng, playerAmount) > 0)
        {
            gameState.result += playerAmount;
            gameState.finalPlayers.push_back(candidate);
        }
    }
    if(dontPlayFinal_)
        return gameState.result;
    return playFinalRound(gameState, rng);
}


double Chase::playQuickRound(Player const& player, evol::Rng const& rng) const
{
    size_t numQuestions = size_t(int(std::round(rng.fetchNormal(10.5, 0.5, 1).top())));
    std::stack randomProbabilites = rng.fetchUniform(0, 100, numQuestions);
    size_t numAnswered = 0;
    while(not randomProbabilites.empty())
    {
        if(player.answer(randomProbabilites.top()))
            ++numAnswered;
        randomProbabilites.pop();
    }
    double result = numAnswered * 500.0; // 500€
    if(numAnswered == 0)
        result = 500.0;
    return result;
}

size_t Chase::playEscapeRound(Player const& candidate, GamePlan const& gamePlan, evol::Rng const& rng, double& amount) const
{
    size_t stepChaser = 8;
    size_t stepCandidate = candidate.deduceStartingStep(gamePlan, rng);
    size_t ret = stepCandidate;
    switch(stepCandidate)
    {
        case 4: amount *= 1 / chaserFactor_; break;
        case 5: break;
        case 6: amount *= chaserFactor_; break;
        default: break;
    }
    std::stack<int> randomProbabilites = rng.fetchUniform(0, 100, 25); // 25 Zufallszahlen sollten erstmal garantiert reichen
    auto fetchProb = [&randomProbabilites, &rng](){
        if(randomProbabilites.empty())
            randomProbabilites = rng.fetchUniform(0, 100, 25);
        int prob = randomProbabilites.top(); 
        randomProbabilites.pop();
        return prob;
    };
    while(true)
    {
        // play candidate question
        if(candidate.answer(fetchProb()))
            stepCandidate--;

        // play chaser question
        if(chaser_.answer(fetchProb()))
            stepChaser--;
        if(stepChaser == 0 or stepCandidate == 0 or stepChaser <= stepCandidate)
            break;
    }
    return (stepCandidate == 0 and stepChaser > 0) ? ret : 0;
}

std::pair<int, int> Chase::playPlayersFinal(std::vector<std::reference_wrapper<const Player>> const& finalPlayers, evol::Rng const& rng, double expectedNumQuestions, double stdDev) const
{
    int playerScore = 0;
    size_t numPlayerQuestions = rng.fetchNormal(expectedNumQuestions, stdDev, 1).top();
    std::stack<int> randomProbabilites = rng.fetchUniform(0, 100 , numPlayerQuestions*finalPlayers.size());
    for(size_t i = 0; i < numPlayerQuestions; ++i)
    {
        size_t count = std::count_if(finalPlayers.begin(), finalPlayers.end(), [&randomProbabilites](const auto& player){
            bool answer = player.get().answer(randomProbabilites.top());
            randomProbabilites.pop();
            return answer;
        });
        if(count > 0)
            playerScore++;
    }
    return { playerScore, numPlayerQuestions };
}

double Chase::playFinalRound(GameState const& gameState, evol::Rng const& rng) const
{
    if(gameState.finalPlayers.size() == 0)
        return 0.0;
    // play candidates final
    int playerScore = gameState.finalPlayers.size(); // candidates start with the amount of candidates as points
    playerScore += playPlayersFinal(gameState.finalPlayers, rng, 20.0, 2.0).first;
    // play chaser final
    std::vector<std::reference_wrapper<const Player>> chaserCont = { chaser_ };
    auto[chaserScore, chaserNumQuestions] = playPlayersFinal(chaserCont, rng, 22.0, 2.0); // chasers answers more quickly, therefore more questions
    // subtract candidates correct answers
    chaserScore -= playPlayersFinal(gameState.finalPlayers, rng, double(chaserNumQuestions-chaserScore), 0.0).first;
    return playerScore > chaserScore ? gameState.result : 0.0;
}

}