#include "ColorExtractorRandom.h"
#include "Random.h"

ColorExtractorRandom::ColorExtractorRandom() {

}

static Random random;


static double distanceEuclidean(const std::vector<double> & a, const std::vector<double> & b){
    double dist = 0;
    for (int dim = 0; dim < 3; dim++) {
        double v = a[dim] - b[dim];
        dist += v * v;
    }
    return sqrt(dist);
}

void ColorExtractorRandom::extract(const Picture &pic, int count) {
    int w = pic.w();
    int h = pic.h();

    struct Info{
        int usage;
        std::vector<double> lab;
    };

    std::map<Pixel, Info> colorMap;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Pixel pixel = *pic.constPixel(x, y);

            auto iter = colorMap.find(pixel);
            if(iter==colorMap.end()){
                double l, a, b;
                pixel.toLab(l, a, b);

                colorMap[pixel]=Info{ .usage=1, .lab=std::vector<double>{ l, a, b } };
            } else{
                colorMap[pixel].usage++;
            }
        }
    }

    std::vector<std::vector<double>> palette;
    for(int i=0;i<count;i++)
        palette.push_back(std::vector<double>{ 0, 0, 0 });
    double bestDistance = 999999999999999;

    int iterationsWithoutImprovement=0;
    for(int iteration=0;iteration<10000;iteration++){
        if(iterationsWithoutImprovement>2000)
            break;

        std::vector<double> newColor = std::vector<double>{ random.nextDouble()*100, random.nextDouble()*256-128, random.nextDouble()*256-128 };
        int newIndex=random.nextInt(count);

        double distance = 0;
        for (auto iter = colorMap.begin(); iter != colorMap.end(); ++iter) {
            const Pixel &pixel = iter->first;
            int weight = iter->second.usage;
            std::vector<double> & lab = iter->second.lab;

            double chosenDistance=99999;
            for(int i=0;i<count;i++){
                double dist = distanceEuclidean(lab, i == newIndex ? newColor : palette[i]);
                if(dist < chosenDistance)
                    chosenDistance = dist;
            }

            distance += chosenDistance * weight;

            if(distance>=bestDistance)
                break;
        }

        if(distance < bestDistance){
            bestDistance = distance;
            palette[newIndex] = newColor;
            iterationsWithoutImprovement=0;
            printf("% 12d %f\n",iteration,distance/w/h);
        } else{
            iterationsWithoutImprovement++;
        }
    }

    for (auto iter = colorMap.begin(); iter != colorMap.end(); ++iter) {
        const Pixel &pixel = iter->first;
        std::vector<double> & lab = iter->second.lab;

        int choice=0;
        int chosenDistance=99999;
        for(int i=0;i<count;i++){
            double dist = distanceEuclidean(lab, palette[i]);
            if(dist < chosenDistance){
                choice=i;
                chosenDistance = dist;
            }
        }

        std::vector<double> & labChoice = palette[choice];
        mapping[pixel]=Pixel::fromLab(labChoice[0],labChoice[1],labChoice[2]);
    }

    for(int i=0;i<count;i++){
        std::vector<double> & labChoice = palette[i];

        colors.push_back(ColorInfo{ .pixel=Pixel::fromLab(labChoice[0],labChoice[1],labChoice[2]), .usage=0, .quality=0 });
    }
}
