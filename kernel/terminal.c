#include "terminal.h"
#include "io.h"

#define VGA_MEMORY ((volatile unsigned char *)0xB8000)

static int terminal_row = 0;
static int terminal_column = 0;
static unsigned char terminal_color = COLOR_LIGHT_GRAY;

void terminal_update_hardware_cursor(int row, int column)
{
    unsigned short position = (unsigned short)(row * VGA_WIDTH + column);
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((position >> 8) & 0xFF));
}

void terminal_enable_cursor(unsigned char cursor_start, unsigned char cursor_end)
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void terminal_scroll(void)
{
    for (int row = 0; row < VGA_HEIGHT - 1; row++)
    {
        for (int col = 0; col < VGA_WIDTH; col++)
        {
            int dst = 2 * (row * VGA_WIDTH + col);
            int src = 2 * ((row + 1) * VGA_WIDTH + col);
            VGA_MEMORY[dst] = VGA_MEMORY[src];
            VGA_MEMORY[dst + 1] = VGA_MEMORY[src + 1];
        }
    }

    for (int col = 0; col < VGA_WIDTH; col++)
    {
        int dst = 2 * ((VGA_HEIGHT - 1) * VGA_WIDTH + col);
        VGA_MEMORY[dst] = ' ';
        VGA_MEMORY[dst + 1] = terminal_color;
    }

    terminal_row = VGA_HEIGHT - 1;
}

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
    terminal_update_hardware_cursor(terminal_row, terminal_column);
}

void terminal_init(void)
{
    terminal_enable_cursor(14, 15);
    terminal_clear();
}

void terminal_set_color(unsigned char color)
{
    terminal_color = color;
}

void terminal_set_cursor(int row, int column)
{
    if (row < 0) row = 0;
    if (row >= VGA_HEIGHT) row = VGA_HEIGHT - 1;
    if (column < 0) column = 0;
    if (column >= VGA_WIDTH) column = VGA_WIDTH - 1;
    terminal_row = row;
    terminal_column = column;
    terminal_update_hardware_cursor(terminal_row, terminal_column);
}

void terminal_putchar(char character)
{
    if (character == '\n')
    {
        terminal_column = 0;
        terminal_row++;
    }
    else if (character == '\b')
    {
        if (terminal_column > 0)
        {
            terminal_column--;
            int index = 2 * (terminal_row * VGA_WIDTH + terminal_column);
            VGA_MEMORY[index] = ' ';
            VGA_MEMORY[index + 1] = terminal_color;
        }
        else if (terminal_row > 0)
        {
            terminal_row--;
            terminal_column = VGA_WIDTH - 1;
            int index = 2 * (terminal_row * VGA_WIDTH + terminal_column);
            VGA_MEMORY[index] = ' ';
            VGA_MEMORY[index + 1] = terminal_color;
        }
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
    {
        terminal_scroll();
    }

    terminal_update_hardware_cursor(terminal_row, terminal_column);
}

void terminal_write(const char *string)
{
    for (int i = 0; string[i] != '\0'; i++)
    {
        terminal_putchar(string[i]);
    }
}

void terminal_new_line(void)
{
    terminal_putchar('\n');
}

void terminal_write_line(const char *string)
{
    terminal_write(string);
    terminal_new_line();
}
