#include "../../include/main.h"

int initialize_configs(GameConfig *game_config, TeamConfig *team1_config, TeamConfig *team2_config) {
    if (read_game_config("./game_config.txt", game_config)) {
      fprintf(stderr, "Error reading game configuration.\n");
      return CONFIG_ERROR;
    }
    if (read_team_config("./team1_config.txt", team1_config)) {
      fprintf(stderr, "Error reading team 1 configuration.\n");
      return CONFIG_ERROR;
    }
    if (read_team_config("./team2_config.txt", team2_config)) {
      fprintf(stderr, "Error reading team 2 configuration.\n");
      return CONFIG_ERROR;
    }
    fprintf_game_config(stdout, game_config);
    fprintf_team_config(stdout, team1_config);
    fprintf_team_config(stdout, team2_config);
    return 0; // Success
  }