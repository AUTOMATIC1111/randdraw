#include "ColorExtractor.h"
#include "Shapes.h"

#include "cxxopts.hpp"

void drawSquare(Picture &pic, const Pixel &color, int x1, int y1, int w, int h) {
    for (int x = x1; x < x1 + w; x++) {
        for (int y = y1; y < y1 + h; y++) {
            pic.paint(x, y, color);
        }
    }
}


int main(int argc, char **argv) {
    std::string input;
    std::string output;
    std::string colormap;
    bool quiet;
    int colorCount;

    try {
        cxxopts::Options options("extract-colors", "Gets a list of important colors from an image");
        options.positional_help("<input> [<output>]");

        options.add_options()
                ("f,input", "input image", cxxopts::value<std::string>(input))
                ("o,output", "output image with desired colors in it", cxxopts::value<std::string>(output))
                ("c,count", "how many colors to produce", cxxopts::value<int>(colorCount)->default_value("3"))
                ("m,colormap", "filename; output a picture that maps the produced palette to input picture",
                 cxxopts::value<std::string>(colormap))
                ("q,quiet", "do not output colors to stdout", cxxopts::value<bool>(quiet))
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

    ColorExtractor extractor(pic, colorCount);

    if (!colormap.empty()) {
        Picture colormapPic(pic.w(), pic.h(), Pixel(0, 0, 0));
        extractor.fillColormap(colormapPic, pic);
        colormapPic.save(colormap.c_str());
    }

    if (!output.empty()) {
        int cellWidth = 24;
        int cellHeight = 48;
        Picture palette(cellWidth * extractor.colors.size(), cellHeight, Pixel(0, 0, 0));


        for (int i = 0; i<extractor.colors.size(); i++) {
            ColorExtractor::ColorInfo & info=extractor.colors[i];
            Pixel p = info.pixel;
            drawSquare(palette, p, cellWidth * i, 0, cellWidth, cellHeight);
        }

        palette.save(output.c_str());
    }

    if (!quiet) {
        for (int i = 0; i<extractor.colors.size(); i++) {
            ColorExtractor::ColorInfo & info=extractor.colors[i];
            Pixel p = info.pixel;
            printf("#%02x%02x%02x % 12d %f\n", p.r, p.g, p.b, info.usage, info.quality);
        }
    }

    return 0;
}
