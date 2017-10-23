#ifndef RANDDRAW_COLOREXTRACTORRANDOM_H
#define RANDDRAW_COLOREXTRACTORRANDOM_H

#include "ColorExtractor.h"

class ColorExtractorRandom :public ColorExtractor{
public:
    ColorExtractorRandom();

    void extract(const Picture &pic, int count);


};

#endif //RANDDRAW_COLOREXTRACTORRANDOM_H
