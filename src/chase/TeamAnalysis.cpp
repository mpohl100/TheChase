#include "TeamAnalysis.h"
#include "TeamChase.h"
#include "Stochastics.h"

#include <evol/Evolution.h>

#include <range/v3/all.hpp>
#include <concepts>
#include <fstream>
#include <cmath>


namespace chase {

SimpleTeamResult teamChaseAnalysis(TeamAnalysisOptions const& options, TeamGamePlan& sharedPlan)
{
    auto chase = std::make_shared<TeamChase>(options.playerEquity, options.nbRounds, options.chaserFactor, options.path);
    std::vector<TeamGamePlan> gamePlans;
    for(size_t i = 0; i < 20; ++i)
        gamePlans.push_back(sharedPlan);
    double winningAverageWin = 0;
    auto winningPopulation = evol::evolution(gamePlans, options.nbGenerations, *chase, winningAverageWin, options.logLevel);
    //std::cout << "\nAvg. win " << winningAverageWin << ", \ngame plan: " << winningPopulation.front().toString() << '\n';
    sharedPlan = winningPopulation.front();
    return {winningPopulation.front(), winningAverageWin, chase->numGames()};
}

std::vector<std::vector<int>> calc_paths()
{
    auto steps = std::vector<int>{0,4,5,6};
    auto paths = stoch::all_paths(steps, 3);
    auto paths2 = stoch::all_paths(steps, 2);
    auto paths1 = stoch::all_paths(steps, 1);
    paths.insert(paths.end(), paths2.begin(), paths2.end());
    paths.insert(paths.end(), paths1.begin(), paths1.end());
    paths.push_back({});
    return paths;
}

void teamPlayerAnalysis(TeamPlayerAnalysis& analysis)
{
    size_t numGames = 0;
    auto paths = calc_paths();
    for( double chaserFactor = analysis.analysisRange.chaserFactorParams[0];
        chaserFactor <= analysis.analysisRange.chaserFactorParams[1];
        chaserFactor += analysis.analysisRange.chaserFactorParams[2])
    {
        TeamGamePlan sharedPlan;
        for(const auto& path : paths)
        {
            auto& gamePlan = sharedPlan.plan[path];
            gamePlan.percentage = {};
        }

        for(const auto& path : paths)
        {
            TeamAnalysisOptions options;
            options.path = path;
            options.playerEquity = analysis.analysisRange.playerPercentages;
            options.chaserFactor = chaserFactor;
            options.logLevel = 1;
            options.nbGenerations = 100;
            options.nbRounds = 500;
            SimpleTeamResult result = teamChaseAnalysis(options, sharedPlan);
            numGames += result.numGames;
            std::cout << chaserFactor << " " << toStringPath(path) << '\n';
            std::cout << "\nAvg. win " << result.avgWin << ", \ngame plan: " << result.gamePlan.toString() << '\n';
            analysis.results[{path, chaserFactor}] = {result.gamePlan.plan[path], result.avgWin, result.numGames};
        }
    }
    std::cout << "Nb. of games: " << numGames << '\n';

}

std::string toString(std::vector<int> const& path, std::vector<double> const& playerPercentages)
{
    std::string ret = "[";
    for(size_t i = 0; i < path.size(); ++i)
    {
        ret += std::to_string(path[i]);
        ret += "(" + std::to_string(playerPercentages[i]*100) + "%)|";
    }
    ret = ret.substr(0, ret.size()-1);
    ret += "]";
    return ret;
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
    auto paths = calc_paths();
    for(const auto& path : paths)
    {
        outfile << toString(path, analysis.analysisRange.playerPercentages) << ';';     
        for( double chaserFactor = analysis.analysisRange.chaserFactorParams[0];
            chaserFactor <= analysis.analysisRange.chaserFactorParams[1];
            chaserFactor += analysis.analysisRange.chaserFactorParams[2])
        {
            SimpleResult& result = analysis.results[{path, chaserFactor}];
            auto percentage = result.gamePlan.percentage;
            outfile << result.avgWin << "€|" << percentage.gamble * 100 << "%|" << percentage.stay * 100 << "%|" << (1 - percentage.gamble - percentage.stay) * 100 << "%;";
        }
        outfile << '\n';
    } 
}

}