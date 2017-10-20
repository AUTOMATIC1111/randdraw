#ifndef RANDDRAW_COLOREXTRACTOR_H
#define RANDDRAW_COLOREXTRACTOR_H

#include "Picture.h"

#include <vector>
#include <map>

class ColorExtractor
{
public:
     ColorExtractor(const Picture &pic, int count);

     std::vector<Pixel> colors;
     std::map<Pixel, int> colorUsage;
};


#endif //RANDDRAW_COLOREXTRACTOR_H
