#include "toolbox.h"
#include "terminal.h"
#include "keyboard.h"
#include "levels.h"
#include "game.h"


static int current_level = 0;

typedef enum { SCREEN_MENU, SCREEN_LEVEL_SELECT, SCREEN_GAME } Screen;
static Screen current_screen = SCREEN_MENU;



static int get_box_at(int row, int column);


static int hex_to_level_index(char character) {
  if (character >= '0' && character <= '9')
    return character - '0';

  if (character >= 'a' && character <= 'f')
    return character - 'a' + 10;

  return -1;
}

static void draw_level_selector(void)
{
    terminal_clear();

    terminal_set_color(0x0F);
    terminal_set_cursor(2, 25);
    terminal_write("SELECT LEVEL");

    terminal_set_cursor(5, 20);
    terminal_write("Available levels:");

    terminal_set_cursor(7, 20);
    terminal_write("0 1 2 3 4 5 6 7");
    terminal_set_cursor(8, 20);
    terminal_write("8 9 A B C D E F");

    terminal_set_cursor(9, 20);
    terminal_write("Q - Return");
}

static int select_level(void) {
  draw_level_selector();

  while (1) {
    char character = keyboard_get_char();

    if (character == 'q')
      return -1;

    int level_index = hex_to_level_index(character);
    terminal_putchar(character);
    if (level_index >= 0 && level_index < LEVEL_COUNT)
      return level_index;
  }
}

static void draw_menu(void) {
  terminal_clear();

  terminal_set_color(0x0F);

  terminal_set_cursor(2, 25);
  terminal_write("GIANT OS");

  terminal_set_cursor(5, 25);
  terminal_write("[1] Start game");

  terminal_set_cursor(6, 25);
  terminal_write("[2] Select level");

  terminal_set_cursor(7, 25);
  terminal_write("[Q] Quit");

  terminal_set_cursor(10, 25);
  terminal_write("Select an option:");
  int len = my_strlen("================================");
  terminal_set_cursor(VGA_HEIGHT - 4, 25);
  terminal_write("================================");
  terminal_set_cursor(VGA_HEIGHT - 3, 25);
  terminal_write("         EL GIGANTE OS          ");
  terminal_set_cursor(VGA_HEIGHT - 2, 25);
  terminal_write("    (C)  CARDIFF ELECTRIC       ");
  terminal_set_cursor(VGA_HEIGHT - 1, 25);
  terminal_write("================================");
}

static Screen process_menu(void) {
  while (1) {
    char character = keyboard_get_char();

    switch (character) {
    case '1':
      return SCREEN_GAME;

    case '2':
      return SCREEN_LEVEL_SELECT;

    case 'q':
      return SCREEN_MENU;
    }
  }
}



void kernel_main(void) {
  while (1) {
    switch (current_screen) {
    case SCREEN_MENU:
      draw_menu();
      current_screen = process_menu();
      break;

    case SCREEN_LEVEL_SELECT: {
      int selected_level = select_level();

      if (selected_level >= 0) {
        current_level = selected_level;
        current_screen = SCREEN_GAME;
      } else {
        current_screen = SCREEN_MENU;
      }
      break;
    }
    case SCREEN_GAME:
      game_run(current_level);
      current_screen = SCREEN_MENU;
      break;
    }
  }
}