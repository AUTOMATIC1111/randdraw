#include <algorithm>
#include "ColorExtractor.h"
#include "Random.h"
#include "KMeans.h"

static Random random;

ColorExtractor::ColorExtractor() :kmeans(3){
}

void ColorExtractor::fillColormap(Picture &colormap, const Picture &pic) {
    int w = pic.w();
    int h = pic.h();

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            const Pixel& pixel = *pic.constPixel(x, y);

            auto iter=mapping.find(pixel);
            if(iter == mapping.end()){
                colormap.paint(x, y, Pixel(0,0,0));
            } else{
                colormap.paint(x, y, iter->second);
            };
        }
    }
}

void ColorExtractor::extract(const Picture &pic, int count) {
    int w = pic.w();
    int h = pic.h();

    std::map<Pixel, int> colorMap;
    std::map<Pixel, int> colorIndexes;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Pixel pixel = *pic.constPixel(x, y);

            colorMap[pixel]++;
        }
    }

    for (auto iter = colorMap.begin(); iter != colorMap.end(); ++iter) {
        const Pixel &pixel = iter->first;
//        int weight = sqrt(iter->second);
        int weight = iter->second;

        double l, a, b;
        pixel.toLab(l, a, b);

        colorIndexes[pixel] = kmeans.add(std::vector<double>{l, a, b}, weight);
    }

    kmeans.process(count);

    std::vector<Pixel> centerPixel;
    for (int i = 0; i < kmeans.centers.size(); i++) {
        KMeans::Center &center = kmeans.centers[i];

        centerPixel.push_back(Pixel::fromLab(center.value[0], center.value[1], center.value[2]));
    }

    for (auto iter = colorIndexes.begin(); iter != colorIndexes.end(); ++iter) {
        const Pixel &pixel = iter->first;
        int index = iter->second;

        mapping[pixel] = centerPixel[kmeans.mapping[index]];
    }

    for (int i = 0; i < kmeans.centers.size(); i++) {
        KMeans::Center &center = kmeans.centers[i];

        if (center.entries == 0) continue;

        colors.push_back(ColorInfo{
                .pixel=centerPixel[i],
                .usage=center.entries,
                .quality=center.averageDistance,
                .l=center.value[0],
                .a=center.value[1],
                .b=center.value[2],
        });
    }

    std::sort(colors.begin(), colors.end(), [](ColorInfo &a, ColorInfo &b) { return a.pixel < b.pixel; });
}
