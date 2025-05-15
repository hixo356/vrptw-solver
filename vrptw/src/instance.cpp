#include "../include/instance.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <cmath>

float distanceBetweenTwoNodes(const Node& node1, const Node& node2){
    return std::sqrt(std::pow(node2.x - node1.x, 2) + std::pow(node2.y - node1.y, 2));
}

void ProblemInstance::readInstanceFromFile(const std::string& filePath){
    std::ifstream inputFile(filePath);
    std::string line;

    if (!inputFile.is_open()) {
        std::cerr << "Failed to open \"" << filePath << "\" file!" << std::endl;
        return;
    }

    // NAME
    std::getline(inputFile, line);
    std::istringstream ss(line);
    ss.ignore(6);
    ss >> this->name;
    
    // OPTIMAL VALUE
    std::getline(inputFile, line);
    std::regex pattern("Optimal value: (\\d+)");
    std::smatch match;
    std::regex_search(line, match, pattern);
    this->optimalResult = std::stoi(match[1].str());

    // SKIP LINE
    std::getline(inputFile, line);

    // DIMENSION
    std::getline(inputFile, line);
    ss.str(line);
    ss.clear();
    ss.ignore(11);
    ss >> this->dimension;

    // SKIP LINE
    std::getline(inputFile, line);
    
    // CAPACITY
    std::getline(inputFile, line);
    ss.str(line);
    ss.clear();
    ss.ignore(10);
    ss >> this->capacity;

    // SKIP LINE
    std::getline(inputFile, line);

    // NODES
    this->nodes.reserve(this->dimension);

    for (int i=0; i<this->dimension; i++) {
        struct Node node;

        inputFile >> node.id >> node.x >> node.y;
        this->nodes.push_back(node);
    }

    // SKIP LINE
    std::getline(inputFile, line);
    std::getline(inputFile, line);

    // DEMANDS
    for (int i=0; i<this->dimension; i++) {
        int tmp, tmp2;

        inputFile >> tmp >> tmp2;

        this->nodes[i].demand = tmp2;
    }

    // CALCULATE DISTANCES FOR LATER
    this->distanceMatrix.resize(this->dimension+1);
    for (auto& vec : this->distanceMatrix) {
        vec.resize(this->dimension+1);
    }

    for (int i=1; i<this->dimension+1; i++) {
        for (int j=1; j<this->dimension+1; j++) {
            distanceMatrix[i][j] = distanceBetweenTwoNodes(this->nodes[i-1], this->nodes[j-1]);
        }
    }

    std::cout << "File " << filePath << " read successfully!" << std::endl;
    inputFile.close();
}

void ProblemInstance::printInstanceData() const {
    std::cout << "Problem Name: " << name << std::endl;
    std::cout << "Optimal Value: " << optimalResult << std::endl;
    std::cout << "Dimension: " << dimension << std::endl;

    std::cout << "Nodes:" << std::endl;
    for (const auto& node : nodes) {
        std::cout << "Node ID: " << node.id
                  << ", X: " << node.x
                  << ", Y: " << node.y
                  << ", Demand: " << node.demand
                  << std::endl;
    }
}

std::vector<ProblemInstance> readAllProblemInstances(std::string dirPath){
    std::vector<ProblemInstance> instances = {};
    
    // check if directory doesn't exist, return empty vector
    if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath)) {
        std::cerr << "Data directory doesn't exist!" << std::endl;
        return instances;
    }

    for (const auto& file : std::filesystem::directory_iterator(dirPath)) {
        ProblemInstance instance;

        instance.readInstanceFromFile(file.path().string());

        // instance.printInstanceData();

        instances.push_back(instance);
    }
    
    std::cout << "All files read!" << std::endl;
    return instances;
}
