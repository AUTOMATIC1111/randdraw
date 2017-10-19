#include "Picture.h"
#include "Random.h"
#include "Shapes.h"
#include "Processor.h"

#include "cxxopts.hpp"

#include <chrono>

using namespace std::chrono;

int main(int argc, char **argv)
{
     std::string shapesText;
     for(const ShapeInfo &info: shapes){
          if(! shapesText.empty()) shapesText += ", ";
          shapesText+=info.name;
     }

     std::string input;
     std::string output;
     int iterations;
     int iterationsDump;
     std::string shapeName;
     bool measureTime;

     try
     {
          cxxopts::Options options("randdraw", " - paint a picture using simple shapes");
          options.positional_help("<input> <output>");

          options.add_options()
               ("i,iterations", "iteration count", cxxopts::value<int>(iterations)->default_value("10000"))
               ("d,dump", "dump a picture to disk every N iterations; if you use this option, output filename should include %d in it, which will be replaced by a number", cxxopts::value<int>(iterationsDump)->default_value("-1"))
               ("s,shape", "shape to use for drawing; one of: "+shapesText, cxxopts::value<std::string>(shapeName)->default_value("lines"))
               ("f,input", "input image", cxxopts::value<std::string>(input))
               ("m,measure", "measure time taken", cxxopts::value<bool>(measureTime))
               ("o,output", "output image", cxxopts::value<std::string>(output))
               ("help", "Print help")
               ;

          options.parse_positional({"input", "output", "positional"});
          options.parse(argc, argv);

          if (options.count("help") || input.empty() || output.empty())
          {
               std::cout << options.help({""}) << std::endl;
               exit(0);
          }
     }
     catch (const cxxopts::OptionException& e)
     {
          std::cout << "error parsing options: " << e.what() << std::endl;
          exit(1);
     }

     const ShapeInfo *info = &shapes[0];
     for(const ShapeInfo &i: shapes){
          if(shapeName == i.name){
               info=&i;
               break;
          }
     }

     Picture target(input.c_str());

     long long duration = 0;
     high_resolution_clock::time_point t1 = high_resolution_clock::now();

     Processor procesor(target, *info, Pixel(0, 0, 0));
     if(iterationsDump==-1)
     {
          procesor.iterate(iterations);
     }
     else
     {
          int generation = 0;
          for(int iteration = 0; iteration < iterations; iteration+=iterationsDump)
          {
               procesor.iterate(iterationsDump);

               high_resolution_clock::time_point t2 = high_resolution_clock::now();
               duration += duration_cast<microseconds>(t2 - t1).count();

               char filename[1000];
               snprintf(filename, sizeof(filename), output.c_str(), generation);
               procesor.pic.Save(filename);
               generation++;

               t1 = high_resolution_clock::now();
          }
     }

     high_resolution_clock::time_point t2 = high_resolution_clock::now();
     duration += duration_cast<microseconds>(t2 - t1).count();

     if(iterationsDump==-1)
     {
          procesor.pic.Save(output.c_str());
     }

     if(measureTime)
     {
          std::cout << "Took: " << 1.0 * duration / 1000000 << "s";
     }

     return 0;
}
