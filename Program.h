#ifndef RANDDRAW_PROGRAM_H
#define RANDDRAW_PROGRAM_H

#include "Shapes.h"

#include <string>

class Program {
public:
    struct Instruction {
        const ShapeInfo *shape;
        int iterations;
        double scale;
    };

protected:
    std::vector<Instruction> instructions;
    int currentInstruction;
    int currentIteration;
    int totalIterationsCount;

public:
    Program(std::string line);

    void get(const Instruction *&instruction, int &iterations);

    void advance(int iterations);

    int totalIterations();
};


#endif //RANDDRAW_PROGRAM_H
