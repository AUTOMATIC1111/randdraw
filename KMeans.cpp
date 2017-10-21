#include "KMeans.h"
#include "Random.h"

#include <cassert>
#include <cmath>

static Random random;

KMeans::KMeans(int dims) : zero(dims, 0.0) {
    dimensions = dims;
    valueCount = 0;
}

void KMeans::add(std::vector<double> value, int weight) {
    assert(value.size() == dimensions);

    values.push_back(value);
    weights.push_back(weight);
    clusterNum.push_back(0);
    valueCount++;
}

double KMeans::distance(const std::vector<double> &a, const std::vector<double> &b) {
    double dist = 0;
    for (int dim = 0; dim < dimensions; dim++) {
        double v = a[dim] - b[dim];
        dist += v * v;
    }
    return sqrt(dist);
}

/*
double KMeans::distance(const std::vector<double> &a, const std::vector<double> &b) {
    double dist = 0;
    for (int dim = 0; dim < dimensions; dim++) {
        dist += abs(a[dim] - b[dim]);
    }
    return dist;
}
*/

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
            clusterNum[i] = choice;
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
        if (clusterNum[i] != center)
            continue;

        double distanceToWorstCenter = distance(values[i], centers[center].value) * weights[i];
        if (distanceToWorstCenter > dist) {
            dist = distanceToWorstCenter;
            choice = i;
        }
    }

    return choice;
}