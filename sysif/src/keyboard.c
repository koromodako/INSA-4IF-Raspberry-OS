#include "keyboard.h"
#include "math.h"
#include "hw.h"
#include "drivers/usb/csud/include/device/hid/keyboard.h"
#include "drivers/usb/csud/include/usbd/usbd.h"

void KeyboardLedsOn() {
    // Pour chaque clavier
    u32 kbd_ind;
    for (kbd_ind = 0; kbd_ind < min(KeyboardCount(), KEYBOARDS_LIMIT); ++kbd_ind) {
        // Recupere l'adresse du clavier a l'index souhaite
        u32 kbd_addr = KeyboardGetAddress(kbd_ind);
        // Si l'adresse n'est pas nulle
        if (kbd_addr != 0) {
            struct KeyboardLeds leds = KeyboardGetLedSupport(kbd_addr);
            leds.CapsLock = 1;
            leds.Compose = 1;
            leds.Kana = 1;
            UsbCheckForChange();
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
    for (kbd_ind = 0; kbd_ind < min(KeyboardCount(), KEYBOARDS_LIMIT); ++kbd_ind) {
        // Recupere l'adresse du clavier a l'index souhaite
        u32 kbd_addr = KeyboardGetAddress(kbd_ind);
        // Si l'adresse n'est pas nulle
        if (kbd_addr != 0) {
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
