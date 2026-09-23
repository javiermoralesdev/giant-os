# Tools
ASM = nasm
CC = gcc
LD = ld
GRUB = grub-mkrescue
QEMU = qemu-system-x86_64

# Directories
KERNEL_DIR = kernel
ISO_DIR = iso

# Files
ASM_SRC = $(KERNEL_DIR)/entry.s
ASM_OBJ = $(KERNEL_DIR)/entry.o

C_SRC = $(KERNEL_DIR)/kernel.c
C_OBJ = $(KERNEL_DIR)/kernel.o
TERMINAL_SRC = $(KERNEL_DIR)/terminal.c
TERMINAL_OBJ = $(KERNEL_DIR)/terminal.o
KEYBOARD_SRC = $(KERNEL_DIR)/keyboard.c
KEYBOARD_OBJ = $(KERNEL_DIR)/keyboard.o
LEVELS_SRC = $(KERNEL_DIR)/levels.c
LEVELS_OBJ = $(KERNEL_DIR)/levels.o
GAME_SRC = $(KERNEL_DIR)/game.c
GAME_OBJ = $(KERNEL_DIR)/game.o
TOOLBOX_SRC = $(KERNEL_DIR)/toolbox.c
TOOLBOX_OBJ = $(KERNEL_DIR)/toolbox.o
MEMORY_SRC = $(KERNEL_DIR)/memory.c
MEMORY_OBJ = $(KERNEL_DIR)/memory.o

KERNEL_BIN = kernel.bin
ISO_IMAGE = giant-os.iso

# Compiler and linker flags
CFLAGS = -m64 \
         -ffreestanding \
         -fno-pie \
         -fno-stack-protector \
         -mno-red-zone \
         -mno-mmx \
         -mno-sse \
         -mno-sse2 \
		 -Werror

LDFLAGS = -m elf_x86_64 \
          -T linker.ld

# Default target
all: $(ISO_IMAGE)

# Assemble the boot code
$(ASM_OBJ): $(ASM_SRC)
	$(ASM) -f elf64 $< -o $@

# Compile the C kernel
$(C_OBJ): $(C_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(TERMINAL_OBJ): $(TERMINAL_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(KEYBOARD_OBJ): $(KEYBOARD_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(LEVELS_OBJ): $(LEVELS_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(GAME_OBJ): $(GAME_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(TOOLBOX_OBJ): $(TOOLBOX_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(MEMORY_OBJ): $(MEMORY_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Link the kernel
$(KERNEL_BIN): $(ASM_OBJ) $(C_OBJ) $(TERMINAL_OBJ) $(KEYBOARD_OBJ) $(LEVELS_OBJ) $(GAME_OBJ) $(TOOLBOX_OBJ) $(MEMORY_OBJ) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(ASM_OBJ) $(C_OBJ) $(TERMINAL_OBJ) $(KEYBOARD_OBJ) $(LEVELS_OBJ) $(GAME_OBJ) $(TOOLBOX_OBJ) $(MEMORY_OBJ)

# Copy the kernel and create the ISO
$(ISO_IMAGE): $(KERNEL_BIN)
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/kernel.bin
	$(GRUB) -o $@ $(ISO_DIR)

# Run the operating system
run: $(ISO_IMAGE)
	$(QEMU) -cdrom $(ISO_IMAGE)

# Remove generated files
clean:
	rm -f $(ASM_OBJ)
	rm -f $(C_OBJ)
	rm -f $(TERMINAL_OBJ)
	rm -f $(KEYBOARD_OBJ)
	rm -f $(LEVELS_OBJ)
	rm -f $(GAME_OBJ)
	rm -f $(TOOLBOX_OBJ)
	rm -f $(MEMORY_OBJ)
	rm -f $(KERNEL_BIN)
	rm -f iso/boot/$(KERNEL_BIN)
	rm -f $(ISO_IMAGE)

.PHONY: all run clean