#ifndef RANDDRAW_KMEANS_H
#define RANDDRAW_KMEANS_H

#include "VectorStack.h"

#include <vector>
#include <string>

class KMeans {
public:
    struct Center{
        std::vector<double> value;
        double averageDistance;
        int entries;
    };

protected:
    VectorStack values;
    VectorStack centersStack;
    std::vector<double> zero;
    int dimensions;

    int findWorstCenter(double & distance);
    int findFurthestValueFromCenter(int center);

public:
    explicit KMeans(int dimensions);

    std::vector<Center> centers;
    std::vector<int> mapping;

    int add(std::vector<double> value, double weight = 1);

    void process(int k);

    void setColorDistanceFunction(std::string name);
};


#endif //RANDDRAW_KMEANS_H
