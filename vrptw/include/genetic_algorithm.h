#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

#include "instance.h"
#include <chrono>
#include <random>
#include <vector>

typedef struct ga_parameters_t{
    // int generations;
    int maxEvals;
    int populationSize;
    int tournamentSize;
    int elite;
    float crossoverPropability;
    float mutationPropability;
} ga_parameters_t;

struct generationResult{
    float bestFitness;
    float averageFitness;
    float worstFitness;
};

typedef struct ga_results_t{
    std::vector<struct generationResult> generationResults;
    std::chrono::duration<double> runTime;
    int numberOfEvaluateCalls;
} ga_results_t;

struct chromosome{
    std::vector<const Node*> depots;
    std::vector<int> vehicleIds;
};

typedef struct individual_t{
    struct chromosome chromosome;
    float fitnessValue = 0;
} individual_t;

class GeneticAlgorithm{
    private:
        std::mt19937 gen;
        struct Node* sepNode;
        std::vector<std::vector<individual_t>> population;
        int evaluationCounter = 0;
        ProblemInstance problemInstance;
        ga_parameters_t parameters;

        void initializePopulation();
        void mutation(individual_t& individual);
        void inversionMutation(individual_t& individial);
        // void fixSolution(individual_t& individual);
        void evaluatePopulation(std::vector<individual_t>& population);
        void elite(std::vector<individual_t>& population, std::vector<individual_t> prevPopulation);
        void timeBalance(individual_t& individual);
        void checkForCloserRoutes(individual_t& individual);
        std::vector<const Node*> routesToDepots(std::vector<std::vector<const Node*>>& routes, std::vector<int>& vehicleIds);
        std::vector<std::vector<const Node*>> depotsToRoutes(const struct chromosome& chromosome);
        generationResult summarizePopulation(std::vector<individual_t> const& population) const;
        std::pair<std::vector<const Node*>, std::vector<const Node*>> crossoverDepots(std::vector<const Node*> const& parent1, std::vector<const Node*> const& parent2);
        std::pair<std::vector<int>, std::vector<int>> crossoverVehicleIds(std::vector<int> const& parent1, std::vector<int> const& parent2);
        std::pair<individual_t, individual_t> crossover(individual_t parent1, individual_t parent2);
        std::vector<individual_t> selectGeneration(std::vector<individual_t> selectionPool);
        std::pair<individual_t, individual_t> selectParents(std::vector<individual_t> selectionPool);
        
    public:
        ga_results_t run(ProblemInstance const& _problem, ga_parameters_t& _parameters);
        GeneticAlgorithm() : gen(std::random_device{}()), sepNode(new struct Node({-1, 0, 0, 0})) {};
        ~GeneticAlgorithm(){ delete sepNode; };
};

#endif