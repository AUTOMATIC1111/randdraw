#include "Picture.h"

#include <cstdio>
#include <string>
#include <math.h>
#include <cassert>
#include <cstring>

static char bmp_data[] =
        "BM" "\0\0\0\0" "\0\0" "\0\0" "\x8a\0\0\0"
                "\x7c\0\0\0" "\0\0\0\0" "\0\0\0\0" "\x01\0" "\x20\0"
                "\x03\0\0\0" "\0\0\0\0" "\x13\x0b\0\0" "\x13\x0b\0\0" "\0\0\0\0" "\0\0\0\0"
                "\0\0\xff\0\0\xff\0\0\xff\0\0\0\0\0\0\xff\x01\0\0\0\0\0\0\0\0\0\0\0\x56\xb8"
                "\x1e\xfc\0\0\0\0\0\0\0\0\x66\x66\x66\xfc\0\0\0\0\0\0\0\0\x63\xf8\x28\xff\0"
                "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

static void print_bmp_header(FILE *f, int w, int h) {
    *(int *) (bmp_data + 2) = w * h * 4 + 0x8a;
    *(int *) (bmp_data + 18) = w;
    *(int *) (bmp_data + 22) = h;
    *(int *) (bmp_data + 34) = w * h * 4;

    fwrite(bmp_data, sizeof(bmp_data) - 1, 1, f);
}

static void print_bmp_color(FILE *f, unsigned char r, unsigned char g, unsigned char b) {
    unsigned char color_data[] = {b, g, r, 0xff};

    fwrite(color_data, sizeof(color_data), 1, f);
}


#if 1

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

#else

void rgb2lab(double R, double G, double B, double &l_s, double &a_s, double &b_s) {
    l_s = R;
    a_s = G;
    b_s = B;
}

void lab2rgb(double l_s, double a_s, double b_s, double &R, double &G, double &B) {
    R = l_s;
    G = a_s;
    B = b_s;
}
#endif

Pixel Pixel::fromLab(double l, double a, double b){
    double rr, gg, bb;

    lab2rgb(l, a, b, rr, gg, bb);

    if(rr>255) rr=255;
    if(gg>255) gg=255;
    if(bb>255) bb=255;
    if(rr<0) rr=0;
    if(gg<0) gg=0;
    if(bb<0) bb=0;

    return Pixel(rr, gg, bb);
}

void Pixel::toLab(double &ll, double &aa, double &bb) const{
    rgb2lab(r, g, b, ll, aa, bb);
}


Picture::Picture() {

}

Picture::Picture(const char *filename) {
    std::string tempFilename = std::string(filename) + ".bmp";
    std::string command = "convert \"" + std::string(filename) + "\" +matte \"" + tempFilename + "\"";

    if (system(command.c_str()) != 0) {
        fprintf(stderr, "Running command failed: %s\n", command.c_str());
        exit(1);
    }

    readFromBMP(tempFilename.c_str());

    unlink(tempFilename.c_str());
}

Picture::Picture(int w, int h, Pixel color) {
    width = w;
    height = h;
    stride = w;
    pixels = new Pixel[width * height];
    for (int i = 0; i < w * h; i++) {
        pixels[i] = color;
    }
}

Picture::Picture(const Picture &other) {
    width = other.width;
    height = other.height;
    stride = other.width;
    pixels = new Pixel[width * height];
    copyFrom(other);
}

Picture::Picture(const Picture &other, int x, int y, int w, int h) {
    assert(x >= 0);
    assert(y >= 0);
    assert(w > 0);
    assert(h > 0);
    assert(x + w <= other.width);
    assert(y + h <= other.height);

    width = w;
    stride = w;
    height = h;

    pixels = new Pixel[width * height];
    for (int line = 0; line < height; line++) {
        memcpy(pixel(0, line), other.constPixel(x, y + line), w * sizeof(Pixel));
    }
}

Picture::~Picture() {
    if (pixels != nullptr) {
        delete[] pixels;
    }
}

void Picture::readFromBMP(const char *filename) {
    char header[0x8a];
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Couldn't open file %s for reading: %s\n", filename, strerror(errno));
        exit(1);
    }

    fread(header, 1, sizeof(header), f);

    width = *(int *) (header + 18);
    height = *(int *) (header + 22);
    stride = width;
    size_t bmpStride = (size_t) (width * 4);

    pixels = new Pixel[width * height];

    unsigned char *line = new unsigned char[bmpStride];
    for (int y = height - 1; y >= 0; y--) {
        fread(line, 1, bmpStride, f);

        for (int x = 0; x < width; x++) {
            Pixel *p = pixel(x, y);
            p->r = line[x * 4 + 2];
            p->g = line[x * 4 + 1];
            p->b = line[x * 4 + 0];
        }
    }
    delete[] line;

    fclose(f);
}

void Picture::writeToBmp(const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Couldn't open file %s for writing: %s\n", filename, strerror(errno));
        exit(1);
    }

    print_bmp_header(f, width, height);

    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            Pixel *p = pixel(x, y);
            print_bmp_color(f, p->r, p->g, p->b);
        }
    }

    fclose(f);
}

void Picture::save(const char *filename) {
    std::string tempFilename = std::string(filename) + ".bmp";
    writeToBmp(tempFilename.c_str());

    std::string command = "convert \"" + tempFilename + "\" \"" + std::string(filename) + "\"";
    if (system(command.c_str()) != 0) {
        fprintf(stderr, "Running command failed: %s\n", command.c_str());
        exit(1);
    }

    unlink(tempFilename.c_str());
}

void Picture::copyFrom(const Picture &other) {
    for (int line = 0; line < height; line++) {
        memcpy(pixel(0, line), other.constPixel(0, line), width * sizeof(Pixel));
    }
}

void Picture::commit(int x, int y, Picture &other) {
    int w = other.w();
    int h = other.h();
    for (int line = 0; line < h; line++) {
        memcpy(pixel(x, y + line), other.constPixel(0, line), w * sizeof(Pixel));
    }
}

long long Picture::distance(Picture &other) {
    long long res = 0;

    if (width != other.width) return -1;
    if (height != other.height) return -1;

    for (int line = 0; line < height; line++) {
        Pixel *a = pixel(0, line);
        Pixel *b = other.pixel(0, line);
        for (int i = 0; i < width; i++) {
            res += abs((int) a->r - (int) b->r);
            res += abs((int) a->g - (int) b->g);
            res += abs((int) a->b - (int) b->b);

            a++;
            b++;
        }
    }

    return res;
}

PictureReference::PictureReference(const Picture &other, int x, int y, int w, int h) : reference(other) {
    assert(x >= 0);
    assert(y >= 0);
    assert(w > 0);
    assert(h > 0);
    assert(x + w <= other.width);
    assert(y + h <= other.height);

    width = w;
    stride = other.stride;
    height = h;

    pixels = other.pixels;

    refX = x;
    refY = y;
}

PictureReference::~PictureReference() {
    pixels = nullptr;
}

PictureEditor::PictureEditor(const Picture &originPic, const Picture &targetPic, int x, int y, int w, int h)
        : origin(originPic, x, y, w, h),
          target(targetPic, x, y, w, h),
          improvement(0) {
    pixels = nullptr;
}

