#include "Analysis.h"
#include "Chase.h"

#include <evol/Evolution.h>

#include <fstream>

namespace chase {

SimpleResult simpleChaseAnalysis(SimpleAnalysisOptions const& options)
{
    auto chase = std::make_shared<Chase>(options.playerEquity, options.nbPlayers, options.nbRounds, options.chaserFactor, options.dontPlayFinal);
    std::vector<GamePlan> gamePlans(20);
    std::generate(gamePlans.begin(), gamePlans.end(), [](){
        GamePlan plan;
        plan.percentage = {};
        return plan;
    } );
    double winningAverageWin = 0;


    auto winningPopulation = evol::evolution(gamePlans, options.nbGenerations, *chase, winningAverageWin, options.logLevel);
    //std::cout << "\nAvg. win " << winningAverageWin << ", \ngame plan: " << winningPopulation.front().toString() << '\n';
    return {winningPopulation.front(), winningAverageWin, chase->numGames()};
}

void singlePlayerAnalysis(SinglePlayerAnalysis& analysis)
{
    size_t numGames = 0;
    for( double playerEquity = analysis.analysisRange.playerEquityParams[0]; 
        playerEquity <= analysis.analysisRange.playerEquityParams[1];
        playerEquity += analysis.analysisRange.playerEquityParams[2])
    {
        for( double chaserFactor = analysis.analysisRange.chaserFactorParams[0];
            chaserFactor <= analysis.analysisRange.chaserFactorParams[1];
            chaserFactor += analysis.analysisRange.chaserFactorParams[2])
        {
            SimpleAnalysisOptions options;
            options.dontPlayFinal = true;
            options.playerEquity = playerEquity;
            options.nbPlayers = 1;
            options.chaserFactor = chaserFactor;
            options.logLevel = 1;
            options.nbGenerations = 100;
            options.nbRounds = 500;
            SimpleResult result = simpleChaseAnalysis(options);
            numGames += result.numGames;
            std::cout << "\nAvg. win " << result.avgWin << ", \ngame plan: " << result.gamePlan.toString() << '\n';
            analysis.results[{playerEquity, chaserFactor}] = result;
        }
    }
    std::cout << "Nb. of games: " << numGames << '\n';
}

void dumpSinglePlayerResults(SinglePlayerAnalysis& analysis, std::string const& filename)
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

    for( double playerEquity = analysis.analysisRange.playerEquityParams[0]; 
        playerEquity <= analysis.analysisRange.playerEquityParams[1];
        playerEquity += analysis.analysisRange.playerEquityParams[2])
    {
        outfile << playerEquity << ';';
        for( double chaserFactor = analysis.analysisRange.chaserFactorParams[0];
            chaserFactor <= analysis.analysisRange.chaserFactorParams[1];
            chaserFactor += analysis.analysisRange.chaserFactorParams[2])
        {
            SimpleResult& result = analysis.results[{playerEquity, chaserFactor}];
            auto percentage = result.gamePlan.percentage;
            outfile << result.avgWin << "€|" << percentage.gamble * 100 << "%|" << percentage.stay * 100 << "%|" << (1 - percentage.gamble - percentage.stay) * 100 << "%;";
        }
        outfile << '\n';
    }
}


}