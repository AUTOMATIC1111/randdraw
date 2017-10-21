#ifndef RANDDRAW_PROGRAM_H
#define RANDDRAW_PROGRAM_H

#include "Shapes.h"

#include <string>

class Program {
    struct Instruction {
        const ShapeInfo *shape;
        int iterations;
    };
    std::vector<Instruction> instructions;
    int currentInstruction;
    int currentIteration;
    int totalIterationsCount;

public:
    Program(std::string line);

    void get(const ShapeInfo *&shape, int &iterations);

    void advance(int iterations);

    int totalIterations();
};


#endif //RANDDRAW_PROGRAM_H
