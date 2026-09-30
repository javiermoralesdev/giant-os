#ifndef TERMINAL_H
#define TERMINAL_H

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define COLOR_BLACK 0x00
#define COLOR_BLUE 0x01
#define COLOR_GREEN 0x02
#define COLOR_CYAN 0x03
#define COLOR_RED 0x04
#define COLOR_MAGENTA 0x05
#define COLOR_BROWN 0x06
#define COLOR_LIGHT_GRAY 0x07
#define COLOR_DARK_GRAY 0x08
#define COLOR_LIGHT_BLUE 0x09
#define COLOR_LIGHT_GREEN 0x0A
#define COLOR_LIGHT_CYAN 0x0B
#define COLOR_LIGHT_RED 0x0C
#define COLOR_LIGHT_MAGENTA 0x0D
#define COLOR_YELLOW 0x0E
#define COLOR_WHITE 0x0F

void terminal_init(void);
void terminal_clear(void);
void terminal_putchar(char character);
void terminal_write(const char *string);
void terminal_new_line(void);
void terminal_write_line(const char *string);
void terminal_set_color(unsigned char color);
void terminal_set_cursor(int row, int column);
void terminal_update_hardware_cursor(int row, int column);
void terminal_enable_cursor(unsigned char cursor_start, unsigned char cursor_end);
void terminal_scroll(void);

#endif