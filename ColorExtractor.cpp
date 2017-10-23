#include "ColorExtractor.h"
#include "Random.h"

static Random random;

ColorExtractor::~ColorExtractor(){

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
