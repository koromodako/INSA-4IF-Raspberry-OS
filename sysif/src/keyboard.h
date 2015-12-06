#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "drivers/usb/csud/include/types.h"

// Constantes -----------------------------
#define UPDATE_DEPTH_LIMIT 2
#define KEYDOWN_BUFFER_SIZE 6
#define KEYBOARDS_LIMIT 1


// Fonctions ------------------------------
/**
 *	Cette fonction met en mémoire les 6 dernières touches appuyées de chaque clavier
 */
void KeyboardsUpdate(void);
/**
 *	Cette fonction compte le nombre d'occurence de la touche key dans le buffer
 */
int KeyWasDown(u16 key);
/**
 * Cette fonction retourne le caractere correspondant à la derniere touche enfoncée
 */
char KeyboardGetChar(u32 keyboardIndex);

#endif //KEYBOARD_H