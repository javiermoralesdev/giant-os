#include "terminal.h"

#define VGA_MEMORY ((volatile unsigned char *)0xB8000)

static int terminal_row = 0;
static int terminal_column = 0;
static unsigned char terminal_color = COLOR_LIGHT_GRAY;

void terminal_clear(void)
{
    for (int row = 0; row < VGA_HEIGHT; row++)
    {
        for (int column = 0; column < VGA_WIDTH; column++)
        {
            int index = 2 * (row * VGA_WIDTH + column);

            VGA_MEMORY[index] = ' ';
            VGA_MEMORY[index + 1] = terminal_color;
        }
    }

    terminal_row = 0;
    terminal_column = 0;
}

void terminal_set_color(unsigned char color)
{
    terminal_color = color;
}

void terminal_set_cursor(int row, int column)
{
    terminal_row = row;
    terminal_column = column;
}

void terminal_putchar(char character)
{
    if (character == '\n')
    {
        terminal_column = 0;
        terminal_row++;
    }
    else
    {
        int index = 2 * (terminal_row * VGA_WIDTH + terminal_column);

        VGA_MEMORY[index] = character;
        VGA_MEMORY[index + 1] = terminal_color;

        terminal_column++;

        if (terminal_column >= VGA_WIDTH)
        {
            terminal_column = 0;
            terminal_row++;
        }
    }

    if (terminal_row >= VGA_HEIGHT)
        terminal_row = 0;
}

void terminal_write(const char *string)
{
    for (int i = 0; string[i] != '\0'; i++)
    {
        terminal_putchar(string[i]);
    }
}