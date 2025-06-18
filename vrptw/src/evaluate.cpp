#include "../include/evaluate.h"
#include "../include/instance.h"
#include "genetic_algorithm.h"

#include <iostream>
//                      std::vector<const Node*> const& solution,
float evaluateSolution(individual_t const& solution, std::vector<std::vector<float>> const& distanceMatrix, const int& truckCapacity, int& counter){
    counter++;

    float penalty = 0;
    float time = 0;
    int routeDemand = 0;
    int excees = 0;

    // int i = 0;
    // if (solution[0]->id == -1) {
    //     i = 1;
    // }
    // while (i < solution.size()) {
    //     // start from depot
    //     time += distanceMatrix[1][solution[i]->id];
    //     routeDemand += solution[i]->demand;
    //     i++;

    //     while (i < solution.size() && solution[i]->id != -1) {
    //         time += distanceMatrix[solution[i-1]->id][solution[i]->id];
    //         routeDemand += solution[i]->demand;
    //         i++;
    //     }
        
    //     // finish in depot
    //     time += distanceMatrix[solution[i-1]->id][1];

    //     if (routeDemand > truckCapacity) {
    //         int excess = routeDemand - truckCapacity;
    //         penalty += excees * excees;
    //     }

    //     routeDemand = 0;
    //     i++;
    // }

    std::vector<int> currentCapacity(25, 0);

    std::vector<const Node*> solGen = solution.chromosome.depots;
    std::vector<int> solVeh = solution.chromosome.vehicleIds;

    std::vector<std::vector<const Node*>> routes(25);

    int i = 0;
    int currentDemand = 0;
    int waitingTime = 0;
    int lateTime = 0;
    int totalTime = 0;
    int routeTime = 0;

    // divide into routes for each vehicle
    for (int j=0; j<solGen.size(); j++) {
        routes[solVeh[j]-1].push_back(solGen[j]);
    }

    for (const auto& route : routes) {
        // waitingTime = 0;
        // lateTime = 0;
        routeTime = 0;

        while(i < route.size()) {
            //start from depot
            routeTime += distanceMatrix[1][route[i]->id];
            if (routeTime < route[i]->readyTime) { //przed otwarciem
                waitingTime = route[i]->readyTime - routeTime;
                penalty += waitingTime/2.0;
                routeTime += waitingTime;
            } else if (routeTime > route[i]->dueTime) { //po zamknieciu
                lateTime = routeTime - route[i]->dueTime;
                // penalty += ((lateTime/2.0) * (lateTime/2.0));
                penalty += lateTime/2.0;
            }
            currentDemand += route[i]->demand;
            routeTime += route[i]->serviceTime;
            totalTime += route[i]->serviceTime;
            i++;

            while (i < route.size()) {
                currentDemand += route[i]->demand;
                if (currentDemand > truckCapacity) {
                    // jak za duzy ladunek to koniec trasy
                    counter++;
                    break;
                }
                routeTime += distanceMatrix[route[i-1]->id][route[i]->id];
                if (routeTime < route[i]->readyTime) { //przed otwarciem
                    waitingTime = route[i]->readyTime - routeTime;
                    penalty += waitingTime/2.0;
                    routeTime += waitingTime;
                } else if (routeTime > route[i]->dueTime) { //po zamknieciu
                    lateTime = routeTime - route[i]->dueTime;
                    // penalty += ((lateTime/2.0) * (lateTime/2.0));
                    penalty += lateTime/2.0;
                }
                routeTime += route[i]->serviceTime;
                totalTime += route[i]->serviceTime;
                i++;
            }

            // do {
            //     time += distanceMatrix[solution[i-1]->id][solution[i]->id];
            //     currentDemand += solution[i]->demand;
            //     i++;
            // }while (currentDemand <= truckCapacity);
            
            routeTime += distanceMatrix[route[i-1]->id][1];
            currentDemand = 0;

            
        }
        i=0;

        //if (counter % 100 == 0){
            //std::cerr << routeTime << std::endl;
        //}

        time += routeTime;
    }
    // std::cerr << "service time:  " <<totalTime <<std::endl;

    return time + penalty;

    // while(i < solGen.size()) {
    //     // start from depot
    //     time += distanceMatrix[1][solGen[i]->id];
    //     if (time < solGen[i]->readyTime) { //przed otwarciem
    //         waitingTime = solGen[i]->readyTime - time;
    //         penalty += waitingTime/2.0;
    //         time += waitingTime;
    //     } else if (time > solGen[i]->dueTime) { //po zamknieciu
    //         lateTime = time - solGen[i]->dueTime;
    //         // penalty += ((lateTime/2.0) * (lateTime/2.0));
    //         penalty += lateTime/2.0;
    //     }
    //     currentCapacity[solVeh[i]] += solGen[i]->demand;
    //     time += solGen[i]->serviceTime;
    //     i++;

    //     while (i < solGen.size()) {
    //         currentCapacity[solVeh[i]] += solGen[i]->demand;
    //         if (currentCapacity[solVeh[i]] > truckCapacity) {
    //             // jak za duzy ladunek to koniec trasy
    //             counter++;
    //             break;
    //         }
    //         time += distanceMatrix[solGen[i-1]->id][solGen[i]->id];
    //         if (time < solution[i]->readyTime) { //przed otwarciem
    //             waitingTime = solution[i]->readyTime - time;
    //             penalty += waitingTime/2.0;
    //             time += waitingTime;
    //         } else if (time > solution[i]->dueTime) { //po zamknieciu
    //             lateTime = time - solution[i]->dueTime;
    //             // penalty += ((lateTime/2.0) * (lateTime/2.0));
    //             penalty += lateTime/2.0;
    //         }
    //         time += solution[i]->serviceTime;
    //         i++;
    //     }

    //     // do {
    //     //     time += distanceMatrix[solution[i-1]->id][solution[i]->id];
    //     //     currentDemand += solution[i]->demand;
    //     //     i++;
    //     // }while (currentDemand <= truckCapacity);
        
    //     time += distanceMatrix[solution[i-1]->id][1];
    //     currentDemand = 0;
    // }
    
    // return fitness value
    // return 1 / (time + penalty); 
    // std::cout<<"Time: "<<time<<" Penalty: "<<penalty<<std::endl;
    // return time + penalty;
}