#include "KMeans.h"
#include "Random.h"
#include "CIEDE2000.h"

#include <cassert>
#include <math.h>
#include <algorithm>

KMeans::KMeans(int dims) : zero(dims, 0.0) {
    dimensions = dims;
    valueCount = 0;
    colorDistanceFunction = Euclidean;
}

int KMeans::add(std::vector<double> value, int weight) {
    assert(value.size() == dimensions);

    int position = values.size();

    values.push_back(value);
    weights.push_back(weight);
    mapping.push_back(0);
    valueCount++;

    return position;
}

double KMeans::distance(const std::vector<double> &a, const std::vector<double> &b) {
    assert(a.size() == dimensions);
    assert(b.size() == dimensions);

    switch(colorDistanceFunction){
        case Euclidean: return distanceEuclidean(a, b);
        case Manhattan: return distanceManhattan(a, b);
        case CIE2000: return distanceCIE2000(a, b);
    }

    return 0;
}

double KMeans::distanceManhattan(const std::vector<double> & a, const std::vector<double> & b){
    double dist = 0;
    for (int dim = 0; dim < dimensions; dim++) {
        dist += abs(a[dim] - b[dim]);
    }
    return dist;
}
double KMeans::distanceEuclidean(const std::vector<double> & a, const std::vector<double> & b){
    double dist = 0;
    for (int dim = 0; dim < dimensions; dim++) {
        double v = a[dim] - b[dim];
        dist += v * v;
    }
    return sqrt(dist);
}
double KMeans::distanceCIE2000(const std::vector<double> & a, const std::vector<double> & b){
    assert(dimensions == 3);

    CIEDE2000::LAB labA{ .l=a[0], .a=a[1], .b=a[2]};
    CIEDE2000::LAB labB{ .l=b[0], .a=b[1], .b=b[2]};

    return CIEDE2000::CIEDE2000(labA, labB);
}


static double acceptableLabDistance[] = {
        1,
        3,
        2,
        4,
        6,
        10,
        12,
        15,
        25,
        99999,
};

static double getAcceptableLabDistance(int colorCount) {
    if (colorCount < 0) return acceptableLabDistance[0];

    if (colorCount >= sizeof(acceptableLabDistance) / sizeof(acceptableLabDistance[0]))
        colorCount = sizeof(acceptableLabDistance) / sizeof(acceptableLabDistance[0]) - 1;

    return acceptableLabDistance[colorCount];
}

#include "Picture.h"

void KMeans::process(int k) {
    bool adaptive = false;
    if (k == 0) {
        adaptive = true;
        k = 2;
    }

    while (centers.size() < k) {
        int choice = 0;
        double choiceDistance = 0;

        for (int i = 0; i < valueCount; i++) {
            double dist = 9999;
            for (int j = 0; j < centers.size(); j++) {
                double distanceToOneCenter = distance(values[i], centers[j].value);
                if (distanceToOneCenter < dist) dist = distanceToOneCenter;
            }

            if (dist > choiceDistance) {
                choice = i;
                choiceDistance = dist;
            }
        }

        centers.push_back(Center{.value=values[choice], .averageDistance=0.0, .entries=1});
    }

    int iterations = 0;
    while (1) {
        std::vector<Center> newCenters;
        for (int i = 0; i < k; i++) {
            newCenters.push_back(Center{.value=zero, .averageDistance=0.0, .entries=0});
        }

        /*
        std::vector<Pixel> palette;
        for (int i = 0; i < k; i++) {
            Pixel p=Pixel::fromLab(centers[i].value[0], centers[i].value[1], centers[i].value[2]);
            palette.push_back(p);
        }
        Picture::fromPalette(palette).save(("../"+std::to_string(100000+iterations)+".png").c_str());
        */

        for (int i = 0; i < valueCount; i++) {
            std::vector<double> &value = values[i];
            int weight = weights[i];

            int choice = 0;
            double choiceDistance = 99999;
            for (int j = 0; j < k; j++) {
                Center &center = centers[j];
                double dist = distance(value, center.value);

                if (dist < choiceDistance) {
                    choice = j;
                    choiceDistance = dist;
                }
            }

            Center &center = newCenters[choice];
            for (int dim = 0; dim < dimensions; dim++) {
                center.value[dim] += value[dim] * weight;
            }
            center.entries += weight;
            center.averageDistance += choiceDistance * weight;
            mapping[i] = choice;
        }

        double totalMovement = 0;
        for (int i = 0; i < k; i++) {
            Center &newCenter = newCenters[i];
            Center &center = centers[i];

            if (newCenter.entries) {
                for (int dim = 0; dim < dimensions; dim++) {
                    newCenter.value[dim] /= newCenter.entries;
                }
                newCenter.averageDistance /= newCenter.entries;
            }

            for (int dim = 0; dim < dimensions; dim++) {
                totalMovement += abs(center.value[dim] - newCenter.value[dim]);
            }
        }

        centers = newCenters;
        iterations++;

        if (totalMovement < 0.000001) {
            if (adaptive) {
                double worstDistance;
                int worstCenter=findWorstCenter(worstDistance);

                if (worstDistance > getAcceptableLabDistance(k) && worstCenter != -1) {
                    int choice = findFurthestValueFromCenter(worstCenter);

                    centers.push_back(Center{.value=values[choice], .averageDistance=0.0, .entries=1});
                    k++;

                    continue;
                }
            }

            break;
        }
    }
}

int KMeans::findWorstCenter(double & distance) {
    int k = centers.size();
    double worstDistance = 0;
    int worstCluster = -1;

    int largestClusterSize = 0;
    for (int i = 0; i < k; i++) {
        Center &center = centers[i];
        if (center.entries > largestClusterSize) {
            largestClusterSize = center.entries;
        }
    }

    for (int i = 0; i < k; i++) {
        Center &center = centers[i];
        double dist = sqrt(1.0 * center.entries / largestClusterSize) * center.averageDistance;

        if (dist > worstDistance) {
            worstDistance = dist;
            worstCluster = i;
        }
    }

    distance = worstDistance;
    return worstCluster;
}

int KMeans::findFurthestValueFromCenter(int center) {
    int choice = 0;
    double dist = 0;
    for (int i = 0; i < valueCount; i++) {
        if (mapping[i] != center)
            continue;

        double distanceToWorstCenter = distance(values[i], centers[center].value) * weights[i];
        if (distanceToWorstCenter > dist) {
            dist = distanceToWorstCenter;
            choice = i;
        }
    }

    return choice;
}

void KMeans::setColorDistanceFunction(KMeans::ColorDistanceFunction v) {
    colorDistanceFunction=v;
}

void KMeans::setColorDistanceFunction(std::string name) {
    std::string n = name;
    std::transform(n.begin(), n.end(), n.begin(), ::tolower);

    if(n == "e" || n == "euc" || n == "euclidean")
        colorDistanceFunction=Euclidean;
    else if(n == "m" || n == "man" || n == "manhattan")
        colorDistanceFunction=Manhattan;
    else if(n == "cie2000")
        colorDistanceFunction=CIE2000;
}
