#pragma once

#include "TeamAnalysis.h"

#include <par/Calculation.h>

namespace chase{

struct TeamAnalysisOptionsPar{
    TeamAnalysisOptions options;
    TeamGamePlan gamePlan;
};

class TeamAnalysisPar : public par::Calculation<par::SubCalculation<SimpleTeamResult, TeamAnalysisOptionsPar>>{
public:
    TeamAnalysisPar(std::vector<double> playerEquity, double chaserFactor, size_t numThreads);
    SimpleTeamResult const& getResult() const;
private:
    virtual void initCalculations() override;
    virtual void initTransformations() override;

    double chaserFactor_;
    std::vector<double> playerEquity_;
};

void teamPlayerAnalysisPar(TeamPlayerAnalysis& analysis);

}