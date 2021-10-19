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
        calculations_.push_back(par::CalcStep(calcs));
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
                    auto currentPlan = calculations_[i].result(&thisCalc).gamePlan.plan;
                    par.gamePlan.plan.insert(currentPlan.begin(), currentPlan.end());
                }
            }
        };
        transformations_.push_back(func);
    }
}

SimpleTeamResult TeamAnalysisPar::getResult() const
{
    const auto& subCalc = calculations_.cbegin()->subCalcs()[0];
    auto result = calculations_.begin()->result(&subCalc);
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