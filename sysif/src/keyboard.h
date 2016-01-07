#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "drivers/usb/csud/include/types.h"

// Constantes -----------------------------
#define UPDATE_DEPTH_LIMIT 2
#define KEYDOWN_BUFFER_SIZE 6
#define KEYBOARDS_LIMIT 2


// Fonctions ------------------------------

u32 KeyboardAddress;

u16* KeyboardOldDown;

char* KeysNormal;

char* KeysShift;

void KeyboardUpdate();

bool KeyWasDown(u16 scanCode);

char KeyboardGetChar();

void KeyboardLedsOn();

void KeyboardLedsOff();

u32 getNbKeyboard();

#endif //KEYBOARD_H
