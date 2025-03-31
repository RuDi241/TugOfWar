#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "../include/game_config.h"
#include "../include/team.h"
#include "../include/team_config.h"
#include "pair_int_int.h"
#include "string_buf.h"
#include <sys/time.h>
#include <stdlib.h>
#define SEED 42

//ALL IN milliseconds
typedef struct GameState {
  char in_round;
  int round_winner;
  int round_loser;
  int simulation_time;
  int number_of_rounds;
  int round_score;
  int round_time;
  int max_consecutive_wins;
  int round_threshold_score;
  PairIntInt simulation_score;
  Team team1;
  Team team2;
} GameState;

int init_game_state(GameState *game_state, GameConfig *game_config,
                    //TeamConfig *team1_config, TeamConfig *team2_config
                    Team *team1, Team *team2);
int serialize_game_state(GameState *game_state, StringBuf *string_buf);
int deserialize_game_state(GameState *game_state, StringBuf *string_buf);
#endif
