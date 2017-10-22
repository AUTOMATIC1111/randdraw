#ifndef RANDDRAW_KMEANS_H
#define RANDDRAW_KMEANS_H

#include <vector>
#include <string>

class KMeans {
public:
    enum ColorDistanceFunction{
        Manhattan,
        Euclidean,
        CIE2000,
    };

    struct Center{
        std::vector<double> value;
        double averageDistance;
        int entries;
    };

protected:
    std::vector<std::vector<double>> values;
    std::vector<int> weights;
    std::vector<double> zero;
    int dimensions;
    int valueCount;
    ColorDistanceFunction colorDistanceFunction;

    double distance(const std::vector<double> & a, const std::vector<double> & b);
    double distanceManhattan(const std::vector<double> & a, const std::vector<double> & b);
    double distanceEuclidean(const std::vector<double> & a, const std::vector<double> & b);
    double distanceCIE2000(const std::vector<double> & a, const std::vector<double> & b);

    int findWorstCenter(double & distance);
    int findFurthestValueFromCenter(int center);

public:
    explicit KMeans(int dimensions);

    void setColorDistanceFunction(ColorDistanceFunction v);
    void setColorDistanceFunction(std::string name);

    std::vector<Center> centers;
    std::vector<int> mapping;

    int add(std::vector<double> value, int weight = 1);

    void process(int k);
};


#endif //RANDDRAW_KMEANS_H
