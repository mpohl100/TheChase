#pragma once

#include "TeamAnalysis.h"

#include <par/Calculation.h>

namespace chase{

struct TeamAnalysisOptionsPar{
    TeamAnalysisOptions options;
    TeamGamePlan gamePlan;
};

class TeamAnalysisPar : par::Calculation<par::SubCalculation<SimpleTeamResult, TeamAnalysisOptionsPar>>{
public:
    TeamAnalysisPar(std::vector<double> playerEquity, double chaserFactor, size_t numThreads);
    virtual void initCalculations() override;
    virtual void initTransformations() override;
private:
    double chaserFactor_;
    std::vector<double> playerEquity_;
};


}