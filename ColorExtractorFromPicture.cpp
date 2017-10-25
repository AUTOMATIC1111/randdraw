#include "ColorExtractorFromPicture.h"

ColorExtractorFromPicture::ColorExtractorFromPicture(const Picture &pic) :originalColors(3) {
    int w = pic.w();
    int h = pic.h();

    std::map<Pixel, int> colorMap;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Pixel pixel = *pic.constPixel(x, y);

            auto iter = colorMap.find(pixel);
            if(iter==colorMap.end()) {

                originalColors.add(pixel.toLab(), 1.0);
                colors.push_back(ColorInfo{ .pixel=pixel, .usage=0, .quality=0 });
                colorMap[pixel]=1;
            }
        }
    }
}

void ColorExtractorFromPicture::extract(const Picture &pic, int ) {
    int w = pic.w();
    int h = pic.h();

    std::map<Pixel, int> colorCounts;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Pixel pixel = *pic.constPixel(x, y);

            auto iter = mapping.find(pixel);
            if(iter==mapping.end()){
                std::vector<double> lab = pixel.toLab();
                colorCounts[pixel]=1;

                int index = originalColors.findClosest(lab);
                ColorInfo & info = colors[index];
                iter->second = info.pixel;
                info.usage++;
            }

            colorCounts[pixel]++;
        }
    }

    for(auto iter: colors){
        iter.usage = colorCounts[iter.pixel];
    }
}
