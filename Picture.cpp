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

