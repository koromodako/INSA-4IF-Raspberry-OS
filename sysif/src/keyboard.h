#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "drivers/usb/csud/include/types.h"

// Constantes -----------------------------
#define UPDATE_DEPTH_LIMIT 2
#define KEYDOWN_BUFFER_SIZE 6
#define KEYBOARDS_LIMIT 2


// Fonctions ------------------------------

char keyboardCall();

/**
 *	Cette fonction met en memoire les 6 dernieres touches appuyees de chaque clavier
 */
void KeyboardsUpdate(void);
/**
 *	Cette fonction compte le nombre d'occurence de la touche key dans le buffer
 */
int KeyWasDown(u16 key);
/**
 * Cette fonction retourne le caractere correspondant a la derniere touche enfoncee
 */
char KeyboardGetChar();

void KeyboardLedsOn();

void KeyboardLedsOff();

u32 getNbKeyboard();

u32 getKeyDownCount();

#endif //KEYBOARD_H
