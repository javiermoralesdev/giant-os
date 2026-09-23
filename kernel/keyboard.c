#include "keyboard.h"

static unsigned char inb(unsigned short port)
{
    unsigned char result;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(result)
        : "Nd"(port)
    );

    return result;
}

static int keyboard_has_data(void)
{
    return inb(0x64) & 1;
}

static unsigned char keyboard_read_scancode(void)
{
    while (!keyboard_has_data())
    {
        __asm__ volatile ("pause");
    }

    return inb(0x60);
}

static char scancode_to_char(unsigned char scancode)
{
    switch (scancode)
    {
        case 0x02: return '1';
        case 0x03: return '2';
        case 0x04: return '3';
        case 0x05: return '4';
        case 0x06: return '5';
        case 0x07: return '6';
        case 0x08: return '7';
        case 0x09: return '8';
        case 0x0A: return '9';
        case 0x0B: return '0';

        case 0x10: return 'q';
        case 0x11: return 'w';
        case 0x12: return 'e';
        case 0x1E: return 'a';
        case 0x1F: return 's';
        case 0x20: return 'd';
        case 0x21: return 'f';
        case 0x2E: return 'c';
        case 0x30: return 'b';
        case 0x13: return 'r';

        case 0x1C: return '\n';
        case 0x39: return ' ';

        default: return 0;
    }
}

char keyboard_get_char(void)
{
    while (1)
    {
        unsigned char scancode = keyboard_read_scancode();

        /*
         * Ignore key-release scan codes.
         * The high bit indicates a key release
         * for standard one-byte scan codes.
         */
        if (scancode & 0x80)
            continue;

        char character = scancode_to_char(scancode);

        if (character != 0)
            return character;
    }
}