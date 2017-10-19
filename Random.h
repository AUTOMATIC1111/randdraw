#ifndef RANDDRAW_RANDOM_H
#define RANDDRAW_RANDOM_H

class Random
{
     unsigned int seedValue = 1;

public:
     Random(int seed);

     unsigned int seed() { return seedValue; }
     void seed(unsigned int val);

     unsigned int next();
     unsigned int nextInt(int max);
     unsigned int nextInt(int min, int max);
     double nextDouble();
};


#endif //RANDDRAW_RANDOM_H
