#include "TeamAnalysisPar.h"
#include "Stochastics.h"

#include <iostream>

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
        TeamGamePlan sharedPlan;
        for(const auto& p : paths)
        {
            auto& plan = sharedPlan.plan[p];
            plan.percentage = {};
        }
        for(const auto& p : paths)
        {
            TeamAnalysisOptions options;
            options.playerEquity = playerEquity_;
            options.chaserFactor = chaserFactor_;
            options.path = p;
            options.logLevel = 1;
            options.nbGenerations = 100;
            options.nbRounds = 500;
            TeamAnalysisOptionsPar par;
            par.options = options;
            par.gamePlan = sharedPlan;
            std::function<SimpleTeamResult(TeamAnalysisOptionsPar)> f = [](TeamAnalysisOptionsPar par){
                SimpleTeamResult result = teamChaseAnalysis(par.options, par.gamePlan);
                TeamGamePlan& plan = result.gamePlan;
                std::cout << result.avgWin << ' ' << plan.toString() << '\n';
                return result;
            };
            par::SubCalculation<SimpleTeamResult, TeamAnalysisOptionsPar> c(f, par);
            calcs.push_back(c);
        }
        calculations_.push_back(par::CalcStep(calcs));
    }
}

void TeamAnalysisPar::initTransformations()
{
    for(size_t i = 0; i < calculations_.size() - 1; ++i)
    {
        auto func = [i, this](){
            TeamGamePlan sharedPlan;
                            
            for(size_t j = 0; j <= i; j++)
            {
                size_t index = 0;
                for(auto& thisCalc : calculations_[j].subCalcs())
                {
                    SimpleTeamResult result = calculations_[i].result(index++);
                    auto& options = std::get<TeamAnalysisOptionsPar>(thisCalc.args()).options;
                    sharedPlan.plan[options.path] = result.gamePlan.plan[options.path];
                }  
            }              
            auto& nextCalcStep = calculations_[i+1];
            for(auto& calc : nextCalcStep.subCalcs())
            {
                auto& par = std::get<TeamAnalysisOptionsPar>(calc.args());   
                par.gamePlan.plan.insert(sharedPlan.plan.begin(), sharedPlan.plan.end());
            }
        };
        transformations_.push_back(func);
    }
}

SimpleTeamResult TeamAnalysisPar::getResult() const
{
    auto result = calculations_.rbegin()->result(0);
    return result;
}

void teamPlayerAnalysisPar(TeamPlayerAnalysis& analysis)
{
    for( double chaserFactor = analysis.analysisRange.chaserFactorParams[0];
        chaserFactor <= analysis.analysisRange.chaserFactorParams[1];
        chaserFactor += analysis.analysisRange.chaserFactorParams[2])
    {
        auto analyser = TeamAnalysisPar(analysis.analysisRange.playerPercentages, chaserFactor, 4);
        analyser.init();
        analyser.calculate();
        SimpleTeamResult result = analyser.getResult();
        for(const auto& [path, gamePlan] : result.gamePlan.plan)
            analysis.results[{path, chaserFactor}] = { gamePlan, result.avgWin, result.numGames };
    }
}


}