#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <stdint.h>
#include "kheap.h"
#include "fb.h"
#include "util.h"

// Permet de tracer une ligne entre deux points
void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

#endif