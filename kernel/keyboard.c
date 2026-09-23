#include "keyboard.h"

static unsigned char inb(unsigned short port) {
  unsigned char result;

  __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));

  return result;
}

static int keyboard_has_data(void) { return inb(0x64) & 1; }

static unsigned char keyboard_read_scancode(void) {
  while (!keyboard_has_data()) {
    __asm__ volatile("pause");
  }

  return inb(0x60);
}

// Tabla de traducción para PS/2 Scan Code Set 1 (sin pulsar Shift)
static const char scancode_map[128] = {
    0,   27,  '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
    '-', '=', '\b', '\t', 'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
    'o', 'p', '[',  ']',  '\n', 0,   'a', 's',  'd', 'f', 'g', 'h',
    'j', 'k', 'l',  ';',  '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm',  ',',  '.',  '/', 0,   '*',  0,   ' '};

static char scancode_to_char(unsigned char scancode) {
  // Ignoramos scancodes fuera del rango de la tabla o scancodes de liberación
  // de tecla (release >= 0x80)
  if (scancode >= 128) {
    return 0;
  }

  return scancode_map[scancode];
}

char keyboard_get_char(void) {
  while (1) {
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