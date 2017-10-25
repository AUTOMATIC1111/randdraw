#include "ColorExtractorRandom.h"
#include "Random.h"
#include "VectorStack.h"

ColorExtractorRandom::ColorExtractorRandom() {

}

static Random random;

void ColorExtractorRandom::extract(const Picture &pic, int count) {
    int w = pic.w();
    int h = pic.h();

    struct Info {
        int usage;
        std::vector<double> lab;
    };

    std::map<Pixel, Info> colorMap;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Pixel pixel = *pic.constPixel(x, y);

            auto iter = colorMap.find(pixel);
            if (iter == colorMap.end()) {
                double l, a, b;
                pixel.toLab(l, a, b);

                colorMap[pixel] = Info{.usage=1, .lab=std::vector<double>{l, a, b}};
            } else {
                colorMap[pixel].usage++;
            }
        }
    }

    VectorStack palette(3);

    for (int i = 0; i < count; i++)
        palette.add(random.nextLabColor(), 1.0);

    double bestDistance;

    int iterationsWithoutImprovement = 0;
    for (int iteration = 0; iteration < 10000; iteration++) {
        if (iterationsWithoutImprovement > 2000)
            break;

        int newIndex = random.nextInt(count - 1);
        std::vector<double> oldColor = palette.value(newIndex);
        palette.value(newIndex) = random.nextLabColor();

        double distance = 0;
        for (auto iter = colorMap.begin(); iter != colorMap.end(); ++iter) {
            const Pixel &pixel = iter->first;

            double chosenDistance;
            palette.findClosest(iter->second.lab, chosenDistance);
            distance += chosenDistance * iter->second.usage;

            if (distance >= bestDistance && iteration>0)
                break;
        }

        if (iteration==0 || distance < bestDistance) {
            bestDistance = distance;
            iterationsWithoutImprovement = 0;
            printf("% -12d %f\n", iteration, distance / w / h);
        } else {
            palette.value(newIndex) = oldColor;
            iterationsWithoutImprovement++;
        }
    }

    std::vector<int> paletteUsage(count, 0);
    for (auto iter = colorMap.begin(); iter != colorMap.end(); ++iter) {
        const Pixel &pixel = iter->first;
        std::vector<double> &lab = iter->second.lab;

        int index = palette.findClosest(lab);
        mapping[pixel] = Pixel::fromLab(palette.value(index));
        paletteUsage[index] += iter->second.usage;
    }

    for (int i = 0; i < count; i++) {
        std::vector<double> &labChoice = palette.value(i);

        colors.push_back(
            ColorInfo{.pixel=Pixel::fromLab(labChoice[0], labChoice[1], labChoice[2]), .usage=paletteUsage[i], .quality=0});
    }
}
