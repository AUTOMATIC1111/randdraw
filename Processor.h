#ifndef RANDDRAW_PROCESSOR_H
#define RANDDRAW_PROCESSOR_H

#include "Picture.h"
#include "Shapes.h"
#include "Program.h"

class Processor {
    Picture &target;
    std::vector<Pixel> colors;
    int picw;
    int pich;
    Program &program;

public:
    static double allowWorseMultiplier;

    Processor(Picture &target, Program &program, Pixel initialColor, std::vector<Pixel> colors);

    Picture pic;

    void iterate(int iterations);
};

#endif //RANDDRAW_PROCESSOR_H
