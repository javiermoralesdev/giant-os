#ifndef KEYBOARD_H
#define KEYBOARD_H

#define COMMAND_LINE_SIZE 1024

char keyboard_get_char(void);
int keyboard_get_string(char* out);

#endif