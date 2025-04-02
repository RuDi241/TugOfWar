#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "../include/game_config.h"
#include "../include/team.h"
#include "../include/team_config.h"
#include "pair_int_int.h"
#include "string_buf.h"
#include <sys/time.h>

//ALL IN milliseconds
typedef struct GameState {
  char in_round;
  
  struct timeval start_round_time;
  time_t current_round_time;

  struct timeval start_simulation_time;
  time_t current_simulation_time; 
  time_t max_simulation_time;

  int number_of_rounds_played;
  int max_number_of_rounds;

  int round_score;
  PairIntInt simulation_score;

  Team team1;
  Team team2;

  int team1_sum;
  int team2_sum;

} GameState;

int init_game_state(GameState *game_state, GameConfig *game_config,
                    TeamConfig *team1_config, TeamConfig *team2_config);
int serialize_game_state(GameState *game_state, StringBuf *string_buf);
int deserialize_game_state(GameState *game_state, StringBuf *string_buf);
#endif
