#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H
#include <stdio.h>
#include <time.h>

typedef struct GameConfig {
  time_t max_simulation_time;
} GameConfig;

int read_game_config(const char *filename, GameConfig *game_config);
int write_game_config(const char *filename, const GameConfig *game_config);
int fprintf_game_config(FILE *stream, const GameConfig *game_config);
#endif
