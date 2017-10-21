#include "Processor.h"

#include <map>

Processor::Processor(Picture &targetPic, Program &programVar, Pixel initialColor, std::vector<Pixel> setColors)
        : program(programVar),
          target(targetPic),
          pic(targetPic.w(), targetPic.h(), initialColor) {
    picw = pic.w();
    pich = pic.h();

    if (setColors.empty()) {
        std::map<Pixel, int> colorsMap;
        for (int y = 0; y < pich; y++) {
            for (int x = 0; x < picw; x++) {
                Pixel *pixel = target.pixel(x, y);
                colorsMap[*pixel]++;
            }
        }
        for (auto const &iter: colorsMap) {
            colors.push_back(iter.first);
        }
    } else {
        colors = setColors;
    }
}

void Processor::iterate(int iterations) {
    while (iterations > 0) {
        const ShapeInfo *shapeInfo;
        int iterCount;
        program.get(shapeInfo, iterCount);

        if (iterCount == 0) break;
        if (iterCount > iterations) iterCount = iterations;
        iterations -= iterCount;
        program.advance(iterCount);

        for (int i = 0; i < iterCount; i++) {
            Pixel color = colors[random.nextInt((int) colors.size())];

            int w, h;
            shapeInfo->selectSize(w, h);

            if (w > picw) w = picw;
            if (h > pich) h = pich;

            int px = random.nextInt(picw - w);
            int py = random.nextInt(pich - h);

            unsigned int seed = random.seed();
            PictureEditor edited(pic, target, px, py, w, h);

            shapeInfo->draw(edited, color, w, h);
            if (edited.improvement > 0) {
                PictureReference ref(pic, px, py, w, h);

                random.seed(seed);
                shapeInfo->draw(ref, color, w, h);
            }
        }
    }


}
