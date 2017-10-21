#ifndef RANDDRAW_SHAPES_H
#define RANDDRAW_SHAPES_H

#include "Picture.h"
#include "Random.h"

#include <vector>

extern Random random;

struct ShapeInfo {
    const char *name;

    void (*selectSize)(int &w, int &h);

    void (*draw)(Picture &pic, const Pixel &color, int w, int h);
};

extern std::vector<ShapeInfo> shapes;

#endif //RANDDRAW_SHAPES_H
