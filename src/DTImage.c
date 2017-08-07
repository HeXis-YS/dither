/*
 *  DTImage.c
 *  dither Utility
 *
 *  Implementation of image handling functions.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <DTImage.h>

void ReadPixelsFromFile(DTImage *img, FILE *file);

DTImage *
CreateImageFromFile(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
	perror("Could not open image file");
	return NULL;
    }

    char identifier[2];
    size_t read = fread(&identifier, 1, 2, file);
    if (read != 2 || identifier[0] != 'P' || identifier[1] != '6') {
	fprintf(stderr, "Image file of unrecognized format.\n");
	return NULL;
    }

    DTImage *image = malloc(sizeof(DTImage));

    fscanf(file, " %d %d %*d ", &image->width, &image->height);
    image->resolution = image->width * image->height;
    ReadPixelsFromFile(image, file);

    fclose(file);

    return image;
}

void
WriteImageToFile(DTImage *img, char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
	perror("Could not open output file");
	return;
    }

    fprintf(file, "P6\n%d %d\n255\n", img->width, img->height);
    for (int i = 0; i < img->resolution; i++)
	fwrite(&img->pixels[i], sizeof(DTPixel), 1, file);
}

void
ReadPixelsFromFile(DTImage *img, FILE *file)
{
    img->pixels = malloc(sizeof(DTPixel) * img->width * img->height);

    for (int i = 0; i < img->resolution; i++)
	fread(&img->pixels[i], sizeof(DTPixel), 1, file);
}
