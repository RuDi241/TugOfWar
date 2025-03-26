#ifndef TEAM_H
#define TEAM_H
#include "../include/player.h"
#include "../include/string_buf.h"
#include "../include/team_config.h"

typedef struct Team {
  int size;
  Player *players;
} Team;


//to arrange players
typedef struct PlayerEnergyIndexed { 
  int index; // Original position
  int energy;   // value of the energy
} PlayerEnergyIndexed;

int make_team(Team *team, TeamConfig *team_config);
int create_team_processes(  pid_t *team_pid, int (*to_team_fd)[2], int (*to_referee_team_fd)[2], TeamConfig *team_config);

int receive_data_from_team(pid_t *team_pid, int (*to_referee_team_fd)[2], Team *team);
int send_position_to_team( pid_t *team_pid, int (*to_team_fd)[2], Team *team);

int destroy_team(Team *team);
int serialize_team(Team *team, StringBuf *string_buf);
int deserialize_team(Team *team, StringBuf *string_buf);
int arrange_team(Team *team);
int compare_by_energy(const void *a, const void *b);
#endif
