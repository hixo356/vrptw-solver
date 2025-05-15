#ifndef INSTANCE_H
#define INSTANCE_H

#include <string>
#include <vector>

struct Node{
    int id = 0;
    int x = 0;
    int y = 0;
    int demand = 0;
};

class ProblemInstance{
    private:
        std::string name;
        
        int capacity = 0;
        int optimalResult = 0;
        int dimension = 0;
    public:
        // ProblemInstance(std::string _name, std::vector<Node> _nodes, int _capacity, int _optimalResult);
        ProblemInstance(){};
        std::vector<Node> nodes;
        std::vector<std::vector<float>> distanceMatrix;

        const int& getCapacity() const { return capacity; };
        const int& getOptimalResult() const { return optimalResult; };
        const std::vector<Node>& getNodes() const { return nodes; };
        const std::string& getName() const { return name; };

        void readInstanceFromFile(const std::string& filePath);
        void printInstanceData() const;
};

std::vector<ProblemInstance> readAllProblemInstances(std::string dirPath);

#endif