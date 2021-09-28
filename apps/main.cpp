#include <chase/Analysis.h>
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
    chase::SinglePlayerAnalysis analysis;
    chase::singlePlayerAnalysis(analysis);
    chase::dumpSinglePlayerResults(analysis, "SinglePlayerAnalysis.csv");
    return 0;
}

