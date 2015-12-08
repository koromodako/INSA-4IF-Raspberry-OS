#ifndef FONT_H_
#define FONT_H_

#include <stdint.h>

typedef struct {
    uint32_t cursor_x, cursor_y;
    uint32_t min_x, min_y, max_x, max_y;
} FontCursor;

typedef struct {
    uint32_t * widths;
    uint32_t * heights;
    char ** values;
    uint32_t max_width, max_height;
    uint32_t spacing_width;
} FontTable;

/**
 * Initialise la police (requis pour utiliser drawLetter et drawLetters)
 */
FontTable * initFont();

/**
 * Initialise le curseur pour écrire
 */
FontCursor * initCursor(uint32_t min_x, uint32_t min_y, uint32_t max_x, uint32_t max_y);

/**
 * Permet d'afficher une lettre à la position du curseur et l'avance
 */
void drawLetter(FontCursor * cursor, FontTable * font, char letter);

/**
 * Permet d'afficher une phrase à la position du curseur et l'avance
 */
void drawLetters(FontCursor * cursor, FontTable * font, char * letters);

/**
 * Avance le curseur d'une lettre (plus l'espacement entre les lettres)
 */
void advanceCursor(FontCursor * cursor, FontTable * font, uint32_t width);

/**
 * Vérification de la position du curseur et retour à la ligne (ou reset) en cas de dépassement de la zone
 */
void checkCursor(FontCursor * cursor, FontTable * font);

#endif
