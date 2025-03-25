#include "../include/error_codes.h"
#include "../include/game_config.h"
#include "../include/team_config.h"
#include "../include/team.h"
#include "../include/game_state.h"
#include "../include/ipc.h"


int main(int argc, char *argv[]) {

  GameConfig game_config;
  GameState game_state;
  TeamConfig team1_config;
  TeamConfig team2_config;
  Team team1;
  Team team2;  

  pid_t *t1_pid;
  int (*to_t1_fd)[2];
  int (*to_referee_t1_fd)[2];

  pid_t *t2_pid;
  int (*to_t2_fd)[2];
  int (*to_referee_t2_fd)[2];
  pid_t *pid;

  if (read_game_config("./game_config.txt", &game_config))
    return CONFIG_ERROR;
  if (read_team_config("./team1_config.txt", &team1_config))
    return CONFIG_ERROR;
  if (read_team_config("./team2_config.txt", &team2_config))
    return CONFIG_ERROR;
  fprintf_game_config(stdout, &game_config);
  fprintf_team_config(stdout, &team1_config);
  fprintf_team_config(stdout, &team2_config);

  //create team1,2
  make_team(&team1, &team1_config);  
  make_team(&team2, &team2_config);  

  //init game state
  init_game_state(&game_state, &game_config, &team1, &team2);

  init_interprocess_communication(t1_pid, to_t1_fd, to_referee_t1_fd, t2_pid, to_t2_fd, to_referee_t2_fd, &team1, &team2);

  create_pipes(to_t1_fd, to_referee_t1_fd, to_t2_fd, to_referee_t2_fd, &team1, &team2);

  //create team1's processes
  create_team_processes(t1_pid, to_t1_fd, to_referee_t1_fd, &team1, '1');

  //create team2's processes
  create_team_processes(t2_pid, to_t2_fd, to_referee_t2_fd, &team2, '2');


  //start a round
  


  return 0;
}
