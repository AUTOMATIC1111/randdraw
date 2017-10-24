#ifndef RANDDRAW_COLOREXTRACTORFROMPICTURE_H
#define RANDDRAW_COLOREXTRACTORFROMPICTURE_H


#include "ColorExtractor.h"

class ColorExtractorFromPicture : public ColorExtractor {
public:
    ColorExtractorFromPicture(const Picture &pic);

    void extract(const Picture &pic, int count);
};


#endif //RANDDRAW_COLOREXTRACTORFROMPICTURE_H
