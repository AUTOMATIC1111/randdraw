#ifndef RANDDRAW_PICTURE_H
#define RANDDRAW_PICTURE_H

struct Pixel
{
     union
     {
          struct{
               unsigned char r;
               unsigned char g;
               unsigned char b;
          };
     };

     bool operator < (const Pixel & other) const{
          return (bool) (((*(int *)this) & 0xffffff) < ((*(int *)&other) & 0xffffff));
     }

     Pixel(){}
     Pixel(unsigned char rr,unsigned char gg,unsigned char bb){ r=rr; g=gg; b=bb; }
};

class Picture
{
     int width;
     int height;
     int stride;
     Pixel *pixels;

public:
     Picture(const char *filename);
     Picture(int width, int height, Pixel color);
     Picture(const Picture & other);
     Picture(const Picture & other, int x, int y, int w, int h);
     ~Picture();

     void Save(const char *filename);

     Pixel *pixel(int x, int y){
          return (Pixel *) &pixels[x + y * stride];
     }
     const Pixel *constPixel(int x, int y) const{
          return (Pixel *) &pixels[x + y * stride];
     }

     int w() { return width; }
     int h() { return height; }

     void CopyFrom(const Picture &picture);

     void commit(int x, int y, Picture &pic);

     long long distance(Picture &other);

private:
     void ReadFromBMP(const char *filename);
     void WriteToBmp(const char *filename);

};

#endif //RANDDRAW_PICTURE_H
