/*
 *  DTImage.h
 *  dither Utility
 *
 *  Basic image related structures and function declarations.
 *
 */

#ifndef DT_IMAGE
#define DT_IMAGE

typedef unsigned char byte;

typedef struct {
    byte r, g, b;
} DTPixel;

typedef struct {
    int width;
    int height;
    unsigned long resolution;
    DTPixel *pixels;
} DTImage;

typedef enum {
    t_PPM,
    t_PNG,
    t_UNKNOWN
} DTImageType;

DTImage *CreateImageFromFile(char *filename);
void WriteImageToFile(DTImage *img, char *filename);

DTPixel PixelFromRGB(byte r, byte g, byte b);

#endif
