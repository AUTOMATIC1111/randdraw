#include <algorithm>
#include "ColorExtractor.h"
#include "Random.h"
#include "KMeans.h"

static Random random;

ColorExtractor::ColorExtractor(const Picture &pic, int count) {
    int w = pic.w();
    int h = pic.h();

    std::map<Pixel, int> colorMap;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Pixel pixel = *pic.constPixel(x, y);

            colorMap[pixel]++;
        }
    }

    KMeans kmeans(3);
    for (auto iter = colorMap.begin(); iter != colorMap.end(); ++iter) {
        const Pixel &pixel = iter->first;
//        int weight = sqrt(iter->second);
        int weight = iter->second;

        double l, a, b;
        pixel.toLab(l, a, b);

        kmeans.add(std::vector<double>{l, a, b}, weight);
    }

    kmeans.process(count);

    for (int i = 0; i < kmeans.centers.size(); i++) {
        KMeans::Center &center = kmeans.centers[i];

        Pixel pixel = Pixel::fromLab(center.value[0], center.value[1], center.value[2]);

        if (center.entries == 0) continue;

        colors.push_back(ColorInfo{
                .pixel=pixel,
                .usage=center.entries,
                .quality=center.averageDistance,
                .l=center.value[0],
                .a=center.value[1],
                .b=center.value[2],
        });
    }

    std::sort(colors.begin(), colors.end(), [](ColorInfo &a, ColorInfo &b) { return a.pixel < b.pixel; });
}


void ColorExtractor::fillColormap(Picture &colormap, const Picture &pic) {
    int w = pic.w();
    int h = pic.h();
    int colorCount = colors.size();

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Pixel pixel = *pic.constPixel(x, y);

            double l, a, b;
            pixel.toLab(l, a, b);

            int choice = 0;
            double choiceDistance = 99999;
            for (int j = 0; j < colorCount; j++) {
                ColorInfo &info = colors[j];

                double distance = sqrt(
                        (l - info.l) * (l - info.l) +
                        (a - info.a) * (a - info.a) +
                        (b - info.b) * (b - info.b)
                );

                if (distance < choiceDistance) {
                    choice = j;
                    choiceDistance = distance;
                }
            }

            colormap.paint(x, y, colors[choice].pixel);
        }
    }
}