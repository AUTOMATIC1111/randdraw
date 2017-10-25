#include "Processor.h"

#include <map>

double Processor::allowWorseMultiplier = 0;

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
    long long count = 0;
    double totalImprovement = 0;

    while (iterations > 0) {
        const ShapeInfo *shapeInfo;
        const Program::Instruction *instruction;
        int iterCount;

        program.get(instruction, iterCount);
        shapeInfo = instruction->shape;

        if (iterCount == 0) break;
        if (iterCount > iterations) iterCount = iterations;
        iterations -= iterCount;
        program.advance(iterCount);

        for (int i = 0; i < iterCount; i++) {
            Pixel color = colors[random.nextInt((int) colors.size() - 1)];

            int w, h;
            shapeInfo->selectSize(w, h);
            w *= instruction->scale;
            h *= instruction->scale;

            if (w > picw) w = picw;
            if (h > pich) h = pich;

            int px = random.nextInt(picw - w);
            int py = random.nextInt(pich - h);

            unsigned int seed = random.seed();
            PictureEditor edited(pic, target, px, py, w, h);

            double averageImprovement = count == 0 ? 0 : totalImprovement / count;

            shapeInfo->draw(edited, color, w, h);
            if (edited.improvement > 0 || edited.improvement > -averageImprovement * allowWorseMultiplier) {
                PictureReference ref(pic, px, py, w, h);

                random.seed(seed);
                shapeInfo->draw(ref, color, w, h);

                if (edited.improvement > 0) {
                    count++;
                    totalImprovement += edited.improvement;
                }
            }
        }
    }


}
