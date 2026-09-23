#include "terminal.h"
#include "memory.h"

// Reservamos 1 MiB de Heap en la sección .bss de tu Kernel
#define HEAP_SIZE (1024 * 1024) 
static uint8_t heap_memory[HEAP_SIZE];

typedef struct BlockHeader {
    size_t size;               // Tamaño útil del bloque
    int is_free;               // 1 si está libre, 0 si está ocupado
    struct BlockHeader *next;  // Puntero al siguiente bloque
} BlockHeader;

static BlockHeader *free_list = NULL;

void kmalloc_init(void) {
    free_list = (BlockHeader*) heap_memory;
    free_list->size = HEAP_SIZE - sizeof(BlockHeader);
    free_list->is_free = 1;
    free_list->next = NULL;
}

void* kmalloc(size_t size) {
    if (size == 0) return NULL;

    // Alineación a 8 bytes (múltiplo de 8 para x86_64)
    size = (size + 7) & ~((size_t)7);

    BlockHeader *current = free_list;

    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            // Dividir bloque si el espacio sobrante justifica un nuevo header
            if (current->size >= size + sizeof(BlockHeader) + 16) {
                BlockHeader *next_block = (BlockHeader*)((uint8_t*)current + sizeof(BlockHeader) + size);
                next_block->size = current->size - size - sizeof(BlockHeader);
                next_block->is_free = 1;
                next_block->next = current->next;

                current->size = size;
                current->next = next_block;
            }

            current->is_free = 0;
            return (void*)(current + 1);
        }
        current = current->next;
    }

    return NULL; // Sin memoria libre suficiente
}

void kfree(void *ptr) {
    if (!ptr) return;

    // Recuperar la cabecera retrocediendo en memoria
    BlockHeader *header = (BlockHeader*)ptr - 1;
    header->is_free = 1;

    // Fusión de bloques libres consecutivos
    BlockHeader *current = free_list;
    while (current != NULL && current->next != NULL) {
        if (current->is_free && current->next->is_free) {
            current->size += sizeof(BlockHeader) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

void system_halt(void) {
    // Deshabilitar interrupciones para que no despierten a la CPU
    __asm__ volatile ("cli");
    
    // Bucle infinito con hlt
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void panik(const char* msg){
    terminal_clear();
    terminal_set_color(COLOR_RED);
    terminal_write("KERNEL PANIK!!\n");
    terminal_write(msg);
    system_halt();

}