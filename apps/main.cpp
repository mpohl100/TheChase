#include <chase/TeamAnalysisPar.h>
#include <clara.hpp>

#include <iostream>

int main(int argc, char** argv)
{
    using namespace clara;


    std::string name;
    bool help = false;
    auto cli = Opt(name, "name")["-n"]["--name"]("name to greet") | Help(help);
     

    auto result = cli.parse(Args(argc, argv));
    if (!result) {
        std::cerr << "Error in command line: " << result.errorMessage() << '\n';
        exit(1);
    }
    if (help) {
        std::cout << cli;
        exit(0);
    }

#if 0
    chase::SimpleAnalysisOptions options;
    options.dontPlayFinal = true;
    options.nbGenerations = 500;
    chase::simpleChaseAnalysis(options);
#else
    std::vector<std::vector<double>> playerPercentages = {
                                                          {0.4, 0.7, 0.4, 0.4},
                                                          {0.4, 0.4, 0.7, 0.4},
                                                          {0.4, 0.4, 0.4, 0.7},
                                                          {0.6, 0.4, 0.4, 0.4},
                                                          {0.4, 0.6, 0.4, 0.4},
                                                          {0.4, 0.4, 0.6, 0.4},
                                                          {0.4, 0.4, 0.4, 0.6},
                                                          {0.5, 0.4, 0.4, 0.4},
                                                          {0.4, 0.5, 0.4, 0.4},
                                                          {0.4, 0.4, 0.5, 0.4},
                                                          {0.4, 0.4, 0.4, 0.5},
                                                          {0.7, 0.5, 0.5, 0.5},
                                                          {0.5, 0.7, 0.5, 0.5},
                                                          {0.5, 0.5, 0.7, 0.5},
                                                          {0.5, 0.5, 0.5, 0.7},
                                                          {0.6, 0.5, 0.5, 0.5},
                                                          {0.5, 0.6, 0.5, 0.5},
                                                          {0.5, 0.5, 0.6, 0.5},
                                                          {0.5, 0.5, 0.5, 0.6},
                                                          {0.7, 0.6, 0.6, 0.6},
                                                          {0.6, 0.7, 0.6, 0.6},
                                                          {0.6, 0.6, 0.7, 0.6},
                                                          {0.6, 0.6, 0.6, 0.7}};
    for(const auto& playerPercentage : playerPercentages)
    {
        chase::TeamPlayerAnalysis analysis;
        analysis.analysisRange.playerPercentages = playerPercentage;
        analysis.analysisRange.chaserFactorParams = {{2.0, 5.0, 1.0}};
        chase::teamPlayerAnalysisPar(analysis);
        chase::dumpTeamPlayerResults(analysis, std::string("Team" + chase::toStringPath(playerPercentage) + ".csv"));
    }
#endif
    return 0;
}

