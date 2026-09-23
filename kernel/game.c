#include "game.h"

#include "keyboard.h"
#include "levels.h"
#include "terminal.h"

#define COLOR_DEFAULT 0x07
#define COLOR_PLAYER 0x0E
#define COLOR_BOX 0x0C

static const Level *active_level;

static int player_row;
static int player_column;

static Position boxes[MAX_BOXES];
static int box_count;

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

  while (1) {
    if (is_level_complete()) {
      draw_victory();

      while (1) {
        char character = keyboard_get_char();

        if (character == 'q' || character == '\n' && level_index + 1 >= LEVEL_COUNT)
          return;

        if (character == '\n') {
          load_level(++level_index);
          terminal_clear();
          draw_map();        
          break;
        }

        if (character == 'r') {
          load_level(level_index);
          terminal_clear();
          draw_map();
          break;
        }
      }

      continue;
    }

    char character = keyboard_get_char();

    if (character == 'q')
      return;

    if (character == 'r') {
      load_level(level_index);
      terminal_clear();
      draw_map();
      continue;
    }

    if (character == 'w')
      move_player(-1, 0);

    if (character == 's')
      move_player(1, 0);

    if (character == 'a')
      move_player(0, -1);

    if (character == 'd')
      move_player(0, 1);

    draw_map();
  }
}