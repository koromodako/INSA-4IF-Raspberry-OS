#include "img.h"
#include "util.h"
#include "fb.h"
#include "math.h"

#include <stdint.h>

Image loadImage(typeImage type, const unsigned char * start, const unsigned char * end)
{
    Image img;
    switch (type)
    {
        case PPM :
        case PGM :
        {
            // Iterator in the image
            char * cursorImage = (char *) start;

            // Check the type
            if (cursorImage[0] == 'P' && cursorImage[1] == '6') {
                img.type = PPM;
            } else if (cursorImage[0] == 'P' && cursorImage[1] == '5' ) {
                img.type = PGM;
            } else {
                return img;
            }

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
            img.end = (unsigned char *) end;

            break;
        }
        default:
            // Format non supporté
            break;
    }

    return img;
}

void displayImage(Image img, uint32_t start_x, uint32_t start_y, uint32_t limit_x, uint32_t limit_y)
{
    unsigned char * it = img.start;
    for (uint32_t j = 0; j < img.height; ++j)
    {
        if (start_y + j > limit_y) {
            break;
        }
        for (uint32_t i = 0; i < img.width; ++i)
        {
            if (it == img.end) {
                return;
            } else if (start_x + i > limit_x) {
                break;
            }
            switch (img.type)
            {
                case PPM:
                {
                    uint32_t red = divide32(((uint32_t) *it) * NB_COLOR_LEVEL, img.colorLevel);
                    it++;
                    uint32_t green = divide32(((uint32_t) *it) * NB_COLOR_LEVEL, img.colorLevel);
                    it++;
                    uint32_t blue = divide32(((uint32_t) *it) * NB_COLOR_LEVEL, img.colorLevel);
                    it++;
                    put_pixel_RGB24(start_x + i, start_y + j, red, green, blue);
                    break;
                }
                case PGM:
                {
                    uint32_t gray = divide32(((uint32_t) *it) * NB_COLOR_LEVEL, img.colorLevel);
                    it++;
                    put_pixel_RGB24(start_x + i, start_y + j, gray, gray, gray);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }
}
