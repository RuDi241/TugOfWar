#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "../include/game_config.h"
#include "../include/team.h"
#include "../include/team_config.h"
#include "pair_int_int.h"
#include "string_buf.h"
#include <sys/time.h>

typedef struct GameState {
  char in_round;
  //time_t round_start_time;
  time_t simulation_time;
  time_t round_period;
  int number_of_rounds;
  int round_score;
  PairIntInt simulation_score;
  //time_t simulation_start_time; 
  Team team1;
  Team team2;
} GameState;

int init_game_state(GameState *game_state, GameConfig *game_config,
                    //TeamConfig *team1_config, TeamConfig *team2_config
                    Team *team1, Team *team2);
int serialize_game_state(GameState *game_state, StringBuf *string_buf);
int deserialize_game_state(GameState *game_state, StringBuf *string_buf);
#endif
