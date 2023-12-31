/*
 *  main.c
 *  dither Utility
 *
 *  Created on August 2017 by Cesar Tessarin.
 *  Rewritten and expanded from the 2010 version.
 *
 */

#include "DTDither.h"
#include "DTImage.h"
#include "DTPalette.h"
#include "MCQuantization.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
#include "getopt.h"
#else
#include <unistd.h>
#endif

DTPalette *PaletteForIdentifier(char *s, DTImage *img);
DTPalette *ReadPaletteFromStdin(int size);
DTPalette *QuantizedPaletteForImage(DTImage *image, int size);

int main(int argc, char **argv) {
    char *paletteID = NULL;
    char *inputFile, *outputFile;
    int verbose = 0;
    int dither = 1;
    int c;

    opterr = 0;

    while ((c = getopt(argc, argv, "dvp:")) != -1) {
        switch (c) {
            case 'p':
                paletteID = optarg;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'd':
                dither = 0;
                break;
            case '?':
                fprintf(stderr, "Ignoring unknown option: -%c\n", optopt);
        }
    }

    /* check if there is still two arguments remaining, for i/o */
    if (argc - optind != 2) {
        fprintf(stderr, "Usage: %s [-p palette[.size]] [-dv] input output\n", argv[0]);
        return 1;
    }

    /* all arguments ok */
    inputFile = argv[optind];
    outputFile = argv[optind + 1];

    DTImage *input = CreateImageFromFile(inputFile);
    if (input == NULL) {
        return 2;
    }

    DTPalette *palette = PaletteForIdentifier(paletteID, input);
    if (palette == NULL) {
        return 3;
    }

    /* dump palette if verbose option was set */
    if (verbose) {
        for (int i = 0; i < palette->size; i++) {
            printf("%d %d %d\n", palette->colors[i].r, palette->colors[i].g, palette->colors[i].b);
        }
    }

    if (dither) {
        ApplyFloydSteinbergDither(input, palette);
    } else {
        /* closest color only */
        DTPixel *pixel;
        for (int i = 0; i < input->height; i++) {
            for (int j = 0; j < input->width; j++) {
                pixel = &input->pixels[i * input->width + j];
                *pixel = FindClosestColorFromPalette(*pixel, palette);
            }
        }
    }

    WriteImageToFile(input, outputFile);

    return 0;
}

DTPalette *PaletteForIdentifier(char *str, DTImage *image) {
    if (str == NULL) {
        return StandardPaletteRGB();
    }

    char *name, *sizeStr;
    char *sep = ".";
    name = strtok(str, sep);
    sizeStr = strtok(NULL, sep);
    int size = 0;

    /* if size was inserted, transform to int and check if is valid */
    if (sizeStr) {
        size = (int)strtol(sizeStr, (char **)NULL, 10);
        if (size <= 0) {
            fprintf(stderr, "Invalid palette size, aborting.\n");
            return NULL;
        }
    }

    /* RGB */
    if (strcmp(name, "rgb") == 0) {
        if (size) {
            fprintf(stderr, "Ignored palette size.\n");
        }
        return StandardPaletteRGB();
    } else if (strcmp(name, "bw") == 0) {
        if (size == 1) {
            fprintf(stderr, "Invalid palette size for B&W. Must be at least 2.\n");
            return NULL;
        } else if (!size) {
            size = 2;
        }
        return StandardPaletteBW(size);
    } else if (strcmp(name, "custom") == 0) {
        if (!size) {
            fprintf(stderr, "Size required for custom palette, aborting.\n");
            return NULL;
        }
        return ReadPaletteFromStdin(size);
    } else if (strcmp(name, "auto") == 0) {
        if (!size) {
            fprintf(stderr, "Size required for automatic palette, aborting.\n");
            return NULL;
        } else if (size - 1) {
            fprintf(stderr, "Size must be a power of 2, aborting.\n");
            return NULL;
        }
        return QuantizedPaletteForImage(image, size);
    }

    /* unknown palette */
    fprintf(stderr, "Unrecognized palette identifier, aborting.\n");
    return NULL;
}

DTPalette *ReadPaletteFromStdin(int size) {
    DTPalette *palette = malloc(sizeof(DTPalette));
    palette->size = size;
    palette->colors = malloc(sizeof(DTPixel) * size);

    int r, g, b;
    for (int i = 0; i < size; i++) {
        scanf(" %d %d %d", &r, &g, &b);
        palette->colors[i].r = r;
        palette->colors[i].g = g;
        palette->colors[i].b = b;
    }

    return palette;
}

DTPalette *QuantizedPaletteForImage(DTImage *image, int size) {
    MCTriplet *data = malloc(sizeof(MCTriplet) * image->resolution);
    MCTriplet *colors;
    DTPalette *palette;

    for (int i = 0; i < image->resolution; i++) {
        data[i] = MCTripletMake(image->pixels[i].r, image->pixels[i].g, image->pixels[i].b);
    }

    colors = MCQuantizeData(data, image->resolution, log2(size));

    palette = malloc(sizeof(DTPalette));
    palette->size = size;
    palette->colors = malloc(sizeof(DTPixel) * size);

    for (int i = 0; i < size; i++) {
        palette->colors[i].r = colors[i].value[0];
        palette->colors[i].g = colors[i].value[1];
        palette->colors[i].b = colors[i].value[2];
    }

    free(data);
    free(colors);

    return palette;
}

/* vim:set ts=8 sts=4 sw=4 */
