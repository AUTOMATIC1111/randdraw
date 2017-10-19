#include "Picture.h"

#include <cstdio>
#include <string>
#include <ctime>
#include <chrono>
#include <iostream>
#include <map>
#include <vector>
#include <random>

using namespace std::chrono;

std::minstd_rand simple_rand;
int randMax = simple_rand.max();
int randMin = simple_rand.min();
int randInt(int max)
{
     return (int)((long long)(simple_rand() - randMin) * max / (randMax - randMin));
}

#define min(a, b) ((a)<(b)?(a):(b))
#define max(a, b) ((a)>(b)?(a):(b))


void process(int iterations, Picture &target, Picture &pic)
{
     int picw = pic.w();
     int pich = pic.h();

     std::map<Pixel,int> colorsMap;
     std::vector<Pixel> colors;

     for (int y = 0; y < pich; y++)
     {
          for (int x = 0; x < picw; x++)
          {
               Pixel *pixel=target.pixel(x, y);
               colorsMap[*pixel]++;
          }
     }
     for(auto const& iter: colorsMap)
          colors.push_back(iter.first);

     for (int i = 0; i < iterations; i++)
     {
          Pixel color = colors[randInt(colors.size())];

          int radius = 1 + randInt(3);
          int px = randInt(pic.w());
          int py = randInt(pic.h());

          int x1 = max(px - radius, 0);
          int x2 = min(px + radius, picw - 1);
          int y1 = max(py - radius, 0);
          int y2 = min(py + radius, pich - 1);

          int w = x2 - x1 + 1;
          int h = y2 - y1 + 1;

          Picture edited(pic, x1, y1, w, h);
          Picture original(pic, x1, y1, w, h);
          Picture reference(target, x1, y1, w, h);

          for (int x = 0; x < w; x++)
          {
               for (int y = 0; y < h; y++)
               {
                    *edited.pixel(x, y) = color;
               }
          }

          long long distanceToEdited = reference.distance(edited);
          long long distanceToOriginal = reference.distance(original);
          if (distanceToEdited < distanceToOriginal)
          {
               pic.commit(x1, y1, edited);
          }
     }
}

int main(int argc, char **argv)
{
     if (argc != 4)
     {
          printf("Usage: %s <iterations> <source> <destination>\n", argv[0]);
          return 1;
     }

     simple_rand.seed((unsigned int) time(NULL));

     int iterations = std::stoi(argv[1]);
     const char *input = argv[2];
     const char *output = argv[3];

     Picture target(input);
     Picture pic(target.w(), target.h(), Pixel(0, 0, 0));

     high_resolution_clock::time_point t1 = high_resolution_clock::now();
     process(iterations, target, pic);
     high_resolution_clock::time_point t2 = high_resolution_clock::now();

     pic.Save(output);

     auto duration = duration_cast<microseconds>(t2 - t1).count();
     std::cout << "Took: " << 1.0 * duration / 1000000 << "s";

     return 0;
}
