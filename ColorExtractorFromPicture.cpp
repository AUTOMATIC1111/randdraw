#include "ColorExtractorFromPicture.h"


static double distanceEuclidean(const std::vector<double> & a, const std::vector<double> & b){
    double dist = 0;
    for (int dim = 0; dim < 3; dim++) {
        double v = a[dim] - b[dim];
        dist += v * v;
    }
    return sqrt(dist);
}

ColorExtractorFromPicture::ColorExtractorFromPicture(const Picture &pic) {
    int w = pic.w();
    int h = pic.h();

    std::map<Pixel, int> colorMap;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Pixel pixel = *pic.constPixel(x, y);

            colorMap[pixel]++;
        }
    }

    for (auto iter: colorMap) {
        const Pixel &pixel = iter.first;

        colors.push_back(ColorInfo{ .pixel=pixel, .usage=0, .quality=0 });
    }
}

void ColorExtractorFromPicture::extract(const Picture &pic, int count) {
    int w = pic.w();
    int h = pic.h();
    count = colors.size();

    std::map<Pixel, std::vector<double>> colorMap;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Pixel pixel = *pic.constPixel(x, y);

            auto iter = colorMap.find(pixel);
            if(iter==colorMap.end()){
                double l, a, b;
                pixel.toLab(l, a, b);

                colorMap[pixel]=std::vector<double>{ l, a, b };
            }
        }
    }

    std::vector<std::vector<double>> palette;
    for(int i=0;i<count;i++){
        double l, a, b;
        colors[i].pixel.toLab(l, a, b);

        palette.emplace_back(std::vector<double>{ l, a, b } );
    }

    for (auto iter: colorMap) {
        const Pixel &pixel = iter.first;
        const std::vector<double> & lab = iter.second;

        int chosen=0;
        double chosenDistance=99999;
        for(int i=0;i<count;i++) {
            double dist = distanceEuclidean(lab, palette[i]);
            if (dist < chosenDistance) {
                chosen=i;
                chosenDistance = dist;
            }
        }

        mapping[pixel]=colors[chosen].pixel;
    }
}
