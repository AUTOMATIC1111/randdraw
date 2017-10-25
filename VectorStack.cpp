#include <algorithm>
#include "VectorStack.h"
#include "CIEDE2000.h"

VectorStack::VectorStack(int dims) :dimensions(dims), valueCount(0), colorDistanceFunction(Euclidean){

}

double VectorStack::distance(const std::vector<double> &a, const std::vector<double> &b)  const{
    assert(a.size() == dimensions);
    assert(b.size() == dimensions);

    switch(colorDistanceFunction){
        case Euclidean: return distanceEuclidean(a, b);
        case Manhattan: return distanceManhattan(a, b);
        case CIE2000: return distanceCIE2000(a, b);
    }

    return 0;
}

double VectorStack::distanceManhattan(const std::vector<double> & a, const std::vector<double> & b) const{
    double dist = 0;
    for (int dim = 0; dim < dimensions; dim++) {
        dist += abs(a[dim] - b[dim]);
    }
    return dist;
}
double VectorStack::distanceEuclidean(const std::vector<double> & a, const std::vector<double> & b) const{
    double dist = 0;
    for (int dim = 0; dim < dimensions; dim++) {
        double v = a[dim] - b[dim];
        dist += v * v;
    }
    return sqrt(dist);
}
double VectorStack::distanceCIE2000(const std::vector<double> & a, const std::vector<double> & b) const{
    assert(dimensions == 3);

    CIEDE2000::LAB labA{ .l=a[0], .a=a[1], .b=a[2]};
    CIEDE2000::LAB labB{ .l=b[0], .a=b[1], .b=b[2]};

    return CIEDE2000::CIEDE2000(labA, labB);
}

int VectorStack::add(const std::vector<double> &values, double weight) {
    entries.push_back(Entry{ .values=values, .weight=weight });
    return  valueCount++;
}

int VectorStack::findFurthest(const VectorStack &other) const{
    int choice = 0;
    double choiceDistance = 0;

    for (int i = 0; i < valueCount; i++) {
        double dist;
        other.findClosest(entries[i].values, dist);

        if (i==0 || dist > choiceDistance) {
            choice = i;
            choiceDistance = dist;
        }
    }

    return choice;
}

int VectorStack::findClosest(const std::vector<double> &vector, double & distanceResult) const{
    int choice = 0;
    double choiceDistance;

    for (int j = 0; j < valueCount; j++) {
        double dist = distance(entries[j].values, vector);

        if (j==0 || dist < choiceDistance) {
            choice = j;
            choiceDistance = dist;
        }
    }

    distanceResult = choiceDistance;
    return choice;
}

int VectorStack::findClosest(const std::vector<double> &vector) const{
    double distanceResult;
    return findClosest(vector, distanceResult);
}

void VectorStack::setColorDistanceFunction(VectorStack::ColorDistanceFunction v) {
    colorDistanceFunction=v;
}

void VectorStack::setColorDistanceFunction(std::string name) {
    std::string n = name;
    std::transform(n.begin(), n.end(), n.begin(), ::tolower);

    if(n == "e" || n == "euc" || n == "euclidean")
        colorDistanceFunction=Euclidean;
    else if(n == "m" || n == "man" || n == "manhattan")
        colorDistanceFunction=Manhattan;
    else if(n == "cie2000")
        colorDistanceFunction=CIE2000;
}
