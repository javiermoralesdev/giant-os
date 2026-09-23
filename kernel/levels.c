#include "levels.h"

static const Level levels[LEVEL_COUNT] =
{
    // Nivel 1: Empuje básico
    {
        .rows =
        {
            "######",
            "#    #",
            "# .  #",
            "#    #",
            "######"
        },
        .player_start_row = 3,
        .player_start_column = 3,
        .box_count = 1,
        .box_starts = { { 2, 3 } },
        .width = 6,
        .height = 5
    },

    // Nivel 2: Empuje en L
    {
        .rows =
        {
            "######",
            "# .  #",
            "#    #",
            "#    #",
            "######"
        },
        .player_start_row = 3,
        .player_start_column = 3,
        .box_count = 1,
        .box_starts = { { 2, 3 } },
        .width = 6,
        .height = 5
    },

    // Nivel 3: Dos cajas rectas
    {
        .rows =
        {
            "#######",
            "# . . #",
            "#     #",
            "#     #",
            "#######"
        },
        .player_start_row = 3,
        .player_start_column = 3,
        .box_count = 2,
        .box_starts = { { 2, 2 }, { 2, 4 } },
        .width = 7,
        .height = 5
    },

    // Nivel 4: Cajas hacia los lados
    {
        .rows =
        {
            "#######",
            "#     #",
            "#.   .#",
            "#     #",
            "#######"
        },
        .player_start_row = 1,
        .player_start_column = 3,
        .box_count = 2,
        .box_starts = { { 2, 2 }, { 2, 4 } },
        .width = 7,
        .height = 5
    },

    // Nivel 5: Sorteando el centro
    {
        .rows =
        {
            "#######",
            "#.   .#",
            "# ### #",
            "#     #",
            "#     #",
            "#######"
        },
        .player_start_row = 4,
        .player_start_column = 3,
        .box_count = 2,
        .box_starts = { { 3, 1 }, { 3, 5 } },
        .width = 7,
        .height = 6
    },

    // Nivel 6: Maniobra en U
    {
        .rows =
        {
            "########",
            "#.    .#",
            "#      #",
            "#      #",
            "########"
        },
        .player_start_row = 3,
        .player_start_column = 3,
        .box_count = 2,
        .box_starts = { { 2, 2 }, { 2, 5 } },
        .width = 8,
        .height = 5
    },

    // Nivel 7: Tres en columna
    {
        .rows =
        {
            "#######",
            "# .   #",
            "# .   #",
            "# .   #",
            "#     #",
            "#######"
        },
        .player_start_row = 4,
        .player_start_column = 5,
        .box_count = 3,
        .box_starts = { { 1, 4 }, { 2, 4 }, { 3, 4 } },
        .width = 7,
        .height = 6
    },

    // Nivel 8: El embudo
    {
        .rows =
        {
            "########",
            "###..###",
            "#      #",
            "#      #",
            "########"
        },
        .player_start_row = 3,
        .player_start_column = 3,
        .box_count = 2,
        .box_starts = { { 2, 3 }, { 2, 4 } },
        .width = 8,
        .height = 5
    },

    // Nivel 9: Pasaje central
    {
        .rows =
        {
            "#########",
            "### . ###",
            "###   ###",
            "#       #",
            "#########"
        },
        .player_start_row = 3,
        .player_start_column = 4,
        .box_count = 1,
        .box_starts = { { 2, 4 } },
        .width = 9,
        .height = 5
    },

    // Nivel 10: Separación
    {
        .rows =
        {
            "########",
            "# .  . #",
            "#      #",
            "#      #",
            "########"
        },
        .player_start_row = 3,
        .player_start_column = 3,
        .box_count = 2,
        .box_starts = { { 2, 2 }, { 2, 5 } },
        .width = 8,
        .height = 5
    },

    // Nivel 11: Espaciado doble
    {
        .rows =
        {
            "##########",
            "# .    . #",
            "#        #",
            "#        #",
            "##########"
        },
        .player_start_row = 3,
        .player_start_column = 4,
        .box_count = 2,
        .box_starts = { { 2, 3 }, { 2, 6 } },
        .width = 10,
        .height = 5
    },

    // Nivel 12: Cuatro cajas, empuje directo
    {
        .rows =
        {
            "########",
            "# .  . #",
            "#      #",
            "# .  . #",
            "#      #",
            "#      #",
            "########"
        },
        .player_start_row = 5,
        .player_start_column = 3,
        .box_count = 4,
        .box_starts = { { 2, 2 }, { 2, 5 }, { 4, 2 }, { 4, 5 } },
        .width = 8,
        .height = 7
    },

    // Nivel 13: Desplazamiento cardinal
    {
        .rows =
        {
            "########",
            "# .  . #",
            "#      #",
            "#      #",
            "#      #",
            "# .  . #",
            "########"
        },
        .player_start_row = 3,
        .player_start_column = 3,
        .box_count = 4,
        .box_starts = { { 2, 2 }, { 2, 5 }, { 4, 2 }, { 4, 5 } },
        .width = 8,
        .height = 7
    },

    // Nivel 14: Gran sala de cuatro
    {
        .rows =
        {
            "##########",
            "#        #",
            "# .    . #",
            "#        #",
            "# .    . #",
            "#        #",
            "#        #",
            "##########"
        },
        .player_start_row = 6,
        .player_start_column = 4,
        .box_count = 4,
        .box_starts = { { 3, 2 }, { 3, 7 }, { 5, 2 }, { 5, 7 } },
        .width = 10,
        .height = 8
    },

    // Nivel 15: Empuje en diagonal amplio
    {
        .rows =
        {
            "############",
            "#          #",
            "# .      . #",
            "#          #",
            "#          #",
            "############"
        },
        .player_start_row = 4,
        .player_start_column = 5,
        .box_count = 2,
        .box_starts = { { 3, 3 }, { 3, 8 } },
        .width = 12,
        .height = 6
    },

    // Nivel 16: El almacén lineal (6 cajas)
    {
        .rows =
        {
            "################",
            "#              #",
            "# . . .  . . . #",
            "#              #",
            "#              #",
            "#              #",
            "################"
        },
        .player_start_row = 5,
        .player_start_column = 7,
        .box_count = 6,
        .box_starts = { { 4, 2 }, { 4, 4 }, { 4, 6 }, { 4, 9 }, { 4, 11 }, { 4, 13 } },
        .width = 16,
        .height = 7
    }
};

const Level *levels_get(int level_index)
{
    if (level_index < 0 || level_index >= LEVEL_COUNT)
        return 0;

    return &levels[level_index];
}