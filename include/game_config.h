#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H
#include <stdio.h>
#include <time.h>

typedef struct GameConfig {
  time_t max_simulation_time;
  int max_number_of_rounds;
  int score_gap_to_win;
  int max_consecutive_wins;
} GameConfig;

int read_game_config(const char *filename, GameConfig *game_config);
int write_game_config(const char *filename, const GameConfig *game_config);
int fprintf_game_config(FILE *stream, const GameConfig *game_config);
#endif
