#ifndef SOLUTION_H
#define SOLUTION_H

#include "instance.h"
#include <string>
#include <chrono>

typedef std::chrono::duration<int, std::milli> ms_t;

template<typename T>
class InstanceSolution{
    private:
        std::string name;
        ms_t time;
    public:
        std::vector<Node> const solution;

        InstanceSolution();
};

#endif