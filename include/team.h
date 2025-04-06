#ifndef TEAM_H
#define TEAM_H
#include "../include/player.h"
#include "../include/team_config.h"

typedef struct Team {
  int size;
  Player *players;
} Team;

int make_team(Team *team, TeamConfig *team_config);

int receive_data_from_team(Team *team);
int send_position_to_team(Team *team);

int destroy_team(Team *team);
int arrange_team(Team *team);
int announce_result_to_team(Team *team, int lastRoundOutcome);
#endif
