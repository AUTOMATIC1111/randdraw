#include <algorithm>
#include "ColorExtractor.h"
#include "Random.h"
#include "KMeans.h"

static Random random;

// using http://www.easyrgb.com/index.php?X=MATH&H=01#text1
void rgb2lab(double R, double G, double B, double &l_s, double &a_s, double &b_s) {
    double var_R = R / 255.0;
    double var_G = G / 255.0;
    double var_B = B / 255.0;


    if (var_R > 0.04045) var_R = pow(((var_R + 0.055) / 1.055), 2.4);
    else var_R = var_R / 12.92;
    if (var_G > 0.04045) var_G = pow(((var_G + 0.055) / 1.055), 2.4);
    else var_G = var_G / 12.92;
    if (var_B > 0.04045) var_B = pow(((var_B + 0.055) / 1.055), 2.4);
    else var_B = var_B / 12.92;

    var_R = var_R * 100.;
    var_G = var_G * 100.;
    var_B = var_B * 100.;

    //Observer. = 2°, Illuminant = D65
    double X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
    double Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
    double Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;


    double var_X = X / 95.047;         //ref_X =  95.047   Observer= 2°, Illuminant= D65
    double var_Y = Y / 100.000;          //ref_Y = 100.000
    double var_Z = Z / 108.883;          //ref_Z = 108.883

    if (var_X > 0.008856) var_X = pow(var_X, (1. / 3.));
    else var_X = (7.787 * var_X) + (16. / 116.);
    if (var_Y > 0.008856) var_Y = pow(var_Y, (1. / 3.));
    else var_Y = (7.787 * var_Y) + (16. / 116.);
    if (var_Z > 0.008856) var_Z = pow(var_Z, (1. / 3.));
    else var_Z = (7.787 * var_Z) + (16. / 116.);

    l_s = (116. * var_Y) - 16.;
    a_s = 500. * (var_X - var_Y);
    b_s = 200. * (var_Y - var_Z);
}

//http://www.easyrgb.com/index.php?X=MATH&H=01#text1
void lab2rgb(double l_s, double a_s, double b_s, double &R, double &G, double &B) {
    double var_Y = (l_s + 16.) / 116.;
    double var_X = a_s / 500. + var_Y;
    double var_Z = var_Y - b_s / 200.;

    if (pow(var_Y, 3) > 0.008856) var_Y = pow(var_Y, 3);
    else var_Y = (var_Y - 16. / 116.) / 7.787;
    if (pow(var_X, 3) > 0.008856) var_X = pow(var_X, 3);
    else var_X = (var_X - 16. / 116.) / 7.787;
    if (pow(var_Z, 3) > 0.008856) var_Z = pow(var_Z, 3);
    else var_Z = (var_Z - 16. / 116.) / 7.787;

    double X = 95.047 * var_X;    //ref_X =  95.047     Observer= 2°, Illuminant= D65
    double Y = 100.000 * var_Y;   //ref_Y = 100.000
    double Z = 108.883 * var_Z;    //ref_Z = 108.883


    var_X = X / 100.;       //X from 0 to  95.047      (Observer = 2°, Illuminant = D65)
    var_Y = Y / 100.;       //Y from 0 to 100.000
    var_Z = Z / 100.;      //Z from 0 to 108.883

    double var_R = var_X * 3.2406 + var_Y * -1.5372 + var_Z * -0.4986;
    double var_G = var_X * -0.9689 + var_Y * 1.8758 + var_Z * 0.0415;
    double var_B = var_X * 0.0557 + var_Y * -0.2040 + var_Z * 1.0570;

    if (var_R > 0.0031308) var_R = 1.055 * pow(var_R, (1 / 2.4)) - 0.055;
    else var_R = 12.92 * var_R;
    if (var_G > 0.0031308) var_G = 1.055 * pow(var_G, (1 / 2.4)) - 0.055;
    else var_G = 12.92 * var_G;
    if (var_B > 0.0031308) var_B = 1.055 * pow(var_B, (1 / 2.4)) - 0.055;
    else var_B = 12.92 * var_B;

    R = var_R * 255.;
    G = var_G * 255.;
    B = var_B * 255.;
}

ColorExtractor::ColorExtractor(const Picture &pic, int count) {
    int w = pic.w();
    int h = pic.h();

    std::map<Pixel, int> colorMap;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Pixel pixel = *pic.constPixel(x, y);

            colorMap[pixel]++;
        }
    }

    KMeans kmeans(3);
    for (auto iter = colorMap.begin(); iter != colorMap.end(); ++iter) {
        const Pixel &pixel = iter->first;
        int weight = sqrt(iter->second);

        double l, a, b;
        rgb2lab(pixel.r, pixel.g, pixel.b, l, a, b);

        kmeans.add(std::vector<double>{l, a, b}, weight);
    }

    kmeans.process(count);

    for (int i = 0; i < kmeans.centers.size(); i++) {
        KMeans::Center &center = kmeans.centers[i];

        double r, g, b;
        lab2rgb(center.value[0], center.value[1], center.value[2], r, g, b);

        Pixel pixel(r, g, b);

        if (center.entries == 0) continue;

        colors.push_back(ColorInfo{
                .pixel=pixel,
                .usage=center.entries,
                .quality=center.averageDistance,
                .l=center.value[0],
                .a=center.value[1],
                .b=center.value[2],
        });
    }

    std::sort(colors.begin(), colors.end(), [](ColorInfo &a, ColorInfo &b) { return a.pixel < b.pixel; });
}


void ColorExtractor::fillColormap(Picture &colormap, const Picture &pic) {
    int w = pic.w();
    int h = pic.h();
    int colorCount = colors.size();

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Pixel pixel = *pic.constPixel(x, y);

            double l, a, b;
            rgb2lab(pixel.r, pixel.g, pixel.b, l, a, b);

            int choice = 0;
            double choiceDistance = 99999;
            for (int j = 0; j < colorCount; j++) {
                ColorInfo &info = colors[j];

                double distance = sqrt(
                        (l - info.l) * (l - info.l) +
                        (a - info.a) * (a - info.a) +
                        (b - info.b) * (b - info.b)
                );

                if (distance < choiceDistance) {
                    choice = j;
                    choiceDistance = distance;
                }
            }

            colormap.paint(x, y, colors[choice].pixel);
        }
    }
}