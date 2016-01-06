#include "img.h"
#include "util.h"
#include "fb.h"

#include <stdint.h>

Image loadImage(typeImage type, const unsigned char * start, const unsigned char * end)
{
    Image img;
    if (type == PPM)
    {

        // Iterator in the image
        char * cursorImage = (char *) start;

        // Check the type
        if (cursorImage[0] != 'P' || cursorImage[1] != '6' ) {
            return img;
        }

        img.type = PPM;

        cursorImage = getNextChar(cursorImage, '\n');
        cursorImage++;

        // Escape comments
        while (*cursorImage == '#') {
            cursorImage = getNextChar(cursorImage, '\n');
            cursorImage++;
        }

        // Width
        img.width = atoi(cursorImage);
        cursorImage = getNextChar(cursorImage, ' ');
        cursorImage++;

        // Height
        img.height = atoi(cursorImage);
        cursorImage = getNextChar(cursorImage, '\n');
        cursorImage++;

        // Color level
        img.colorLevel = atoi(cursorImage);
        cursorImage = getNextChar(cursorImage, '\n');
        cursorImage++;

        // Begin of pixels definitions
        img.start = (unsigned char *) cursorImage;
    }

    return img;
}

void displayImage(Image img, uint32_t start_x, uint32_t start_y)
{
    unsigned char * it = img.start;
    for (uint32_t j = 0; j < img.height; ++j)
    {
        for (uint32_t i = 0; i < img.width; ++i)
        {
            // TODO ajuster la couleur en fonction du niveau
            uint32_t red = (uint32_t) *it;
            it++;
            uint32_t green  = (uint32_t) *it;
            it++;
            uint32_t blue = (uint32_t) *it;
            it++;
            put_pixel_RGB24(start_x + i, start_y + j, red, green, blue);
        }
    }
}
