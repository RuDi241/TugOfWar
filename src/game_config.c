#include "../include/game_config.h"
#include "../include/error_codes.h"
#include <stdio.h>
#include <string.h>

#define CONFIG_LINE_SIZE 256

int parse_game_config(char *buf, GameConfig *game_config) {
  char dummy[CONFIG_LINE_SIZE];
  // Skip blank lines
  if (sscanf(buf, "%s", dummy) == EOF)
    return 0;
  // Handle comments
  if (sscanf(buf, "%[#]", dummy) == 1)
    return 0;

  if (sscanf(buf, " max_simulation_time = %ld",
             &game_config->max_simulation_time) == 1) {
    if (game_config->max_simulation_time <= 0)
      return CONFIG_ERROR;
    return 0;
  }
  fprintf(stderr, "Invalid line in config file\n");
  return CONFIG_ERROR;
}
const GameConfig DEFAULT_CONFIG = {
    .max_simulation_time = 100,
};
int fprintf_game_config(FILE *stream, const GameConfig *game_config) {
  if (stream == NULL)
    return IO_ERROR;
  fprintf(stream, "max_simulation_time = %ld\n",
          game_config->max_simulation_time);
  fflush(stream);
  return 0;
}

int write_game_config(const char *filename, const GameConfig *game_config) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    fprintf(stderr, "Unable to create a new game config file at %s \n",
            filename);
    return IO_ERROR;
  }
  int status = fprintf_game_config(file, game_config);
  fflush(file);
  fclose(file);
  return status;
}

int read_game_config(const char *filename, GameConfig *game_config) {
  // initialize game_config
  *game_config = DEFAULT_CONFIG;
  FILE *file = fopen(filename, "r");
  // if no config, write default config
  if (file == NULL) {
    return write_game_config(filename, game_config);
  }
  char buf[CONFIG_LINE_SIZE];
  int line = 0;
  int config_status = 0;
  while (fgets(buf, CONFIG_LINE_SIZE, file)) {
    int status = parse_game_config(buf, game_config);
    if (status) {
      config_status = CONFIG_ERROR;
      fprintf(stderr, "Error parsing game config file %s at line %d\n",
              filename, line);
    }
    ++line;
  }
  fclose(file);
  return config_status;
}
