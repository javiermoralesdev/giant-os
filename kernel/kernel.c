#include "keyboard.h"
#include "memory.h"
#include "terminal.h"
#include "toolbox.h"
#include "sokoban.h"
#include "io.h"

static void power_off(void) {
  terminal_write_line("Apagando el sistema...");

  // ACPI shutdown for QEMU (modern default piix4 / q35)
  outw(0x604, 0x2000);

  // ACPI shutdown for older QEMU / Bochs
  outw(0xB004, 0x2000);

  // ACPI shutdown for VirtualBox
  outw(0x4004, 0x3400);

  // Cloud-hypervisor / QEMU debug exit fallback
  outb(0x501, 0x31);

  // Fallback for bare metal or emulators without ACPI shutdown port:
  // halt the CPU indefinitely.
  terminal_write_line("El sistema se ha detenido. Puede apagar el equipo.");
  while (1) {
    __asm__ volatile ("cli; hlt");
  }
}

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
    terminal_write_line("Comandos: play sokoban, help, clear, exit");
    goto internal;
  }

  if (!my_strcmp(cmd, "clear")) {
    terminal_clear();
    goto internal;
  }

  if (!my_strcmp(cmd, "exit") || !my_strcmp(cmd, "shutdown")) {
    power_off();
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