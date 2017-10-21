#ifndef RANDDRAW_KMEANS_H
#define RANDDRAW_KMEANS_H

#include <vector>

class KMeans {
    std::vector<std::vector<double>> values;
    std::vector<int> clusterNum;
    std::vector<int> weights;
    std::vector<double> zero;
    int dimensions;
    int valueCount;

    double distance(const std::vector<double> & a, const std::vector<double> & b);

    int findWorstCenter(double & distance);
    int findFurthestValueFromCenter(int center);

public:
    explicit KMeans(int dimensions);

    struct Center{
        std::vector<double> value;
        double averageDistance;
        int entries;
    };

    std::vector<Center> centers;

    void add(std::vector<double> value, int weight = 1);

    void process(int k);

};


#endif //RANDDRAW_KMEANS_H
