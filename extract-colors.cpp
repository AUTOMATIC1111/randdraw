#include "ColorExtractor.h"
#include "Shapes.h"

#include "cxxopts.hpp"

void drawSquare(Picture &pic, const Pixel &color, int x1, int y1, int w, int h)
{
     for (int x = x1; x < x1 + w; x++)
     {
          for (int y = y1; y < y1 + h; y++)
          {
               pic.paint(x, y, color);
          }
     }
}


int main(int argc, char **argv)
{
     std::string input;
     std::string output;
     bool quiet;
     int colorCount;

     try
     {
          cxxopts::Options options("extract-colors", " - get a list of important colors from an image");
          options.positional_help("<input> [<output>]");

          options.add_options()
               ("f,input", "input image", cxxopts::value<std::string>(input))
               ("o,output", "output image with desired colorsi in it", cxxopts::value<std::string>(output))
               ("c,count", "how many colors to produce", cxxopts::value<int>(colorCount)->default_value("3"))
               ("q,quiet", "do not output colors to stdout", cxxopts::value<bool>(quiet))
               ("help", "Print help");

          options.parse_positional({"input", "output", "positional"});
          options.parse(argc, argv);

          if (options.count("help") || input.empty() || colorCount < 1)
          {
               std::cout << options.help({""}) << std::endl;
               exit(0);
          }
     }
     catch (const cxxopts::OptionException &e)
     {
          std::cout << "error parsing options: " << e.what() << std::endl;
          exit(1);
     }

     Picture pic(input.c_str());

     ColorExtractor extractor(pic, colorCount);

     if (!output.empty())
     {
          int cellWidth = 24;
          int cellHeight = 48;
          Picture palette(cellWidth * extractor.colors.size(), cellHeight, Pixel(0, 0, 0));

          int i=0;
          for (auto iter=extractor.colorUsage.begin(); iter!=extractor.colorUsage.end(); ++iter)
          {
               drawSquare(palette, iter->first, cellWidth * i, 0, cellWidth, cellHeight);
               i++;
          }

          palette.save(output.c_str());
     }

     if (!quiet)
     {

          for (auto iter=extractor.colorUsage.begin(); iter!=extractor.colorUsage.end(); ++iter)
          {
               Pixel p = iter->first;
               printf("#%02x%02x%02x\n", p.r, p.g, p.b);
          }
     }

     return 0;
}
