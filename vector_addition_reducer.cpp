#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

#include "src/entities/arg_parser.h"
#include "src/vector_reducer.h"

std::vector<std::vector<int>> readTargets(const std::string &path) {
    std::ifstream f(path);
    if (!f) {
        std::cerr << "Unable open file \"" << path << "\"" << std::endl;
        return {};
    }

    int dimension;
    int targetsCount;
    f >> dimension >> targetsCount;

    std::vector<std::vector<int>> targets(targetsCount, std::vector<int>(dimension));
    for (int i = 0; i < targetsCount; i++)
        for (int j = 0; j < dimension; j++)
            f >> targets[i][j];

    f.close();
    return targets;
}

int main(int argc, char **argv) {
    ArgParser parser("vector_addition_reducer", "Reduce number of additions in linear combinations");
    parser.add("--input-path", "-i", ArgType::String, "Path to file with expressions", "", true);

    parser.addSection("Strategies parameters");
    parser.add("--max-abs-value", ArgType::Natural, "Max absolute vectors value (0 for unlimited)", "0");
    parser.add("--cover-weight", "-cw", ArgType::Real, "Weight for hit cover", "1000");
    parser.add("--one-step-weight", "-osw", ArgType::Real, "Weight for one step cover", "100");
    parser.add("--hamming-weight", "-hw", ArgType::Real, "Weight for Hamming distance", "5");
    parser.add("--matches-weight", "-mw", ArgType::Real, "Weight for matches count", "3");

    parser.addSection("Other parameters");
    parser.add("--print-task", ArgType::Flag, "Print readed task");
    parser.add("--verbose", "-v", ArgType::Flag, "Print verbose info during reducing");

    if (!parser.parse(argc, argv))
        return -1;

    std::string inputPath = parser["--input-path"];
    ReduceParameters parameters;
    parameters.maxAbsValue = std::stod(parser["--max-abs-value"]);
    parameters.coverWeight = std::stod(parser["--cover-weight"]);
    parameters.oneStepWeight = std::stod(parser["--one-step-weight"]);
    parameters.hammingWeight = std::stod(parser["--hamming-weight"]);
    parameters.matchesWeight = std::stod(parser["--matches-weight"]);
    parameters.verbose = parser.isSet("--verbose");

    std::cout << "Parsed parameters:" << std::endl;
    std::cout << "- input path: " << inputPath << std::endl;
    std::cout << "- max abs value: " << parameters.maxAbsValue << std::endl;
    std::cout << "- cover weight: " << parameters.coverWeight << std::endl;
    std::cout << "- one step weight: " << parameters.oneStepWeight << std::endl;
    std::cout << "- Hamming weight: " << parameters.hammingWeight << std::endl;
    std::cout << "- matches weight: " << parameters.matchesWeight << std::endl;
    std::cout << std::endl;

    std::vector<std::vector<int>> targets = readTargets(inputPath);
    if (targets.empty())
        return -1;

    VectorReducer reducer;
    reducer.setTargets(targets);

    if (parser.isSet("--print-task"))
        reducer.printTask();

    auto startTime = std::chrono::high_resolution_clock::now();
    int additions = reducer.reduce(parameters);
    auto endTime = std::chrono::high_resolution_clock::now();

    double seconds = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0;

    std::cout << std::endl;
    std::cout << "Additions: " << additions << std::endl;
    std::cout << "Elapsed time: " << seconds << " seconds" << std::endl;
    return 0;
}
