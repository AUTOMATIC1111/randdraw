#include "ColorExtractor.h"
#include "Random.h"

static Random random;

ColorExtractor::ColorExtractor(const Picture &pic, int count)
{
     int w = pic.w();
     int h = pic.h();

     std::map<Pixel, int> colorMap;
     for (int y = 0; y < h; y++)
     {
          for (int x = 0; x < w; x++)
          {
               Pixel pixel=*pic.constPixel(x, y);

               colorMap[pixel]++;
          }
     }

     for (auto iter=colorMap.begin(); iter!=colorMap.end(); ++iter)
     {
          iter->second = sqrt(sqrt(iter->second));
     }

     struct Center
     {
          double coords[3];
          int entryCount;
     };

     std::vector<Center> centers;
     for (int i = 0; i < count; i++)
     {
          int x = random.nextInt(w);
          int y = random.nextInt(h);

          Pixel p = *pic.constPixel(x, y);

          centers.push_back(Center{.coords={(double)p.r, (double)p.g, (double)p.b}, .entryCount=0});
     }

     int iterations = 0;
     while(1)
     {
          std::vector<Center> newCenters;
          for (int i = 0; i < count; i++)
          {
               newCenters.push_back(Center{.coords={0, 0, 0}, .entryCount=0});
          }

          for (auto iter=colorMap.begin(); iter!=colorMap.end(); ++iter)
          {
               int colorCount = iter->second;
               const Pixel &pixel = iter->first;

               int choice = 0;
               double choiceDistance = 99999;
               for (int j = 0; j < count; j++)
               {
                    Center &center = centers[j];
                    double distance =
                         abs(pixel.r - center.coords[0]) +
                         abs(pixel.g - center.coords[1]) +
                         abs(pixel.b - center.coords[2]);

                    if (distance < choiceDistance)
                    {
                         choice = j;
                         choiceDistance = distance;
                    }
               }

               Center &center = newCenters[choice];
               center.coords[0] += pixel.r * colorCount;
               center.coords[1] += pixel.g * colorCount;
               center.coords[2] += pixel.b * colorCount;
               center.entryCount+=colorCount;
          }

          double totalMovement = 0;
          for (int i = 0; i < count; i++)
          {
               Center &newCenter = newCenters[i];
               Center &center = centers[i];

               if(newCenter.entryCount)
               {
                    newCenter.coords[0] /= newCenter.entryCount;
                    newCenter.coords[1] /= newCenter.entryCount;
                    newCenter.coords[2] /= newCenter.entryCount;
               }

               totalMovement +=
                    abs(center.coords[0] - newCenter.coords[0]) +
                    abs(center.coords[1] - newCenter.coords[1]) +
                    abs(center.coords[2] - newCenter.coords[2]);
          }

          centers = newCenters;
          iterations++;

          if (totalMovement < 0.1)
          {
               break;
          }
     }

     for (int i = 0; i < count; i++)
     {
          Center & center = centers[i];
          Pixel pixel((unsigned char) center.coords[0], (unsigned char) center.coords[1], (unsigned char) center.coords[2]);

          colors.push_back(pixel);
          colorUsage[pixel] = center.entryCount;
     }
}
