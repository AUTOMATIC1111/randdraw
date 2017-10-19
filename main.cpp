#include "Picture.h"
#include "Random.h"
#include "Shapes.h"

#include <cstdio>
#include <string>
#include <ctime>
#include <chrono>
#include <iostream>
#include <map>

using namespace std::chrono;

void process(int iterations, Picture &target, Picture &pic, const ShapeInfo &shapeInfo)
{
     int picw = pic.w();
     int pich = pic.h();

     std::map<Pixel, int> colorsMap;
     std::vector<Pixel> colors;

     for (int y = 0; y < pich; y++)
     {
          for (int x = 0; x < picw; x++)
          {
               Pixel *pixel = target.pixel(x, y);
               colorsMap[*pixel]++;
          }
     }
     for (auto const &iter: colorsMap)
          colors.push_back(iter.first);

     for (int i = 0; i < iterations; i++)
     {
          Pixel color = colors[random.nextInt((int) colors.size())];


          int w, h;
          shapeInfo.selectSize(w, h);

          if (w > picw) w = picw;
          if (h > pich) h = pich;

          int px = random.nextInt(picw - w);
          int py = random.nextInt(pich - h);

          unsigned int seed = random.seed();
          PictureEditor edited(pic, target, px, py, w, h);

          shapeInfo.draw(edited, color, w, h);
          if (edited.improvement > 0)
          {
               PictureReference ref(pic, px, py, w, h);

               random.seed(seed);
               shapeInfo.draw(ref, color, w, h);
          }
     }
}

int main(int argc, char **argv)
{
     if (argc < 3 || argc > 5)
     {
          printf("Usage: %s <source> <destination> [<shape> <iterations>]\n", argv[0]);
          printf("Shape can be any of:\n");
          for(const ShapeInfo &info: shapes){
               printf("  %s\n",info.name);
          }
          return 1;
     }

     const char *input = argv[1];
     const char *output = argv[2];
     int iterations = argc > 4 ? std::stoi(argv[4]) : 100000;
     std::string shapeName = argc > 3 ? argv[3] : "";

     const ShapeInfo *info = &shapes[0];
     for(const ShapeInfo &i: shapes){
          if(shapeName == i.name){
               info=&i;
               break;
          }
     }

     Picture target(input);
     Picture pic(target.w(), target.h(), Pixel(0, 0, 0));

     high_resolution_clock::time_point t1 = high_resolution_clock::now();
     process(iterations, target, pic, *info);
     high_resolution_clock::time_point t2 = high_resolution_clock::now();

     pic.Save(output);

     auto duration = duration_cast<microseconds>(t2 - t1).count();
     std::cout << "Took: " << 1.0 * duration / 1000000 << "s";

     return 0;
}
