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
#endif
    chase::TeamPlayerAnalysis analysis;
    analysis.analysisRange.playerPercentages = {0.7, 0.4, 0.4, 0.7};
    analysis.analysisRange.chaserFactorParams = {{1.0, 5.0, 1.0}};
    chase::teamPlayerAnalysisPar(analysis);
    chase::dumpTeamPlayerResults(analysis, "TeamPlayerAnalysis2.csv");
    return 0;
}

