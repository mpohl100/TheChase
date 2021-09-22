#pragma once

#include "Chase.h"

#include <stddef.h>
#include <map>

namespace chase{

struct SimpleAnalysisOptions{
    double playerEquity = 0.5;
    size_t nbPlayers = 1;
    size_t nbRounds = 500;
    int nbGenerations = 100;
    int logLevel = 2;
    double chaserFactor = 3.0;
    bool dontPlayFinal = false;
};

struct SimpleResult{
    GamePlan gamePlan;
    double avgWin;
    size_t numGames = 0;
};

SimpleResult simpleChaseAnalysis(SimpleAnalysisOptions const& options);

struct SinglePlayerAnalysis{
    struct AnalysisRange{
        std::array<double,3> playerEquityParams = {{0, 1.0, 0.1}};
        std::array<double,3> chaserFactorParams = {{1.0, 5.0, 0.5}};
    };
    AnalysisRange analysisRange;
    std::map<std::pair<double, double>, SimpleResult> results;
};

void singlePlayerAnalysis(SinglePlayerAnalysis& analysis);

void dumpSinglePlayerResults(SinglePlayerAnalysis& analysis, std::string const& filename);

}