#include "TeamAnalysis.h"

#include <evol/Evolution.h>
#include <fstream>

namespace chase {

SimpleResult teamChaseAnalysis(TeamAnalysisOptions const& options)
{
    auto chase = std::make_shared<Chase>(options.playerEquity, options.nbRounds, options.chaserFactor, options.path);
    std::vector<GamePlan> gamePlans(20);
    std::generate(gamePlans.begin(), gamePlans.end(), [&chase](){
        GamePlan plan;
        plan.chase = chase;
        for(const Player& candidate : chase->candidates())
            plan.percentages[candidate.index()] = {};
        return plan;
    } );
    double winningAverageWin = 0;


    auto winningPopulation = evol::evolution(gamePlans, options.nbGenerations, *chase, winningAverageWin, options.logLevel);
    //std::cout << "\nAvg. win " << winningAverageWin << ", \ngame plan: " << winningPopulation.front().toString() << '\n';
    return {winningPopulation.front(), winningAverageWin, chase->numGames()};
}

void teamPlayerAnalysis(TeamPlayerAnalysis& analysis)
{
    size_t numGames = 0;
    // todo for loop over all possible paths
    for( double chaserFactor = analysis.analysisRange.chaserFactorParams[0];
        chaserFactor <= analysis.analysisRange.chaserFactorParams[1];
        chaserFactor += analysis.analysisRange.chaserFactorParams[2])
    {
        TeamAnalysisOptions options;
        options.playerEquity = analysis.analysisRange.playerPercentages;
        options.chaserFactor = chaserFactor;
        options.logLevel = 1;
        options.nbGenerations = 100;
        options.nbRounds = 500;
        SimpleResult result = teamChaseAnalysis(options);
        numGames += result.numGames;
        std::cout << "\nAvg. win " << result.avgWin << ", \ngame plan: " << result.gamePlan.toString() << '\n';
        analysis.results[chaserFactor] = result;
    }
    std::cout << "Nb. of games: " << numGames << '\n';

}

void dumpTeamPlayerResults(TeamPlayerAnalysis& analysis, std::string const& filename)
{
    std::ofstream outfile(filename.c_str());
    // header line
    outfile << ";";
    for( double chaserFactor = analysis.analysisRange.chaserFactorParams[0];
        chaserFactor <= analysis.analysisRange.chaserFactorParams[1];
        chaserFactor += analysis.analysisRange.chaserFactorParams[2])
    {
        outfile << chaserFactor << ';';
    }
    outfile << '\n';    
}

}