#include "../include/team_config.h"
#include "../include/error_codes.h"
#include "../include/pair_int_int.h"
#include <stdio.h>
#include <stdlib.h>

#define CONFIG_LINE_SIZE 256

const TeamConfig DEFAULT_TEAM_CONFIG = {
    .num_players = 4,
    .initial_energy_range = {100, 200},
    .energy_decay_range = {1, 10},
    .fall_probability = 0.1,
    .fall_time_range = {10, 20},
};

int validate_positive_range(PairIntInt range) {
  return range.first < 0 || range.second < 0 || range.first > range.second;
}
int parse_team_config(char *buf, TeamConfig *team_config) {
  char dummy[CONFIG_LINE_SIZE];

  // Skip blank lines
  if (sscanf(buf, "%s", dummy) == EOF)
    return 0;
  // Handle comments
  if (sscanf(buf, "%[#]", dummy) == 1)
    return 0;

  // Parse number of players
  if (sscanf(buf, "num_players = %d", &team_config->num_players) == 1) {
    if (team_config->num_players <= 0) {
      fprintf(stderr, "Invalid number of players\n");
      return CONFIG_ERROR;
    }
    return 0;
  }

  // Parse energy level range
  if (sscanf(buf, "initial_energy_range = %d %d",
             &team_config->initial_energy_range.first,
             &team_config->initial_energy_range.second) == 2) {
    if (validate_positive_range(team_config->initial_energy_range)) {
      fprintf(stderr, "Invalid initial energy range. Both numbers must be "
                      "positive integers, and min <= max.\n");
      return CONFIG_ERROR;
    }
    return 0;
  }

  // Parse energy decay range
  if (sscanf(buf, "energy_decay_range = %d %d",
             &team_config->energy_decay_range.first,
             &team_config->energy_decay_range.second) == 2) {
    if (validate_positive_range(team_config->energy_decay_range)) {
      fprintf(stderr, "Invalid energy decay range. Both numbers must be "
                      "positive integers, and min <= max.\n");
      return CONFIG_ERROR;
    }
    return 0;
  }

  // Parse fall probability
  if (sscanf(buf, "fall_probability = %lf", &team_config->fall_probability) ==
      1) {
    if (team_config->fall_probability < 0 ||
        team_config->fall_probability > 1) {
      fprintf(stderr,
              "Invalid fall probability. Must be a number between 0 and 1.\n");
      return CONFIG_ERROR;
    }
    return 0;
  }
  if (sscanf(buf, "fall_time_range = %d %d",
             &team_config->fall_time_range.first,
             &team_config->fall_time_range.second) == 2) {
    if (validate_positive_range(team_config->fall_time_range)) {
      fprintf(stderr, "Invalid fall time range. Both numbers must be "
                      "positive integers, and min <= max.\n");
      return CONFIG_ERROR;
    }
    return 0;
  }
  fprintf(stderr, "Invalid line in config file\n");
  return CONFIG_ERROR;
}

int read_team_config(const char *filename, TeamConfig *team_config) {
  // initialize with default values
  *team_config = DEFAULT_TEAM_CONFIG;
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    // if no config, write default config
    return write_team_config(filename, team_config);
  }
  char buf[CONFIG_LINE_SIZE];
  int line = 0;
  int config_status = 0;
  while (fgets(buf, CONFIG_LINE_SIZE, file)) {
    int status = parse_team_config(buf, team_config);
    if (status) {
      config_status = CONFIG_ERROR;
      fprintf(stderr, "Error parsing team config file %s at line %d\n",
              filename, line);
    }
    ++line;
  }
  fclose(file);
  return config_status;
}
int write_team_config(const char *filename, const TeamConfig *team_config) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    fprintf(stderr, "Unable to create a new team config file at %s \n",
            filename);
    return IO_ERROR;
  }
  int status = fprintf_team_config(file, team_config);
  fflush(file);
  fclose(file);
  return status;
}
int fprintf_team_config(FILE *stream, const TeamConfig *team_config) {
  if (stream == NULL)
    return IO_ERROR;
  fprintf(stream, "num_players = %d\n", team_config->num_players);
  fprintf(stream, "initial_energy_range = %d %d\n",
          team_config->initial_energy_range.first,
          team_config->initial_energy_range.second);
  fprintf(stream, "energy_decay_range = %d %d\n",
          team_config->energy_decay_range.first,
          team_config->energy_decay_range.second);
  fprintf(stream, "fall_probability = %lf\n", team_config->fall_probability);
  fprintf(stream, "fall_time_range = %d %d\n",
          team_config->fall_time_range.first,
          team_config->fall_time_range.second);
  fflush(stream);
  return 0;
}
