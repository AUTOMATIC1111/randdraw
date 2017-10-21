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
void KMeans::process(int k) {
    bool adaptive = false;
    if (k == 0) {
        adaptive = true;
        k = 2;
    }

    int startingSize = centers.size();
    int iterations = 0;
    while (1) {
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
            center.averageDistance += choiceDistance;
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
            if(adaptive){
                double worstDistance = 0;

                for (int i = 0; i < k; i++) {
                    Center &center = centers[i];
                    if(center.averageDistance > worstDistance)
                        worstDistance = center.averageDistance;
                }

                if(worstDistance>15){
                    k++;
                    continue;
                }
            }

            break;
        }
    }

}



