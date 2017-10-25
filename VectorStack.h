#ifndef RANDDRAW_COLORSTACK_H
#define RANDDRAW_COLORSTACK_H

#include "Picture.h"

#include <vector>
#include <cassert>
#include <string>

class VectorStack {
public:
    enum ColorDistanceFunction{
        Manhattan,
        Euclidean,
        CIE2000,
    };

protected:
    ColorDistanceFunction colorDistanceFunction;
    int dimensions;
    int valueCount;

    double distanceManhattan(const std::vector<double> & a, const std::vector<double> & b) const;
    double distanceEuclidean(const std::vector<double> & a, const std::vector<double> & b) const;
    double distanceCIE2000(const std::vector<double> & a, const std::vector<double> & b) const;

public:
    VectorStack(int dimensions);

    struct Entry{
        std::vector<double> values;
        double weight;
    };

    void setColorDistanceFunction(ColorDistanceFunction v);
    void setColorDistanceFunction(std::string name);

    std::vector<Entry> entries;

    std::vector<double> &value(int index) {
        assert(index >= 0 && index < entries.size());

        return entries[index].values;
    }

    double &weight(int index){
        assert(index >= 0 && index<entries.size());

        return entries[index].weight;
    }

    int size() const{
        return valueCount;
    }

    int add(const std::vector<double> & values, double weight);

    double distance(const std::vector<double> & a, const std::vector<double> & b) const;

    int findFurthest(const VectorStack &other) const;

    int findClosest(const std::vector<double> &vector, double & distance) const;
    int findClosest(const std::vector<double> &vector) const;
};

#endif //RANDDRAW_COLORSTACK_H
