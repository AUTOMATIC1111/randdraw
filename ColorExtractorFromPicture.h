#ifndef RANDDRAW_COLOREXTRACTORFROMPICTURE_H
#define RANDDRAW_COLOREXTRACTORFROMPICTURE_H

#include "ColorExtractor.h"
#include "VectorStack.h"

class ColorExtractorFromPicture : public ColorExtractor {
    VectorStack originalColors;

public:
    ColorExtractorFromPicture(const Picture &pic);

    void extract(const Picture &pic, int count);
};


#endif //RANDDRAW_COLOREXTRACTORFROMPICTURE_H
