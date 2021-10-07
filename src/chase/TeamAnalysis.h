#pragma once

#include "Analysis.h"
#include "TeamGamePlan.h"

#include <vector>

namespace chase {

struct TeamAnalysisOptions{
    std::vector<double> playerEquity;
    size_t nbRounds = 500;
    int nbGenerations = 100;
    int logLevel = 2;
    double chaserFactor = 3.0;
    bool dontPlayFinal = false;
    std::vector<int> path;   
};

struct SimpleTeamResult{
    TeamGamePlan gamePlan;
    double avgWin;
    size_t numGames = 0;
};

SimpleTeamResult teamChaseAnalysis(TeamAnalysisOptions const& options);

struct TeamPlayerAnalysis{
    struct AnalysisRange{
        std::vector<double> playerPercentages;
        std::array<double,3> chaserFactorParams = {{1.0, 5.0, 0.5}};
    };
    AnalysisRange analysisRange;
    std::map<std::pair<std::vector<int>, double>, SimpleResult> results;
};

void teamPlayerAnalysis(TeamPlayerAnalysis& analysis);

void dumpTeamPlayerResults(TeamPlayerAnalysis& analysis, std::string const& filename);


}