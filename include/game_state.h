#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "../include/game_config.h"
#include "../include/team.h"
#include "../include/team_config.h"
#include "pair_int_int.h"
#include "string_buf.h"
#include <sys/time.h>

enum PreviousRoundResult {
  TEAM1_TEAM2_DRAW,
  TEAM1_WIN,
  TEAM2_WIN
};

enum RoundTeamResult {
  DRAW,
  LOSE,
  WIN,
};

typedef struct GameState {
  char in_round;
  char in_simulation;

  struct timeval start_round_time;
  time_t current_round_time;

  struct timeval start_simulation_time;
  time_t current_simulation_time; 
  time_t max_simulation_time;

  int number_of_rounds_played;
  int max_number_of_rounds;

  int round_score;
  PairIntInt simulation_score;
  int score_gap_to_win;

  Team team1;
  Team team2;

  int team1_sum;
  int team2_sum;

  int max_consecutive_wins;
  enum PreviousRoundResult previous_round_result;
  int simulation_winning_method;
  int simulation_winner;
  int current_win_streak;

} GameState;

int init_game_state(GameState *game_state, GameConfig *game_config,
                    TeamConfig *team1_config, TeamConfig *team2_config);
int destroy_game_state(GameState *game_state);
int end_round_protocol(GameState *game_state);
int end_simulation_protocol(GameState *game_state);
int serialize_game_state(GameState *game_state, StringBuf *string_buf);
int deserialize_game_state(GameState *game_state, StringBuf *string_buf);
#endif
