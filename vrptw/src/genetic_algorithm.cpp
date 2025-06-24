#include "../include/genetic_algorithm.h"
#include "../include/instance.h"
#include "../include/evaluate.h"
#include <algorithm>
#include <cfloat>
#include <chrono>
#include <climits>
#include <iostream>
#include <iterator>
#include <numeric>
#include <queue>
#include <random>
#include <format>
#include <ranges>
#include <utility>
#include <vector>
#include <unordered_map>

void GeneticAlgorithm::evaluatePopulation(std::vector<individual_t>& population){
    for (auto& individual : population) {
        if (individual.fitnessValue != 0) {
            continue;
        }
        individual.fitnessValue = evaluateSolution(individual, this->problemInstance.distanceMatrix, this->problemInstance.getCapacity(), this->evaluationCounter);
    }
}

void GeneticAlgorithm::initializePopulation(){
    std::vector<const Node*> randomDepots(this->problemInstance.nodes.size()-1);
    std::vector<int> randomVehicleIds(this->problemInstance.nodes.size()-1);

    std::uniform_int_distribution<> dist(1, 25);
    for (int& val : randomVehicleIds) {
        val = dist(this->gen);
    }

    // go through all nodes except first, which is the depot
    int i=0;
    for (const Node& node : this->problemInstance.nodes | std::views::drop(1)) {
        randomDepots[i] = &node;
        i++;
    }

    // this->population.resize(this->parameters.generations);
    std::vector<individual_t> emptyGeneration(this->parameters.populationSize);
    this->population.push_back(emptyGeneration);
    // this->population[0].resize(this->parameters.populationSize);
    for (individual_t& individual : this->population[0]) {
        individual_t tmp{randomDepots, randomVehicleIds, 0};
        individual = tmp;
        std::shuffle(randomDepots.begin(), randomDepots.end(), this->gen);
        std::shuffle(randomVehicleIds.begin(), randomVehicleIds.end(), this->gen);
    }

    // EVALUATE INITIAL POPULATION
    evaluatePopulation(this->population[0]);
}

bool individualCompare(const individual_t& lhs, const individual_t& rhs){
    return lhs.fitnessValue < rhs.fitnessValue;
}

std::vector<individual_t> GeneticAlgorithm::selectGeneration(std::vector<individual_t> selectionPool){
    std::shuffle(selectionPool.begin(), selectionPool.end(), this->gen);
    std::sort(selectionPool.begin(), selectionPool.end(), individualCompare);

    selectionPool.erase(selectionPool.begin() + this->parameters.populationSize, selectionPool.end());
    return selectionPool;
}

std::pair<individual_t, individual_t> GeneticAlgorithm::selectParents(std::vector<individual_t> selectionPool){
    std::shuffle(selectionPool.begin(), selectionPool.end(), this->gen);
    selectionPool.erase(selectionPool.begin() + this->parameters.tournamentSize, selectionPool.end());

    std::sort(selectionPool.begin(), selectionPool.end(), individualCompare);

    return std::make_pair(selectionPool[0], selectionPool[1]);
}

std::pair<std::vector<const Node*>, std::vector<const Node*>> GeneticAlgorithm::crossoverDepots(std::vector<const Node*> const& parent1, std::vector<const Node*> const& parent2){
    int p = 0, q = 0;
    Node emptyNode;
    Node* emptyNodePtr = &emptyNode;

    std::vector<const Node*> offspring1;
    offspring1.resize(parent1.size(), emptyNodePtr);
    std::vector<const Node*> offspring2;
    offspring2.resize(parent2.size(), emptyNodePtr);

    // get two randim unique points for dividing genes
    std::vector<int> ints(std::min(parent1.size(), parent2.size()));
    std::iota(ints.begin(), ints.end(), 0);
    std::shuffle(ints.begin(), ints.end(), this->gen);
    
    p = std::min(ints[0], ints[1]);
    q = std::max(ints[0], ints[1]);

    // replace middle parts
    std::copy(parent1.begin()+p, parent1.begin()+q, offspring2.begin()+p);
    std::copy(parent2.begin()+p, parent2.begin()+q, offspring1.begin()+p);

    // fill the rest accordingly
    // offspring1
    std::unordered_map<int, size_t> geneIndexMap1;
    for (size_t i=0; i < offspring1.size(); i++) {
        int _id = offspring1[i]->id;
        if (_id != 0) {
            geneIndexMap1[_id] = i;
        }
    }

    int i=0;
    while (i < parent1.size()) {
        // skip middle fragment
        if (i == p) {
            i = q;
        }

        int foundIdx = i;
        int wantedId = parent1[i]->id;

        while (geneIndexMap1.find(wantedId) != geneIndexMap1.end()){
            foundIdx = geneIndexMap1[wantedId];
            wantedId = parent1[foundIdx]->id;
            // if (wantedId == -1) {
            //     break;
            // }
        }

        if (foundIdx != parent1.size()) {
            offspring1[i] = parent1[foundIdx];
        } else {
            offspring1[i] = parent1[foundIdx];
        }

        geneIndexMap1[offspring1[i]->id] = i;
        i++;
    }

    // offspring2
    std::unordered_map<int, size_t> geneIndexMap2;
    for (size_t i=0; i < offspring2.size(); i++) {
        int _id = offspring2[i]->id;
        if (_id != 0) {
            geneIndexMap2[_id] = i;
        }
    }

    i = 0;
    while (i < parent2.size()) {
        // skip middle fragment
        if (i == p) {
            i = q;
        }

        int foundIdx = i;
        int wantedId = parent2[i]->id;

        while (geneIndexMap2.find(wantedId) != geneIndexMap2.end()){
            foundIdx = geneIndexMap2[wantedId];
            wantedId = parent2[foundIdx]->id;
            // if (wantedId == -1) {
            //     break;
            // }
        }

        if (foundIdx != parent2.size()) {
            offspring2[i] = parent2[foundIdx];
        } else {
            offspring2[i] = parent2[foundIdx];
        }

        geneIndexMap2[offspring2[i]->id] = i;
        i++;
    }

    return std::make_pair(offspring1, offspring2);
}

std::pair<std::vector<int>, std::vector<int>> GeneticAlgorithm::crossoverVehicleIds(std::vector<int> const& parent1, std::vector<int> const& parent2){
    int p = 0, q = 0;
    Node emptyNode;
    Node* emptyNodePtr = &emptyNode;

    std::vector<int> offspring1;
    offspring1.resize(parent1.size(), 0);
    std::vector<int> offspring2;
    offspring2.resize(parent2.size(), 0);

    // get two randim unique points for dividing genes
    std::vector<int> ints(std::min(parent1.size(), parent2.size()));
    std::iota(ints.begin(), ints.end(), 0);
    std::shuffle(ints.begin(), ints.end(), this->gen);
    
    p = std::min(ints[0], ints[1]);
    q = std::max(ints[0], ints[1]);

    // replace middle parts
    std::copy(parent1.begin()+p, parent1.begin()+q, offspring2.begin()+p);
    std::copy(parent2.begin()+p, parent2.begin()+q, offspring1.begin()+p);

    // front
    std::copy(parent1.begin(), parent1.begin()+p, offspring2.begin());
    std::copy(parent2.begin(), parent2.begin()+p, offspring1.begin());

    // back
    std::copy(parent1.begin()+q, parent1.end(), offspring2.begin()+q);
    std::copy(parent2.begin()+q, parent2.end(), offspring1.begin()+q);

    return std::make_pair(offspring1, offspring2);
}

std::pair<individual_t, individual_t> GeneticAlgorithm::crossover(individual_t parent1, individual_t parent2){
    individual_t offspring1, offspring2;
    // std::pair<individual_t, individual_t> offsprings = std::make_pair(offspring1, offspring2);
    std::pair<std::vector<const Node *>, std::vector<const Node *>> depots = crossoverDepots(parent1.chromosome.depots, parent2.chromosome.depots);
    std::pair<std::vector<int>, std::vector<int>> vehicleIds = crossoverVehicleIds(parent1.chromosome.vehicleIds, parent2.chromosome.vehicleIds);

    offspring1.chromosome.depots = depots.first;
    offspring1.chromosome.vehicleIds = vehicleIds.first;

    offspring2.chromosome.depots = depots.second;
    offspring2.chromosome.vehicleIds = vehicleIds.second;

    offspring1.fitnessValue = evaluateSolution(offspring1, this->problemInstance.distanceMatrix, this->problemInstance.getCapacity(), this->evaluationCounter);
    offspring2.fitnessValue = evaluateSolution(offspring2, this->problemInstance.distanceMatrix, this->problemInstance.getCapacity(), this->evaluationCounter);

    return std::make_pair(offspring1, offspring2);
}

void GeneticAlgorithm::mutation(individual_t& individual){
    std::vector<int> ints(individual.chromosome.depots.size()-1);
    std::iota(ints.begin(), ints.end(), 0);
    std::shuffle(ints.begin(), ints.end(), this->gen);
    std::swap(individual.chromosome.depots[ints[0]], individual.chromosome.depots[ints[1]]);
    std::swap(individual.chromosome.vehicleIds[ints[2]], individual.chromosome.vehicleIds[ints[3]]);

    // fixSolution(individual);
    individual.fitnessValue = evaluateSolution(individual, this->problemInstance.distanceMatrix, this->problemInstance.getCapacity(), this->evaluationCounter);
}

void GeneticAlgorithm::inversionMutation(individual_t& individial){
    std::vector<int> ints(individial.chromosome.depots.size()-1);
    std::iota(ints.begin(), ints.end(), 0);
    std::shuffle(ints.begin(), ints.end(), this->gen);
    int p1 = std::min(ints[0],ints[1]);
    int q1 = std::max(ints[0],ints[1]);

    int p2 = std::min(ints[2],ints[3]);
    int q2 = std::max(ints[2],ints[3]);

    std::reverse(individial.chromosome.depots.begin()+p1, individial.chromosome.depots.begin()+q1);
    std::reverse(individial.chromosome.vehicleIds.begin()+p2, individial.chromosome.vehicleIds.begin()+q2);

    // std::reverse(individial.chromosome.depots.begin()+p2, individial.chromosome.depots.begin()+q2);
    // std::reverse(individial.chromosome.vehicleIds.begin()+p2, individial.chromosome.vehicleIds.begin()+q2);
}

bool timeCompare(const Node* lhs, const Node* rhs){
    return lhs->readyTime < rhs->readyTime;
}

void GeneticAlgorithm::timeBalance(individual_t& individual){
    auto routes = depotsToRoutes(individual.chromosome);

    for (auto& route : routes) {
        std::sort(route.begin(), route.end(), timeCompare);    
    }

    individual.chromosome.depots = routesToDepots(routes, individual.chromosome.vehicleIds);
    individual.fitnessValue = evaluateSolution(individual, this->problemInstance.distanceMatrix, this->problemInstance.getCapacity(), this->evaluationCounter);
}

void GeneticAlgorithm::checkForCloserRoutes(individual_t& individual){
    int neighbourCount = this->problemInstance.getDimension() / 10;
    // std::vector<std::pair<const Node*, int>> neighbours(neighbourCount);
    std::vector<std::pair<int, int>> neighbours(neighbourCount);
    
    int i=0;
    for (auto& depot : individual.chromosome.depots) {
        // neighbours.clear();

        // auto distances = this->problemInstance.distanceMatrix[depot->id];
        // distances.erase(distances.begin()); // remove home node

        // using Pair = std::pair<float, int>;

        // auto cmp = [](const Pair& a, const Pair& b) {
        //     return a.first < b.first;
        // };

        // std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> maxHeap(cmp);

        // for (int i = 0; i < distances.size(); ++i) {
        //     maxHeap.emplace(distances[i], i);
        //     if (maxHeap.size() > neighbourCount) {
        //         maxHeap.pop();
        //     }
        // }

        // while (!maxHeap.empty()) {
        //     neighbours.emplace_back(&this->problemInstance.getNodes()[maxHeap.top().second], individual.chromosome.vehicleIds[i]);
        //     maxHeap.pop();
        // }

        // std::reverse(neighbours.begin(), neighbours.end());

        // neighbours = this->problemInstance.adjacencyMatrix[depot->id];
        // neighbours.erase(neighbours.begin()+neighbourCount, neighbours.end());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        if(dist(gen) < 1){

            for (int j=0; j<neighbourCount; j++) {
                neighbours[j].first = this->problemInstance.adjacencyMatrix[depot->id][j];
                int target = neighbours[j].first;
                auto it = std::find_if(individual.chromosome.depots.begin(), individual.chromosome.depots.end(), [target](const Node* node) {
                    return node->id == target;
                });
                int index = std::distance(individual.chromosome.depots.begin(), it);
                if(index > 98){
                    std::cout << "t\n";
                }
                neighbours[j].second = individual.chromosome.vehicleIds[index];
            }
            
            // count which route occurs the most in the vicinity
            std::unordered_map<int, int> freq;

            for (auto& node : neighbours) {
                ++freq[node.second];
            }

            int mostFrequentRoute = neighbours[0].second;
            int maxCount = freq[mostFrequentRoute];

            for (const auto& [routeId, count] : freq) {
                if (count > maxCount) {
                    mostFrequentRoute = routeId;
                    maxCount = count;
                }
            }

            // move point to another route (i.e. change vehicleId to the new route)
            if (mostFrequentRoute > 21800){
                std::cout<< "t\n";
            }
            individual.chromosome.vehicleIds[i] = mostFrequentRoute;

            i++;
        }
    }
}

std::vector<std::vector<const Node*>> GeneticAlgorithm::depotsToRoutes(const struct chromosome& chromosome){
    std::vector<std::vector<const Node*>> routes(25);
    
    for (size_t i=0; i<chromosome.depots.size(); i++) {
        routes[chromosome.vehicleIds[i]-1].push_back(chromosome.depots[i]);
    }

    return routes;
}

std::vector<const Node*> GeneticAlgorithm::routesToDepots(std::vector<std::vector<const Node*>>& routes, std::vector<int>& vehicleIds){
    std::vector<const Node*> depots(vehicleIds.size());

    for (size_t i=0; i<vehicleIds.size(); i++) {
        depots[i] = routes[vehicleIds[i]-1].front();
        routes[vehicleIds[i]-1].erase(routes[vehicleIds[i]-1].begin());
    }

    return depots;
}

generationResult GeneticAlgorithm::summarizePopulation(std::vector<individual_t> const& population) const {
    float totalFitness = 0;
    float bestFitness = FLT_MAX;
    float worstFitness = 0;
    for (const auto& individual : population) {
        if (individual.fitnessValue < bestFitness) {
            bestFitness = individual.fitnessValue;
        } else if (individual.fitnessValue > worstFitness) {
            worstFitness = individual.fitnessValue;
        }
        totalFitness += individual.fitnessValue;
    }
    float averageFitness = totalFitness / population.size();

    return {bestFitness, averageFitness, worstFitness};
}

void GeneticAlgorithm::elite(std::vector<individual_t>& population, std::vector<individual_t> prevPopulation){
    std::sort(prevPopulation.begin(), prevPopulation.end(), individualCompare);
    for (int i=0; i<this->parameters.elite; i++) {
        population.push_back(prevPopulation[i]);
    }
}

ga_results_t GeneticAlgorithm::run(ProblemInstance const& _problem, ga_parameters_t& _parameters){
    this->population.clear();
    this->evaluationCounter = 0;
    this->problemInstance = _problem;
    this->parameters = _parameters;
    std::vector<generationResult> allGenResults; //(this->parameters.generations);

    auto start = std::chrono::high_resolution_clock::now();

    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    initializePopulation();
    // std::cout << "Population initialized!" << std::endl;

    int generation = 0;
    // for (generation = 0; generation < this->parameters.generations-1; generation++) {
    while (this->evaluationCounter < this->parameters.maxEvals){
        
        
        allGenResults.push_back(summarizePopulation(this->population[generation]));

        if(generation % 20 == 0){
            generationResult lastGen = allGenResults.back();
            std::cerr << std::format("Generation {:>3}:      ", generation);
            std::cerr << std::format("Best: {:<5.0f}   Average: {:<5.0f}   Worst: {:<5.0f}\n", lastGen.bestFitness, lastGen.averageFitness, lastGen.worstFitness);
        }

        std::vector<individual_t> selectionPool;

        // std::cerr<<"generation:  "<<generation<<std::endl;

        elite(selectionPool, this->population[generation]);

        while (selectionPool.size() <= this->parameters.populationSize) {
            // std::cerr<<"selection   "<<this->evaluationCounter<<std::endl;
            // std::pair<individual_t, individual_t> parents = selectParents(this->population[generation]);
            std::pair<individual_t, individual_t> children = selectParents(this->population[generation]);

            if (dist(gen) < this->parameters.crossoverPropability) {
                children = crossover(children.first, children.second);
    
                // if (dist(gen) < this->parameters.mutationPropability) {
                //     // inversionMutation(children.first);
                //     mutation(children.first);
                // }
        
                // if (dist(gen) < this->parameters.mutationPropability) {
                //     // inversionMutation(children.second);
                //     mutation(children.second);
                // }
    
                // selectionPool.push_back(children.first);
                // selectionPool.push_back(children.second);

                // timeBalance(children.first);
                // timeBalance(children.second);

                // checkForCloserRoutes(children.first);
                // checkForCloserRoutes(children.second);
            }

            if (dist(gen) < this->parameters.mutationPropability) {
                // inversionMutation(children.first);
                mutation(children.first);

                // timeBalance(children.first);
                // checkForCloserRoutes(children.first);
            }
    
            if (dist(gen) < this->parameters.mutationPropability) {
                // inversionMutation(children.second);
                mutation(children.second);

                // timeBalance(children.second);
                // checkForCloserRoutes(children.second);
            }

            // operating on separated routes here
            // additional steps

            // std::cerr << "timeBalance:\n";

            if(dist(gen) < 0.3){
                timeBalance(children.first);
                timeBalance(children.second);

                mutation(children.first);
                mutation(children.second);
            }
            

            // std::cerr << "closerRoutes:\n";
            if(dist(gen) < 0.3){
                checkForCloserRoutes(children.first);
                checkForCloserRoutes(children.second);

                mutation(children.first);
                mutation(children.second);
            }

            selectionPool.push_back(children.first);
            selectionPool.push_back(children.second);
        }

        // for (auto& individual : selectionPool) {
        //     if (dist(gen) < this->parameters.mutationPropability) {
        //         mutation(individual);
        //     }
        // }

        // std::cout << "DONE!" << std::endl;

        // this->population[generation+1] = selectGeneration(selectionPool);
        this->population.push_back(selectionPool);
        generation++;
    }
    // allGenResults[generation] = summarizePopulation(this->population[generation]);

    // std::cout << "All generations done!" << std::endl;

    auto stop = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> time = stop - start;

    return {allGenResults, time, this->evaluationCounter};
}