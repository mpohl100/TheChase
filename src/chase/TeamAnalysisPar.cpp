#include "TeamAnalysisPar.h"
#include "Stochastics.h"

namespace chase{

TeamAnalysisPar::TeamAnalysisPar( std::vector<double> playerEquity, double chaserFactor, size_t numThreads)
    : par::Calculation<par::SubCalculation<SimpleTeamResult, TeamAnalysisOptionsPar>>(numThreads)
    , chaserFactor_(chaserFactor) 
    , playerEquity_(playerEquity)
{}

void TeamAnalysisPar::initCalculations()
{
    auto steps = std::vector<int>{0,4,5,6};
    auto depths = std::vector<int>{3,2,1,0};
    for(int depth : depths)
    {
        auto paths = stoch::all_paths(steps, depth);
        if(depth == 0)
            paths = {{}};
        std::vector<par::SubCalculation<SimpleTeamResult, TeamAnalysisOptionsPar>> calcs;
        for(const auto& p : paths)
        {
            TeamAnalysisOptions options;
            options.playerEquity = playerEquity_;
            options.chaserFactor = chaserFactor_;
            options.path = p;
            options.logLevel = 0;
            options.nbGenerations = 100;
            options.nbRounds = 500;
            TeamAnalysisOptionsPar par;
            par.options = options;
            std::function<SimpleTeamResult(TeamAnalysisOptionsPar)> f = [](TeamAnalysisOptionsPar par){
                return teamChaseAnalysis(par.options, par.gamePlan);
            };
            par::SubCalculation<SimpleTeamResult, TeamAnalysisOptionsPar> c(f, par);
            calcs.push_back(c);
        }
        calculations_.emplace_back(calcs);
    }
}

void TeamAnalysisPar::initTransformations()
{
    for(size_t i = 0; i < calculations_.size() - 1; ++i)
    {
        auto func = [i, this](){
            auto& nextCalcStep = calculations_[i+1];
            for(auto& calc : nextCalcStep.subCalcs())
            {
                auto& par = std::get<TeamAnalysisOptionsPar>(calc.args());
                for(auto& thisCalc : calculations_[i].subCalcs())
                {
                    auto& currentPlan = thisCalc.result().gamePlan.plan;
                    par.gamePlan.plan.insert(currentPlan.begin(), currentPlan.end());
                }
            }
        };
        transformations_.push_back(func);
    }
}


}