#include "ColorExtractorKmeans.h"
#include "ColorExtractorRandom.h"

#include "cxxopts.hpp"

int main(int argc, char **argv) {
    std::string input;
    std::string output;
    std::string colormap;
    std::string colordist;
    std::string method;
    bool quiet;
    int colorCount = 0;

    try {
        cxxopts::Options options("extract-colors", "Gets a list of important colors from an image");
        options.positional_help("<input> [<output>]");

        options.add_options()
            ("f,input", "input image", cxxopts::value<std::string>(input))
            ("o,output", "output image with desired colors in it", cxxopts::value<std::string>(output))
            ("c,count", "how many colors to produce; if omitted, detected automatically",
             cxxopts::value<int>(colorCount))
            ("m,colormap", "apply the produced palette and write result to specified colormap file",
             cxxopts::value<std::string>(colormap))
            ("method", "method for color extraction; must be one of: kmeans, random",
             cxxopts::value<std::string>(method))
            ("q,quiet", "do not output colors to stdout", cxxopts::value<bool>(quiet))
            ("colordist", "color distance function; one of: Manhattan, Euclidean, CIE2000",
             cxxopts::value<std::string>(colordist))
            ("help", "Print help");
        options.parse_positional({"input", "output", "positional"});
        options.parse(argc, argv);

        if (options.count("help") || input.empty() || colorCount < 0) {
            std::cout << options.help({""}) << std::endl;
            exit(0);
        }
    }
    catch (const cxxopts::OptionException &e) {
        std::cout << "error parsing options: " << e.what() << std::endl;
        exit(1);
    }

    Picture pic(input.c_str());

    ColorExtractor *extractor;

    if(method=="random"){
        extractor = new ColorExtractorRandom();
    } else{
        ColorExtractorKmeans *e = new ColorExtractorKmeans();
        if (!colordist.empty())
            e->kmeans.setColorDistanceFunction(colordist);

        extractor = e;
    }

    extractor->extract(pic, colorCount);

    if (!colormap.empty()) {
        Picture colormapPic(pic.w(), pic.h(), Pixel(0, 0, 0));
        extractor->fillColormap(colormapPic, pic);
        colormapPic.save(colormap.c_str());
    }

    if (!output.empty()) {
        std::vector<Pixel> palette;
        for (int i = 0; i < extractor->colors.size(); i++)
            palette.push_back(extractor->colors[i].pixel);

        Picture::fromPalette(palette).save(output.c_str());
    }

    if (!quiet) {
        for (int i = 0; i < extractor->colors.size(); i++) {
            ColorExtractor::ColorInfo &info = extractor->colors[i];
            Pixel p = info.pixel;
            printf("#%02x%02x%02x % 12d %f\n", p.r, p.g, p.b, info.usage, info.quality);
        }
    }

    delete extractor;

    return 0;
}
