#include "game_config.h"
#include "team_config.h"
#include "error_codes.h"
#include "team.h"
#include "game_state.h"
#include "ipc.h"
#include "player.h"
#include <sys/time.h>
#include <signal.h>


int initialize_configs(GameConfig* , TeamConfig* , TeamConfig*);
void run_simulation(int (**to_t1_fd)[2], int (**to_referee_t1_fd)[2], int (**to_t2_fd)[2], int (**to_referee_t2_fd)[2]);
void announce_winner(Team *team, int (*to_team_fd)[2]);
void announce_loser(Team *team, int (*to_team_fd)[2]);
