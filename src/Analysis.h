#pragma once

#include <stddef.h>

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

void simpleChaseAnalysis(SimpleAnalysisOptions const& options);

}