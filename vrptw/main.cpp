#include <algorithm>
#include <cfloat>
#include <iostream>
#include <fstream>
#include <format>
#include <vector>
#include "include/instance.h"
#include "include/genetic_algorithm.h"

void saveGAResultsToCSV(const ga_results_t& results, const std::string& filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return;
    }

    // file << "Generation,BestFitness,AverageFitness,WorstFitness\n";

    for (size_t i = 0; i < results.generationResults.size(); ++i) {
        const auto& gen = results.generationResults[i];
        file << i << ","
             << gen.bestFitness << ","
             << gen.averageFitness << ","
             << gen.worstFitness << "\n";
    }

    // file << "\nRunTime (seconds)," << results.runTime.count() << "\n";
    // file << "Evaluate Calls," << results.numberOfEvaluateCalls << "\n";

    file.close();
}

void appendInstanceStats(const std::string& filename, const std::string& instanceName, const generationResult& stats) {
    std::ofstream file;
    file.open(filename, std::ios::app);  // open in append mode
    if (file.is_open()) {
        file << instanceName << ","
             << stats.bestFitness << ","
             << stats.averageFitness << ","
             << stats.worstFitness << "\n";
        file.close();
    } else {
        // Handle error opening file if needed
        throw std::runtime_error("Unable to open file: " + filename);
    }
}

template <typename T>
double standardDeviation(const std::vector<T>& data, double mean) {
    double variance = 0.0;
    for (T val : data) {
        variance += (val - mean) * (val - mean);
    }
    variance /= data.size();
    return std::sqrt(variance);
}

int main(int argc, char* argv[]){
    if(argc - 1 != 7){
        std::cerr << "Wrong number of arguments!" << std::endl;
        return 1;
    }

    int maxEvals{};
    int populationSize{};
    int tournamentSize{};
    int elite{};
    float crossoverPropability{};
    float mutationPropability{};
    int runs{};
    ga_parameters_t config_ga{};

    try {
        maxEvals = std::stoi(argv[1]);
        populationSize = std::stoi(argv[2]);
        tournamentSize = std::stoi(argv[3]);
        elite = std::stoi(argv[4]);
        crossoverPropability = std::stof(argv[5]);
        mutationPropability = std::stof(argv[6]);
        runs = std::stoi(argv[7]);

        config_ga = {
            .maxEvals = maxEvals,
            .populationSize = populationSize,
            .tournamentSize = tournamentSize,
            .elite = elite,
            .crossoverPropability = crossoverPropability,
            .mutationPropability = mutationPropability
        };

    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << "\n";
        return 1;
    } catch (const std::out_of_range& e) {
        std::cerr << "Argument out of range: " << e.what() << "\n";
        return 1;
    }

    std::vector<ProblemInstance> instances = readAllProblemInstances("data/");

    ga_results_t results_ga;

    GeneticAlgorithm alg_ga;

    float ga_best_avg = 0;

    int counter = 0;

    for (ProblemInstance instance : instances) {
        // if (counter > 2){ break; }

        std::cerr << instance.getName() << std::endl;

        generationResult ga_avg_results{FLT_MAX, 0, 0};
        float ga_tmp = 0.0f;
        std::vector<float> ga_results;
        for (int i=0; i<runs; i++) {
            std::cerr << "Run " << i+1 << ":" << std::endl;
            results_ga = alg_ga.run(instance, config_ga);
            ga_avg_results.bestFitness = std::min(results_ga.generationResults.back().bestFitness, ga_avg_results.bestFitness);
            ga_avg_results.worstFitness = std::max(results_ga.generationResults.back().bestFitness, ga_avg_results.worstFitness);
            ga_tmp += results_ga.generationResults.back().bestFitness;
            ga_results.push_back(results_ga.generationResults.back().averageFitness);
            saveGAResultsToCSV(results_ga, std::format("results/{:s}_{:d}.txt", instance.getName(), i+1));
        }
        // saveGAResultsToCSV(results_ga, std::format("results/{:s}_1.txt", instance.getName()));
        ga_avg_results.averageFitness = ga_tmp/(float)runs;
        appendInstanceStats("all_results.txt", instance.getName(), ga_avg_results);
        double ga_stddev = standardDeviation(ga_results, ga_avg_results.averageFitness);

        ga_best_avg += ga_avg_results.bestFitness;

        std::cout << std::endl << "GA: " << ga_avg_results.bestFitness << " | " << ga_avg_results.averageFitness << " | " << ga_avg_results.worstFitness << std::endl;

        counter++;
    }

    
}