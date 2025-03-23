#ifndef CONFIG_H
#define CONFIG_H
#include "../include/pair_float_float.h"
#include "../include/pair_int_int.h"
#include <stdio.h>
#include <time.h>

typedef struct Config {
  int players_per_team;
  PairFloatFloat initial_energy_range;
  PairFloatFloat energy_decay_range;
  float fall_probability;
  time_t end_time;
} Config;

int read_config(const char *filename, Config *config);
int create_default_config(const char *filename);
void fprintf_config(FILE *stream, const Config *config);
#endif
