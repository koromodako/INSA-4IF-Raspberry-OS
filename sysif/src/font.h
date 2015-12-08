#ifndef FONT_H_
#define FONT_H_

#include <stdint.h>

/**
 * Initialise la police (requis pour utiliser drawLetter et drawLetters)
 */
void initFont();

/**
 * Initialise le curseur pour écrire
 */
void initCursor(uint32_t min_x, uint32_t min_y, uint32_t max_x, uint32_t max_y);

/**
 * Permet d'afficher une lettre à la position du curseur et l'avance
 */
void drawLetter(char letter);

/**
 * Permet d'afficher une phrase à la position du curseur et l'avance
 */
void drawLetters(char * letters);

/**
 * Avance le curseur d'une lettre (plus l'espacement entre les lettres)
 */
void advanceCursor(uint32_t width);

/**
 * Vérification de la position du curseur et retour à la ligne (ou reset) en cas de dépassement de la zone
 */
void checkCursor();

#endif