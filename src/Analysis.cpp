#include "Analysis.h"
#include "Evolution.h"
#include "Chase.h"

namespace chase {

void simpleChaseAnalysis(SimpleAnalysisOptions const& options)
{
    std::vector<GamePlan> gamePlans(20);
    std::generate(gamePlans.begin(), gamePlans.end(), [&options](){
        GamePlan plan;
        plan.percentages[options.playerEquity] = {};
        return plan;
    } );
    double winningAverageWin = 0;
    Chase chase(options.playerEquity, options.nbPlayers, options.nbRounds, options.chaserFactor, options.dontPlayFinal);

    auto winningPopulation = evol::evolution(gamePlans, options.nbGenerations, chase, winningAverageWin, options.logLevel);
    std::cout << "\nAvg. win " << winningAverageWin << ", \ngame plan: " << winningPopulation.front().toString() << '\n';
}

}