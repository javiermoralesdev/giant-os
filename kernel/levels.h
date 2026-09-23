#ifndef LEVELS_H
#define LEVELS_H

#include "terminal.h"
#define MAX_BOXES 8

typedef struct
{
    int row;
    int column;
} Position;

typedef struct
{
    const char *rows[VGA_HEIGHT];

    int player_start_row;
    int player_start_column;

    int box_count;
    Position box_starts[MAX_BOXES];
    int width;
    int height;

} Level;

#define LEVEL_COUNT 16

const Level *levels_get(int level_index);

#endif