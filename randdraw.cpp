#include "Picture.h"
#include "Shapes.h"
#include "Processor.h"
#include "ColorExtractorKmeans.h"
#include "ColorExtractorFromPicture.h"

#include "cxxopts.hpp"

#include <chrono>

using namespace std::chrono;


int main(int argc, char **argv) {
    std::string shapesText;
    for (const ShapeInfo &info: shapes) {
        if (!shapesText.empty()) shapesText += ", ";
        shapesText += info.name;
    }

    std::string input;
    std::string output;
    std::string colordist;
    int iterationsDump = -1;
    int colorCount = -1;
    std::string programLine;
    bool measureTime;
    bool createWebm;
    std::string colorsInput;

    try {
        cxxopts::Options options("randdraw", "Paint a picture using simple shapes");
        options.positional_help("<input> <output>");

        options.add_options()
            ("p,program",
             "program; a list of comma separated pairs, each takes form of <shape>:<iterations>; for example lines:10000,squares:20000 means do 10000 iterations drawing lines, then 20000 iterations drawing squares; possible shapes are: " +
             shapesText, cxxopts::value<std::string>(programLine)->default_value("lines:100000"))
            ("d,dump",
             "dump a picture to disk every N iterations; if you use this option, you should either also use --webm, or output filename should include %d in it -- multiple pictures will be created, and %d will be replaced by a number",
             cxxopts::value<int>(iterationsDump))
            ("f,input", "input image", cxxopts::value<std::string>(input))
            ("m,measure", "measure time taken", cxxopts::value<bool>(measureTime))
            ("o,output", "output image", cxxopts::value<std::string>(output))
            ("c,colors", "limit number of colors to use when drawing", cxxopts::value<int>(colorCount))
            ("webm", "produce a video; use together with --dump", cxxopts::value<bool>(createWebm))
            ("colordist", "color distance function (for -c option only!); one of: Manhattan, Euclidean, CIE2000",
             cxxopts::value<std::string>(colordist))
            ("import-colors", "read all colors from the image specified in this argument (instead of from input image)",
             cxxopts::value<std::string>(colorsInput))
            ("allow-worse", "also commit edits to image that make it look less like input image - if the decrease in "
                 "likeness is not too large; the numeric argument specifies the threshold for throwing edits away; "
                 " a value of 0.5 is a good start",
             cxxopts::value<double>(Processor::allowWorseMultiplier))
            ("help", "Print help");

        options.parse_positional({"input", "output", "positional"});
        options.parse(argc, argv);

        if (options.count("help") || input.empty() || output.empty()) {
            std::cout << options.help({""}) << std::endl;
            exit(0);
        }
    }
    catch (const cxxopts::OptionException &e) {
        std::cout << "error parsing options: " << e.what() << std::endl;
        exit(1);
    }

    Program program(programLine);
    int iterations = program.totalIterations();
    if (iterations == 0) {
        std::cout << "program is empty" << std::endl;
        exit(2);
    }

    Picture target(input.c_str());

    std::vector<Pixel> colors;
    ColorExtractor *colorExtractor=NULL;
    if (colorCount > 0) {
        Picture colorPic((colorsInput.empty() ? input : colorsInput).c_str());
        ColorExtractorKmeans *extractor=new ColorExtractorKmeans();
        if (!colordist.empty())
            extractor->kmeans.setColorDistanceFunction(colordist);
        extractor->extract(colorPic, colorCount);
        colorExtractor=extractor;
    } else if(! colorsInput.empty()){
        ColorExtractorFromPicture *extractor = new ColorExtractorFromPicture(colorsInput.c_str());
        extractor->extract(colorsInput.c_str(), 0);
        colorExtractor=extractor;
    }

    if(colorExtractor!=NULL){
        for (int i = 0; i < colorExtractor->colors.size(); i++) {
            ColorExtractor::ColorInfo &info = colorExtractor->colors[i];
            Pixel p = info.pixel;
            colors.push_back(p);
        }
        delete colorExtractor;
    }

    long long duration = 0;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    Processor processor(target, program, colors.empty() ? Pixel(0, 0, 0) : colors[0], colors);
    if (iterationsDump == -1) {
        processor.iterate(iterations);
    } else {
        std::string filenameMask;
        std::vector<std::string> filesToDelete;
        if(createWebm){
            filenameMask = output+".frame%06d.bmp";
        } else{
            filenameMask = output;
        }

        int generation = 0;
        for (int iteration = 0; iteration < iterations; iteration += iterationsDump) {
            processor.iterate(iterationsDump);

            high_resolution_clock::time_point t2 = high_resolution_clock::now();
            duration += duration_cast<microseconds>(t2 - t1).count();

            char filename[1000];
            snprintf(filename, sizeof(filename), filenameMask.c_str(), generation);

            if(createWebm) {
                processor.pic.writeToBmp(filename);
                filesToDelete.push_back(filename);
            } else{
                processor.pic.save(filename);
            }

            generation++;
            t1 = high_resolution_clock::now();
        }

        if(createWebm) {
            std::string command = "ffmpeg -r 60 -y -f image2  -i \""+filenameMask+"\" -c:v libvpx -b:v 5M  -auto-alt-ref 0 \""+output+"\"";

            if (system(command.c_str()) != 0) {
                fprintf(stderr, "Running command failed: %s\n", command.c_str());
                exit(1);
            }

            for(const std::string & filename: filesToDelete){
                unlink(filename.c_str());
            }
        }
    }

    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration += duration_cast<microseconds>(t2 - t1).count();

    if (iterationsDump == -1) {
        processor.pic.save(output.c_str());
    }

    if (measureTime) {
        std::cout << "Took: " << 1.0 * duration / 1000000 << "s";
    }

    return 0;
}
