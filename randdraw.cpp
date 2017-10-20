#include "Picture.h"
#include "Shapes.h"
#include "Processor.h"
#include "Program.h"

#include "cxxopts.hpp"

#include <chrono>

using namespace std::chrono;

int main(int argc, char **argv)
{
     std::string shapesText;
     for (const ShapeInfo &info: shapes)
     {
          if (!shapesText.empty()) shapesText += ", ";
          shapesText += info.name;
     }

     std::string input;
     std::string output;
     int iterationsDump;
     std::string programLine;
     bool measureTime;

     try
     {
          cxxopts::Options options("randdraw", "Paint a picture using simple shapes");
          options.positional_help("<input> <output>");

          options.add_options()
               ("p,program",
                "program; a list of comma separated pairs, each takes form of <shape>:<iterations>; for example lines:10000,squares:20000 means do 10000 iterations drawing lines, then 20000 iterations drawing squares; possible shapes are: " +
                shapesText, cxxopts::value<std::string>(programLine)->default_value("lines:100000"))
               ("d,dump",
                "dump a picture to disk every N iterations; if you use this option, output filename should include %d in it, which will be replaced by a number",
                cxxopts::value<int>(iterationsDump)->default_value("-1"))
               ("f,input", "input image", cxxopts::value<std::string>(input))
               ("m,measure", "measure time taken", cxxopts::value<bool>(measureTime))
               ("o,output", "output image", cxxopts::value<std::string>(output))
               ("help", "Print help");

          options.parse_positional({"input", "output", "positional"});
          options.parse(argc, argv);

          if (options.count("help") || input.empty() || output.empty())
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

     Program program(programLine);
     int iterations = program.totalIterations();
     if (iterations == 0)
     {
          std::cout << "program is empty" << std::endl;
          exit(2);
     }

     Picture target(input.c_str());

     long long duration = 0;
     high_resolution_clock::time_point t1 = high_resolution_clock::now();

     Processor procesor(target, program, Pixel(0, 0, 0));
     if (iterationsDump == -1)
     {
          procesor.iterate(iterations);
     } else
     {
          int generation = 0;
          for (int iteration = 0; iteration < iterations; iteration += iterationsDump)
          {
               procesor.iterate(iterationsDump);

               high_resolution_clock::time_point t2 = high_resolution_clock::now();
               duration += duration_cast<microseconds>(t2 - t1).count();

               char filename[1000];
               snprintf(filename, sizeof(filename), output.c_str(), generation);
               procesor.pic.save(filename);
               generation++;

               t1 = high_resolution_clock::now();
          }
     }

     high_resolution_clock::time_point t2 = high_resolution_clock::now();
     duration += duration_cast<microseconds>(t2 - t1).count();

     if (iterationsDump == -1)
     {
          procesor.pic.save(output.c_str());
     }

     if (measureTime)
     {
          std::cout << "Took: " << 1.0 * duration / 1000000 << "s";
     }

     return 0;
}
