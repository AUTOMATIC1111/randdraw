#include "KMeans.h"
#include "Random.h"

#include <cassert>
#include <math.h>
#include <algorithm>

KMeans::KMeans(int dims) : zero(dims, 0.0), values(dims), centersStack(dims) {
    dimensions = dims;
}

int KMeans::add(std::vector<double> value, double weight) {
    assert(value.size() == dimensions);

    mapping.push_back(0);
    return values.add(value, weight);
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

void KMeans::process(int k) {
    bool adaptive = false;
    if (k == 0) {
        adaptive = true;
        k = 2;
    }

    while (centers.size() < k) {
        int choice = values.findFurthest(centersStack);

        centers.push_back(Center{.value=values.value(choice), .averageDistance=0.0, .entries=1});
        centersStack.add(values.value(choice), 1);
    }

    int iterations = 0;
    while (1) {
        std::vector<Center> newCenters;
        for (int i = 0; i < k; i++) {
            newCenters.push_back(Center{.value=zero, .averageDistance=0.0, .entries=0});
        }

        for (int i = 0; i < values.size(); i++) {
            std::vector<double> &value = values.value(i);
            int weight = values.weight(i);

            double choiceDistance;
            int choice = centersStack.findClosest(value, choiceDistance);

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
        for (int i = 0; i < k; i++) {
            centersStack.value(i) = centers[i].value;
        }
        iterations++;

        if (totalMovement < 0.000001) {
            if (adaptive) {
                double worstDistance;
                int worstCenter=findWorstCenter(worstDistance);

                if (worstDistance > getAcceptableLabDistance(k) && worstCenter != -1) {
                    int choice = findFurthestValueFromCenter(worstCenter);

                    centers.push_back(Center{.value=values.value(choice), .averageDistance=0.0, .entries=1});
                    centersStack.add(values.value(choice), 1);
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
    for (int i = 0; i < values.size(); i++) {
        if (mapping[i] != center)
            continue;

        double distanceToWorstCenter = centersStack.distance(values.value(i), centers[center].value);
        if (distanceToWorstCenter > dist) {
            dist = distanceToWorstCenter;
            choice = i;
        }
    }

    return choice;
}

void KMeans::setColorDistanceFunction(std::string name) {
    values.setColorDistanceFunction(name);
    centersStack.setColorDistanceFunction(name);
}
