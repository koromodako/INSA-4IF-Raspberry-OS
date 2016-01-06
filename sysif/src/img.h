#ifndef IMG_H_
#define IMG_H_

#include <stdint.h>

typedef enum {PPM, PGM} typeImage;

typedef struct {
    typeImage type;
    uint32_t width, height;
    uint32_t colorLevel;
    unsigned char * start;
} Image;

Image loadImage(typeImage type, const unsigned char * start, const unsigned char * end);

void displayImage(Image img, uint32_t start_x, uint32_t start_y);

#endif /* IMG_H_ */
