#ifndef RANDDRAW_PROCESSOR_H
#define RANDDRAW_PROCESSOR_H

#include "Picture.h"
#include "Shapes.h"

class Processor
{
     Picture &target;
     const ShapeInfo &shapeInfo;
     std::vector<Pixel> colors;
     int picw;
     int pich;

public:
     Processor(Picture &target, const ShapeInfo &shapeInfo, Pixel initialColor);

     Picture pic;

     void iterate(int iterations);
};

#endif //RANDDRAW_PROCESSOR_H
