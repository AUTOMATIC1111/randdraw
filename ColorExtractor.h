#ifndef RANDDRAW_COLOREXTRACTOR_H
#define RANDDRAW_COLOREXTRACTOR_H

#include "Picture.h"

#include <vector>
#include <map>

class ColorExtractor {
public:
    virtual ~ColorExtractor();

    struct ColorInfo{
        Pixel pixel;
        int usage;
        double quality;
    };

    virtual void extract(const Picture &pic, int count)=0;

    std::vector<ColorInfo> colors;
    std::map<Pixel, Pixel> mapping;

    void fillColormap(Picture &colormap, const Picture &pic);
};


#endif //RANDDRAW_COLOREXTRACTOR_H
