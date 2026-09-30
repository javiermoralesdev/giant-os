#include "keyboard.h"
#include "memory.h"
#include "terminal.h"
#include "toolbox.h"
#include "sokoban.h"

int process_command(char* cmd, char* args){
  if (!my_strcmp(cmd, "play")) {
    if (!my_strcmp(args, "sokoban")) {
      play_sokoban();
      goto internal;
    }
    terminal_write_line("Juego no encontrado");
    goto internal;
  }

  if (!my_strcmp(cmd, "help")) {
    terminal_write_line("Comandos: play sokoban, help, clear");
    goto internal;
  }

  if (!my_strcmp(cmd, "clear")) {
    terminal_clear();
    goto internal;
  }

  terminal_write_line("No reconocido");
  return false;

internal:
  return true;
}

void kernel_main(void) {
  kmalloc_init();
  terminal_init();
  while (1) {
    terminal_write("$ ");
    char line[COMMAND_LINE_SIZE];
    int len = keyboard_get_string(line);
    if (len == 0)
      continue;

    char cmd[COMMAND_LINE_SIZE];
    char args[COMMAND_LINE_SIZE];

    int i = 0;
    while (line[i] == ' ') i++;

    int c = 0;
    while (line[i] && line[i] != ' ') {
      cmd[c++] = line[i++];
    }
    cmd[c] = '\0';

    while (line[i] == ' ') i++;

    int a = 0;
    while (line[i]) {
      args[a++] = line[i++];
    }
    args[a] = '\0';

    if (cmd[0] != '\0') {
      process_command(cmd, args);
    }
  }
}