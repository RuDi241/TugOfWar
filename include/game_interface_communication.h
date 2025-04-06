#ifndef GAME_INTERFACE_COMMUNICATION_H
#define GAME_INTERFACE_COMMUNICATION_H

#include <stdlib.h>
#include <string.h>
#include "game_state.h"

#include "error_codes.h"

enum SimulationWinningMethod {
  CONSECUTIVE_WINS,
  WON_MORE_ROUNDS,
  STILL_PLAYING
};

enum SimulationWinner { SIM_RES_DRAW, SIM_RES_TEAM_1, SIM_RES_TEAM_2 };

typedef struct GraphicsCommunicationInfo {
  pid_t pid;
  int read_write_fd[2];
} GraphicsCommunicationInfo;

typedef struct Display {
  char in_round;
  char in_simulation;

  time_t current_simulation_time;
  time_t max_simulation_time;

  time_t current_round_time;

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
  int current_win_streak;

  enum SimulationWinningMethod simulation_winning_method;
  enum SimulationWinner simulation_winner;
} Display;
int init_graphics_process(GraphicsCommunicationInfo *graphicsCommunicationInfo,
                          Display *display, GameState *game_state);
int create_graphics_process(
    GraphicsCommunicationInfo *graphicsCommunicationInfo);
int create_graphics_pipe(GraphicsCommunicationInfo *graphicsCommunicationInfo);
int update_screen(GameState *game_state, int simulation_winning_method,
                  int simulation_winner,
                  GraphicsCommunicationInfo *graphicsCommunicationInfo,
                  Display *display);
int send_data_to_display(Display *display,
                         GraphicsCommunicationInfo *graphicsCommunicationInfo);
ssize_t write_all(int fd, const void *buffer, size_t count);
void receive_data_from_referee(int fd, Display *disp);
void destroy_display(Display *disp);
ssize_t read_all(int fd, void *buffer, size_t count);
int destroy_graphics_process(
    GraphicsCommunicationInfo *graphicsCommunicationInfo);

void printf_display(Display *disp);
#endif
