#include "../include/error_codes.h"
#include "../include/game_config.h"
#include "../include/team_config.h"

int main(int argc, char *argv[]) {

  GameConfig game_config;
  TeamConfig team1_config;
  TeamConfig team2_config;
  if (read_game_config("./game_config.txt", &game_config))
    return CONFIG_ERROR;
  if (read_team_config("./team1_config.txt", &team1_config))
    return CONFIG_ERROR;
  if (read_team_config("./team2_config.txt", &team2_config))
    return CONFIG_ERROR;
  fprintf_game_config(stdout, &game_config);
  fprintf_team_config(stdout, &team1_config);
  fprintf_team_config(stdout, &team2_config);
  return 0;
}
