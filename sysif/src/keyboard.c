#include "keyboard.h"
#include "math.h"
#include "hw.h"
#include "drivers/usb/csud/include/device/hid/keyboard.h"
#include "drivers/usb/csud/include/usbd/usbd.h"

// General usage of driver

    //1. Call UsbInitialise
    //2. Call UsbCheckForChange
    //3. Call KeyboardCount
    //4. If this is 0, go to 2.
    //5. For each keyboard you support:
    //    1. Call KeyboardGetAddress
    //    2. Call KeybordGetKeyDownCount
    //    3. For each key down:
    //        1. Check whether or not it has just been pushed
    //        2. Store that the key is down
    //    4. For each key stored:
    //        1. Check whether or not key is released
    //        2. Remove key if released
    //6. Perform actions based on keys pushed/released
    //Go to 2.

// Variables globales
u16 keys[KEYBOARDS_LIMIT][KEYDOWN_BUFFER_SIZE] = { { 0x0 } }; // Buffer des 6 dernières touches enfoncées pour chaque clavier

char normalKeys[] = 
{ 
    0x0, 0x0, 0x0, 0x0, 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
    'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
    'u', 'v', 'w', 'x', 'y', 'z', '1', '2',
    '3', '4', '5', '6', '7', '8', '9', '0',
    '\n', 0x0, '\b', '\t', ' ', '-', '=', '[',
    ']', '\\', '#', ';', '\'', '`', ',', '.',
    '/', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, '/', '*', '-', '+',
    '\n', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '0', '.', '\\', 0x0, 0x0, '='
};

char shiftKeys[] = 
{ 
    0x0, 0x0, 0x0, 0x0, 'A', 'B', 'C', 'D',
    'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
    'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', '!', '"',
    0x0, '$', '%', '^', '&', '*', '(', ')',
    '\n', 0x0, '\b', '\t', ' ', '_', '+', '{',
    '}', '|', '~', ':', '@', 0x0, '<', '>',
    '?', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, '/', '*', '-', '+',
    '\n', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '0', '.', '|', 0x0, 0x0, '='
};

// Fonctions ---------------------------------------------------------

//  KeyboardUpdate
void KeyboardsUpdate(void) 
{
    UsbCheckForChange();
    // Pour chaque clavier
    u32 kbd_ind;
    for ( kbd_ind = 0; kbd_ind < min(KeyboardCount(), KEYBOARDS_LIMIT); ++kbd_ind)
    {
        // Récupère l'adresse du clavier a l'index souhaité
        u32 kbd_addr = KeyboardGetAddress(kbd_ind);
        // Si l'adresse n'est pas nulle
        if(kbd_addr != 0)
        {
            // Sauvegarde des KEYDOWN_BUFFER_SIZE touches appuyées
            u32 i;
            for (i = 0; i < min(KEYDOWN_BUFFER_SIZE, KeyboardCount(kbd_addr)); ++i)
            {   
                keys[kbd_ind][i] = KeyboardGetKeyDown(kbd_addr, i);
            }
            // Verification de l'etat du clavier
            if(KeyboardPoll(kbd_addr) != 0)
            {   return; // Le clavier a probablement été deconnecté
            }
        }
    }
}

int KeyWasDown(u16 key) {
    int count = 0;
    int kbd_ind, key_ind;
    for (kbd_ind = 0; kbd_ind < KEYBOARDS_LIMIT; ++kbd_ind)
    {   for (key_ind = 0; key_ind < KEYDOWN_BUFFER_SIZE; ++key_ind)
        {
            if( keys[kbd_ind][key_ind] == key ) 
            {
                count++;
            }
        }
    }
    return count;
}

char KeyboardGetChar()
{
    // Pour chaque clavier
    u32 kbd_ind;
    for ( kbd_ind = 0; kbd_ind < min(KeyboardCount(), KEYBOARDS_LIMIT); ++kbd_ind)
    {
        // Récupère l'adresse du clavier a l'index souhaité
        u32 kbd_addr = KeyboardGetAddress(kbd_ind);
        // Si l'adresse n'est pas nulle
        if(kbd_addr != 0)
        {
            KeyboardLedsOff();
            // Lecture des KEYDOWN_BUFFER_SIZE touches enfoncées
            u32 i;
            for (i = 0; i < KEYDOWN_BUFFER_SIZE; ++i)
            {
                // On recupere les touches du buffer
                u16 key = keys[kbd_ind][i];
                // Si la touche est 0 alors erreur clavier
                if( key == 0 )
                {   // On interrompt
                    break;
                }
                // Si deja appuyée
                else if(KeyWasDown(key) != 0)
                {   // Aller au caractere suivant
                    continue;
                }
                // Si en dehors de la table
                else if( key > 103)
                {   // Aller au caractere suivant
                    continue;
                }
                // On vérifie si il y a un modifier
                struct KeyboardModifiers mods = KeyboardGetModifiers(kbd_addr);
                char ckey;
                // Si le modifier shift existe
                if(mods.LeftShift == 1)
                {   // On retourne le caractère depuis la table shift
                    ckey = shiftKeys[key];
                }
                else if(mods.RightShift == 1)
                {// On retourne le caractère depuis la table shift
                    ckey = shiftKeys[key];
                }
                else
                {   // On retourne le caractere depuis la table normale
                    ckey = normalKeys[key];
                }
                // Si le caractere est 0
                if(ckey == 0)
                {   // Aller au caractere suivant
                    continue;
                }
                // On retourne la touche
                return ckey;
            }
        }
    }
    return 0x0;
}

void KeyboardLedsOn() {
    // Pour chaque clavier
    u32 kbd_ind;
    for ( kbd_ind = 0; kbd_ind < min(KeyboardCount(), KEYBOARDS_LIMIT); ++kbd_ind)
    {
        // Récupère l'adresse du clavier a l'index souhaité
        u32 kbd_addr = KeyboardGetAddress(kbd_ind);
        // Si l'adresse n'est pas nulle
        if(kbd_addr != 0)
        {
            struct KeyboardLeds leds = KeyboardGetLedSupport(kbd_addr);
            leds.CapsLock = 1;
            leds.Compose = 1;
            leds.Kana = 1;UsbCheckForChange();
            leds.Mute = 1;
            leds.NumberLock = 1;
            leds.Power = 1;
            leds.ScrollLock = 1;
            leds.Shift = 1;
            KeyboardSetLeds(kbd_addr, leds);
        }
    }
}

void KeyboardLedsOff() {
    // Pour chaque clavier
    u32 kbd_ind;
    for ( kbd_ind = 0; kbd_ind < min(KeyboardCount(), KEYBOARDS_LIMIT); ++kbd_ind)
    {
        // Récupère l'adresse du clavier a l'index souhaité
        u32 kbd_addr = KeyboardGetAddress(kbd_ind);
        // Si l'adresse n'est pas nulle
        if(kbd_addr != 0)
        {
            struct KeyboardLeds leds = KeyboardGetLedSupport(kbd_addr);
            leds.CapsLock = 0;
            leds.Compose = 0;
            leds.Kana = 0;
            leds.Mute = 0;
            leds.NumberLock = 1;
            leds.Power = 0;
            leds.ScrollLock = 0;
            leds.Shift = 0;
            KeyboardSetLeds(kbd_addr, leds);
        }
    }
}

u32 getNbKeyboard() {
    return KeyboardCount();
}

u32 getKeyDownCount() {
    u32 totalCount = 0;
    // Pour chaque clavier
    u32 kbd_ind;
    for ( kbd_ind = 0; kbd_ind < min(KeyboardCount(), KEYBOARDS_LIMIT); ++kbd_ind)
    {
        // Récupère l'adresse du clavier a l'index souhaité
        u32 kbd_addr = KeyboardGetAddress(kbd_ind);
        // Si l'adresse n'est pas nulle
        if(kbd_addr != 0)
        {
            totalCount += KeyboardGetKeyDownCount(kbd_ind);
        }
    }
    return totalCount;
}
