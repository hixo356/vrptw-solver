#include "../include/evaluate.h"
#include "../include/instance.h"

#include <iostream>

float evaluateSolution(std::vector<const Node*> const& solution, std::vector<std::vector<float>> const& distanceMatrix, const int& truckCapacity, int& counter){
    // counter++;

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

    int i = 0;
    int currentDemand = 0;
    int waitingTime = 0;
    int lateTime = 0;
    while(i < solution.size()) {
        // start from depot
        time += distanceMatrix[1][solution[i]->id];
        if (time < solution[i]->readyTime) { //przed otwarciem
            waitingTime = solution[i]->readyTime - time;
            penalty += waitingTime/2.0;
            time += waitingTime;
        } else if (time > solution[i]->dueTime) { //po zamknieciu
            lateTime = time - solution[i]->dueTime;
            // penalty += ((lateTime/2.0) * (lateTime/2.0));
            penalty += lateTime/2.0;
        }
        currentDemand += solution[i]->demand;
        time += solution[i]->serviceTime;
        i++;

        while (i < solution.size()) {
            currentDemand += solution[i]->demand;
            if (currentDemand > truckCapacity) {
                // jak za duzy ladunek to koniec trasy
                counter++;
                break;
            }
            time += distanceMatrix[solution[i-1]->id][solution[i]->id];
            if (time < solution[i]->readyTime) { //przed otwarciem
                waitingTime = solution[i]->readyTime - time;
                penalty += waitingTime/2.0;
                time += waitingTime;
            } else if (time > solution[i]->dueTime) { //po zamknieciu
                lateTime = time - solution[i]->dueTime;
                // penalty += ((lateTime/2.0) * (lateTime/2.0));
                penalty += lateTime/2.0;
            }
            time += solution[i]->serviceTime;
            i++;
        }

        // do {
        //     time += distanceMatrix[solution[i-1]->id][solution[i]->id];
        //     currentDemand += solution[i]->demand;
        //     i++;
        // }while (currentDemand <= truckCapacity);
        
        time += distanceMatrix[solution[i-1]->id][1];
        currentDemand = 0;
    }
    
    // return fitness value
    // return 1 / (time + penalty); 
    // std::cout<<"Time: "<<time<<" Penalty: "<<penalty<<std::endl;
    return time + penalty;
}