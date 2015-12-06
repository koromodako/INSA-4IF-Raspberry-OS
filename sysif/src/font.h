#ifndef FONT_H_
#define FONT_H_

#define FONT_WIDTH 7
#define FONT_HEIGHT 13
#define FONT_TABLE_SIZE 128
#define LETTER_SPACING 2

typedef struct {
    char ** values;
} FontTable;

FontTable * initFont();

#endif /* FONT_H_ */
