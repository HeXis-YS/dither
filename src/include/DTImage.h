#ifndef DT_IMAGE
#define DT_IMAGE

typedef struct {
    unsigned char r, g, b;
} DTPixel;

typedef struct {
    int width;
    int height;
    DTPixel *pixels;
} DTImage;

DTImage *CreateImageFromFile(char *filename);

#endif