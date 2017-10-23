#ifndef RANDDRAW_COLOREXTRACTORKMEANS_H
#define RANDDRAW_COLOREXTRACTORKMEANS_H

#include "ColorExtractor.h"
#include "KMeans.h"

class ColorExtractorKmeans :public ColorExtractor{
public:
    KMeans kmeans;

    ColorExtractorKmeans();

    void extract(const Picture &pic, int count);
};


#endif //RANDDRAW_COLOREXTRACTORKMEANS_H
