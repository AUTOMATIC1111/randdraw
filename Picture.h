#ifndef RANDDRAW_PICTURE_H
#define RANDDRAW_PICTURE_H

#include <math.h>
#include <vector>

struct Pixel {
    union {
        struct {
            unsigned char r;
            unsigned char g;
            unsigned char b;
        };
    };

    bool operator<(const Pixel &other) const {
        return (bool) (((*(int *) this) & 0xffffff) < ((*(int *) &other) & 0xffffff));
    }

    Pixel() {}

    static Pixel fromLab(double l, double a, double b);

    void toLab(double &l, double &a, double &b) const;

    Pixel(unsigned char rr, unsigned char gg, unsigned char bb) {
        r = rr;
        g = gg;
        b = bb;
    }

    void set(double rr, double gg, double bb){
        r = (unsigned char) (rr < 0 ? 0 : rr >= 255 ? 255 : rr);
        g = (unsigned char) (gg < 0 ? 0 : gg >= 255 ? 255 : gg);
        b = (unsigned char) (bb < 0 ? 0 : bb >= 255 ? 255 : bb);
    }
};

class Picture {
protected:
    Picture();

public:
    int width;
    int height;
    int stride;
    Pixel *pixels;

    Picture(const char *filename);

    Picture(int width, int height, Pixel color);

    Picture(const Picture &other);

    Picture(const Picture &other, int x, int y, int w, int h);

    virtual ~Picture();

    void save(const char *filename);

    virtual Pixel *pixel(int x, int y) {
        return (Pixel *) &pixels[x + y * stride];
    }

    virtual const Pixel *constPixel(int x, int y) const {
        return (Pixel *) &pixels[x + y * stride];
    }

    virtual void paint(int x, int y, Pixel color) {
        *pixel(x, y) = color;
    }

    virtual void paintWithAlpha(int x, int y, Pixel color, double alpha) {
        double q = 1.0 - alpha;

        Pixel *p=pixel(x, y);
        p->set(color.r * alpha + p->r * q, color.g * alpha + p->g * q, color.b * alpha + p->b * q);
    }

    int w() const {
        return width;
    }

    int h() const {
        return height;
    }

    void copyFrom(const Picture &picture);

    void commit(int x, int y, Picture &pic);

    long long distance(Picture &other);

    void readFromBMP(const char *filename);

    void writeToBmp(const char *filename);

    static Picture fromPalette(const std::vector<Pixel> palette);
};

class PictureReference : public Picture {
    const Picture &reference;
    int refX;
    int refY;

public:
    PictureReference(const Picture &origin, int x, int y, int w, int h);

    ~PictureReference();

    Pixel *pixel(int x, int y) {
        return (Pixel *) &pixels[x + refX + (y + refY) * stride];
    }

    const Pixel *constPixel(int x, int y) const {
        return (Pixel *) &pixels[x + refX + (y + refY) * stride];
    }
};

class PictureEditor : public Picture {
    PictureReference target;
    PictureReference origin;

public:
    PictureEditor(const Picture &origin, const Picture &target, int x, int y, int w, int h);

    int improvement;

    int colorDifference(const Pixel *a, const Pixel *b) {
        return
            abs((int) a->r - (int) b->r) +
            abs((int) a->g - (int) b->g) +
            abs((int) a->b - (int) b->b);
    }

    void paint(int x, int y, Pixel color) {
        Pixel *targetPixel = target.pixel(x, y);

        int originalDifference = colorDifference(targetPixel, origin.pixel(x, y));
        int newDifference = colorDifference(targetPixel, &color);
        improvement += originalDifference - newDifference;
    }


    virtual void paintWithAlpha(int x, int y, Pixel color, double alpha) {
        double q = 1.0 - alpha;

        Pixel p=*origin.pixel(x, y);
        p.set(color.r * alpha + p.r * q, color.g * alpha + p.g * q, color.b * alpha + p.b * q);
        paint(x, y, p);
    }

};

#endif //RANDDRAW_PICTURE_H
