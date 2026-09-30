#include "sokoban.h"

#include "keyboard.h"
#include "levels.h"
#include "terminal.h"
#include "toolbox.h"

#define COLOR_DEFAULT 0x07
#define COLOR_PLAYER 0x0E
#define COLOR_BOX 0x0C

static const Level *active_level;

static int player_row;
static int player_column;

static Position boxes[MAX_BOXES];
static int box_count;

static int quit = false;
static int current_level = 0;

static void load_level(int level_index) {
  active_level = levels_get(level_index);

  if (active_level == 0)
    return;

  player_row = active_level->player_start_row;
  player_column = active_level->player_start_column;

  box_count = active_level->box_count;

  for (int i = 0; i < box_count; i++) {
    boxes[i] = active_level->box_starts[i];
  }
}

static int get_box_at(int row, int column) {
  for (int i = 0; i < box_count; i++) {
    if (boxes[i].row == row && boxes[i].column == column) {
      return i;
    }
  }

  return -1;
}

static void draw_map(void) {
  for (int row = 0; row < active_level->height; row++) {
    for (int column = 0; column < active_level->width; column++) {
      terminal_set_cursor(row, column);

      char tile = active_level->rows[row][column];
      int box_index = get_box_at(row, column);

      if (row == player_row && column == player_column) {
        terminal_set_color(COLOR_PLAYER);
        terminal_putchar('@');
      } else if (box_index != -1) {
        if (tile == '.') {
          terminal_set_color(0x0A);
          terminal_putchar('*');
        } else {
          terminal_set_color(COLOR_BOX);
          terminal_putchar('$');
        }
      } else if (tile == '#') {
        terminal_set_color(0x08);
        terminal_putchar('#');
      } else if (tile == '.') {
        terminal_set_color(0x0E);
        terminal_putchar('.');
      } else {
        terminal_set_color(COLOR_DEFAULT);
        terminal_putchar(' ');
      }
    }
  }
}

static int is_wall(int row, int column) {
  if (!active_level || row < 0 || row >= active_level->height || column < 0 || column >= active_level->width)
    return 1;
  return active_level->rows[row][column] == '#';
}

static void move_player(int row_delta, int column_delta) {
  int new_row = player_row + row_delta;
  int new_column = player_column + column_delta;

  if (is_wall(new_row, new_column))
    return;

  int box_index = get_box_at(new_row, new_column);

  if (box_index == -1) {
    player_row = new_row;
    player_column = new_column;
    return;
  }

  int box_new_row = new_row + row_delta;
  int box_new_column = new_column + column_delta;

  if (is_wall(box_new_row, box_new_column))
    return;

  if (get_box_at(box_new_row, box_new_column) != -1)
    return;

  boxes[box_index].row = box_new_row;
  boxes[box_index].column = box_new_column;

  player_row = new_row;
  player_column = new_column;
}

static int is_level_complete(void) {
  for (int i = 0; i < box_count; i++) {
    int row = boxes[i].row;
    int column = boxes[i].column;

    if (active_level->rows[row][column] != '.')
      return 0;
  }

  return 1;
}

static void draw_victory(void) {
  terminal_set_color(0x0A);
  terminal_set_cursor(10, 2);
  terminal_write("LEVEL COMPLETE!");

  terminal_set_color(0x07);
  terminal_set_cursor(11, 2);
  terminal_write("Press Q to return to the menu.");
  terminal_set_cursor(12, 2);
  terminal_write("Press Enter to continue.");
}

void game_run(int level_index) {
  terminal_clear();

  load_level(level_index);
  draw_map();

  while (!quit) {
    if (is_level_complete()) {
      draw_victory();

      while (!quit) {
        char character = keyboard_get_char();

        if (character == 'q' || character == 'Q' ||
            (character == '\n' && level_index + 1 >= LEVEL_COUNT))
          return;

        if (character == '\n') {
          current_level = ++level_index;
          load_level(current_level);
          terminal_clear();
          draw_map();
          break;
        }

        if (character == 'r' || character == 'R') {
          load_level(level_index);
          terminal_clear();
          draw_map();
          break;
        }
      }

      continue;
    }

    char character = keyboard_get_char();

    if (character == 'q' || character == 'Q')
      return;

    if (character == 'r' || character == 'R') {
      load_level(level_index);
      terminal_clear();
      draw_map();
      continue;
    }

    if (character == 'w' || character == 'W')
      move_player(-1, 0);

    if (character == 's' || character == 'S')
      move_player(1, 0);

    if (character == 'a' || character == 'A')
      move_player(0, -1);

    if (character == 'd' || character == 'D')
      move_player(0, 1);

    draw_map();
  }
}

typedef enum { SCREEN_MENU, SCREEN_LEVEL_SELECT, SCREEN_GAME, SCREEN_EXIT } Screen;
static Screen current_screen = SCREEN_MENU;

static int hex_to_level_index(char character) {
  if (character >= '0' && character <= '9')
    return character - '0';

  if (character >= 'a' && character <= 'f')
    return character - 'a' + 10;

  if (character >= 'A' && character <= 'F')
    return character - 'A' + 10;

  return -1;
}

static void draw_level_selector(void) {
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

  terminal_set_cursor(10, 20);
  terminal_write("Q - Return");
}

static int select_level(void) {
  draw_level_selector();

  while (!quit) {
    char character = keyboard_get_char();

    if (character == 'q' || character == 'Q')
      return -1;

    int level_index = hex_to_level_index(character);
    if (level_index >= 0 && level_index < LEVEL_COUNT)
      return level_index;
  }
  return -1;
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
  terminal_write("[Q] Quit to shell");

  terminal_set_cursor(10, 25);
  terminal_write("Select an option:");
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
    case 'Q':
      return SCREEN_EXIT;
    }
  }
}

void play_sokoban(void) {
  quit = false;
  current_screen = SCREEN_MENU;

  while (current_screen != SCREEN_EXIT) {
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

    case SCREEN_EXIT:
      break;
    }
  }
  terminal_clear();
}