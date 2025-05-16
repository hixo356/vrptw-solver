#include <algorithm>
#include <iostream>
#include <ranges>
#include <vector>
#include "evaluate.h"
#include "instance.h"

bool compare(const Node* lhs, const Node* rhs){
    return lhs->demand > rhs->demand;
}

int main(){
    std::vector<ProblemInstance> instances = readAllProblemInstances("data/");

    int counter = 0;
    for (ProblemInstance instance : instances) {
        counter = 0;

        std::vector<const Node*> testSolution(instance.nodes.size()-1);
        int i=0;
        for (const Node& node : instance.nodes | std::views::drop(1)) {
            testSolution[i] = &node;
            i++;
        }

        std::sort(testSolution.begin(), testSolution.end(), compare);

        // for (auto node : testSolution) {
        //     std::cout << node->id << std::endl;
        // }

        std::cout << instance.getName() << " | Fitness: " << evaluateSolution(testSolution, instance.distanceMatrix, instance.getCapacity(), counter) << " Route count: " << counter << std::endl;
    }

    
}