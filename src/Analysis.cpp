#include "Analysis.h"
#include "Evolution.h"
#include "Chase.h"

namespace chase {

void simpleChaseAnalysis(SimpleAnalysisOptions const& options)
{
    Chase chase(options.playerEquity, options.nbPlayers, options.nbRounds, options.chaserFactor, options.dontPlayFinal);
    std::vector<GamePlan> gamePlans(20);
    std::generate(gamePlans.begin(), gamePlans.end(), [&chase](){
        GamePlan plan;
        for(const Player& candidate : chase.candidates())
            plan.percentages[&candidate] = {};
        return plan;
    } );
    double winningAverageWin = 0;


    auto winningPopulation = evol::evolution(gamePlans, options.nbGenerations, chase, winningAverageWin, options.logLevel);
    std::cout << "\nAvg. win " << winningAverageWin << ", \ngame plan: " << winningPopulation.front().toString() << '\n';
}

}