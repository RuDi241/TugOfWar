#ifndef TEAM_CONFIG_H
#define TEAM_CONFIG_H
#include "pair_int_int.h"
#include <stdio.h>

typedef struct TeamConfig {
  int num_players;
  PairIntInt initial_energy_range;
  PairIntInt energy_decay_range;
  double fall_probability;
  PairIntInt fall_time_range;
} TeamConfig;

int read_team_config(const char *filename, TeamConfig *team_config);
int write_team_config(const char *filename, const TeamConfig *team_config);
int fprintf_team_config(FILE *stream, const TeamConfig *team_config);
#endif
