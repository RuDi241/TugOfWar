#include "../include/config.h"
#include <stdio.h>
#include <string.h>

#define MAX_CONFIG_DESCRIPTION_LEN 50

void fprintf_config(FILE *stream, const Config *config) {
  fprintf(stream, "end_time %ld\n", config->end_time);
  fprintf(stream, "players_per_team %d\n", config->players_per_team);
  fprintf(stream, "energy_decay_min %f\n", config->energy_decay_range.first);
  fprintf(stream, "energy_decay_max %f\n", config->energy_decay_range.second);
  fprintf(stream, "initial_energy_min %f\n",
          config->initial_energy_range.first);
  fprintf(stream, "initial_energy_max %f\n",
          config->initial_energy_range.second);
  fprintf(stream, "fall_probability %f\n", config->fall_probability);
}

int create_default_config(const char *filename) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    perror("Error creating file");
    return 1;
  }
  Config default_config = {.end_time = 100,
                           .players_per_team = 4,
                           .energy_decay_range = {0, 5},
                           .initial_energy_range = {5, 50},
                           .fall_probability = 0.5};
  fprintf_config(file, &default_config);
  fclose(file);
  return 0;
}

int read_config(const char *filename, Config *config) {
  FILE *file = fopen(filename, "r");
  // if no config, write default config
  if (file == NULL) {
    int status = create_default_config(filename);
    if (status)
      return status;
  }
  long l;
  fscanf(file, "end_time %ld\n", &l);
  config->end_time = l + time(NULL);
  fscanf(file, "players_per_team %d\n", &(config->players_per_team));
  fscanf(file, "energy_decay_min %f\n", &(config->energy_decay_range.first));
  fscanf(file, "energy_decay_max %f\n", &(config->energy_decay_range.second));
  fscanf(file, "initial_energy_min %f\n",
         &(config->initial_energy_range.first));
  fscanf(file, "initial_energy_max %f\n",
         &(config->initial_energy_range.second));
  fscanf(file, "fall_probability %f\n", &(config->fall_probability));
  fclose(file);
  return 0;
}
