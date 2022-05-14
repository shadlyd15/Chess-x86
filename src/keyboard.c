#include "keyboard.h"
#include "irq.h"
#include "system.h"
#include "timer.h"
#include "Adafruit_GFX.h"

void print_key(char *s) {
    while (*s != '\0'){
        // c++;
        write(*s);
        s++;
    }
}

u8 keyboard_layout_us[128] ={
        KEY_NULL, KEY_ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
        '-', '=', KEY_BACKSPACE, KEY_TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u',
        'i', 'o', 'p', '[', ']', KEY_ENTER, 0, 'a', 's', 'd', 'f', 'g', 'h', 'j',
        'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
        ',', '.', '/', 0, 0, 0, ' ', 0, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5,
        KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, 0, 0, KEY_HOME, KEY_UP,
        KEY_PAGE_UP, '-', KEY_LEFT, '5', KEY_RIGHT, '+', KEY_END, KEY_DOWN,
        KEY_PAGE_DOWN, KEY_INSERT, KEY_DELETE, 0, 0, 0, KEY_F11, KEY_F12

};

struct Keyboard keyboard;

// bad hack! for a better RNG
static bool seeded = false;

void (*input_handler)(int c) = NULL;

static void keyboard_handler(struct Registers *regs) {
    u8 scancode = (u8) inportb(0x60);
    // print_key("Testo");
    if (!seeded) {
        seed(((u32) scancode) * 17 + timer_get());
        seeded = true;
    }

    if (scancode & 0x80){
        input_handler(keyboard_layout_us[scancode - 0x80]);
    }

}

void keyboard_init(void (*handler)(int c)) {
    input_handler = handler;
    irq_install(1, keyboard_handler);
}
