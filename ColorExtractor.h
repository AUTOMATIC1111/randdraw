#ifndef RANDDRAW_COLOREXTRACTOR_H
#define RANDDRAW_COLOREXTRACTOR_H

#include "Picture.h"
#include "KMeans.h"

#include <vector>
#include <map>

class ColorExtractor {
public:
    ColorExtractor();

    KMeans kmeans;
    std::string comparisonFunction;
    void extract(const Picture &pic, int count);

    struct ColorInfo{
        Pixel pixel;
        int usage;
        double quality;

        double l, a, b;
    };
    std::vector<ColorInfo> colors;
    std::map<Pixel, Pixel> mapping;

    void fillColormap(Picture &colormap, const Picture &pic);
};


#endif //RANDDRAW_COLOREXTRACTOR_H
