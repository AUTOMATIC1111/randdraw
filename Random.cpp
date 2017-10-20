#include "Random.h"

#include <ctime>

#define MY_RAND_MAX 0x0fffffffU

Random::Random(int val)
{
     seed(val);
}
Random::Random()
{
     seed(time(NULL));
}

unsigned int Random::next()
{
     seedValue = (seedValue * 1103515245U + 12345U) & MY_RAND_MAX;
     return (int) seedValue;
}


unsigned int Random::nextInt(int max)
{
     return (unsigned int) ((long long) next() * (max + 1) / (MY_RAND_MAX + 1));
}

unsigned int Random::nextInt(int min, int max)
{
     return min + nextInt(max - min);
}

void Random::seed(unsigned int val)
{
     seedValue = val & MY_RAND_MAX;
}

double Random::nextDouble()
{
     return 1.0 * next() / MY_RAND_MAX;
}
