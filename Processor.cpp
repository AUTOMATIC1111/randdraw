#include "Processor.h"

#include <map>

Processor::Processor(Picture &targetPic, const ShapeInfo &shapeInfoVar, Pixel initialColor)
     : shapeInfo(shapeInfoVar),
       target(targetPic),
       pic(targetPic.w(), targetPic.h(), initialColor)
{
     picw = pic.w();
     pich = pic.h();

     std::map<Pixel, int> colorsMap;

     for (int y = 0; y < pich; y++)
     {
          for (int x = 0; x < picw; x++)
          {
               Pixel *pixel = target.pixel(x, y);
               colorsMap[*pixel]++;
          }
     }
     for (auto const &iter: colorsMap)
     {
          colors.push_back(iter.first);
     }
}

void Processor::iterate(int iterations)
{
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
