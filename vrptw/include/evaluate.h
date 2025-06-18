#pragma once

#include "genetic_algorithm.h"
#include "instance.h"
#include <vector>

float evaluateSolution(individual_t const& solution, std::vector<std::vector<float>> const& distanceMatrix, const int& truckCapacity, int& counter);
