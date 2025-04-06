#include "game_interface_communication.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

int init_graphics_process(GraphicsCommunicationInfo *graphicsCommunicationInfo,
                          Display *display, GameState *game_state) {
  graphicsCommunicationInfo->pid = -1;
  graphicsCommunicationInfo->read_write_fd[0] = -1;
  graphicsCommunicationInfo->read_write_fd[1] = -1;

  display->in_round = game_state->in_round;
  display->in_simulation = game_state->in_simulation;
  display->current_simulation_time =
      game_state->current_simulation_time / 1000; // Convert to seconds
  display->max_simulation_time =
      game_state->max_simulation_time / 1000; // Convert to seconds
  display->current_round_time =
      game_state->current_round_time / 1000; // Convert to seconds
  display->number_of_rounds_played = game_state->number_of_rounds_played;
  display->max_number_of_rounds = game_state->max_number_of_rounds;
  display->round_score = game_state->round_score;
  display->simulation_score = game_state->simulation_score;
  display->team1 = game_state->team1;
  display->team2 = game_state->team2;
  display->team1_sum = game_state->team1_sum;
  display->team2_sum = game_state->team2_sum;
  display->max_consecutive_wins = game_state->max_consecutive_wins;
  display->previous_round_result = game_state->previous_round_result;
  display->current_win_streak = game_state->current_win_streak;
  display->simulation_winning_method = game_state->simulation_winning_method;
  display->simulation_winner = game_state->simulation_winner;
  display->score_gap_to_win = game_state->score_gap_to_win;

  return create_graphics_process(graphicsCommunicationInfo);
}

int create_graphics_process(
    GraphicsCommunicationInfo *graphicsCommunicationInfo) {

  create_graphics_pipe(graphicsCommunicationInfo);

  graphicsCommunicationInfo->pid = fork();
  if (graphicsCommunicationInfo->pid < 0) {
    perror("fork failed");
    return FORK_ERROR;
  } else if (graphicsCommunicationInfo->pid == 0) { // Child process
    // Close unused pipe ends
    close(graphicsCommunicationInfo->read_write_fd[1]); // graphics won't write
    // exec the graphics process here
    char read_fd[10];
    sprintf(read_fd, "%d", graphicsCommunicationInfo->read_write_fd[0]);
    execlp("./bin/graphics", "./graphics", read_fd, NULL);
    perror("exec failed");
    return EXEC_ERROR;
  }
  return 0;
}

int create_graphics_pipe(GraphicsCommunicationInfo *graphicsCommunicationInfo) {
  if (pipe(graphicsCommunicationInfo->read_write_fd) == -1) {
    perror("graphics pipe failed");
    return PIPE_ERROR;
  }
  return 0;
}

int update_screen(GameState *game_state, int simulation_winning_method,
                  int simulation_winner,
                  GraphicsCommunicationInfo *graphicsCommunicationInfo,
                  Display *display) {

  // Update the screen with the current game state
  display->in_round = game_state->in_round;
  display->in_simulation = game_state->in_simulation;
  display->current_simulation_time =
      game_state->current_simulation_time / 1000; // Convert to seconds
  display->max_simulation_time = game_state->max_simulation_time; // seconds
  display->current_round_time =
      game_state->current_round_time / 1000; // Convert to seconds
  display->number_of_rounds_played = game_state->number_of_rounds_played;
  display->max_number_of_rounds = game_state->max_number_of_rounds;
  display->round_score = game_state->round_score;
  display->simulation_score = game_state->simulation_score;
  display->team1 = game_state->team1;
  display->team2 = game_state->team2;
  display->team1_sum = game_state->team1_sum;
  display->team2_sum = game_state->team2_sum;
  display->max_consecutive_wins = game_state->max_consecutive_wins;
  display->previous_round_result = game_state->previous_round_result;
  display->current_win_streak = game_state->current_win_streak;
  display->simulation_winning_method = simulation_winning_method;
  display->simulation_winner = simulation_winner;
  display->score_gap_to_win = game_state->score_gap_to_win;

  // Here you would typically send this data to the display process
  send_data_to_display(display, graphicsCommunicationInfo);

  return 0;
}

int send_data_to_display(Display *display,
                         GraphicsCommunicationInfo *graphicsCommunicationInfo) {

  int fd = graphicsCommunicationInfo->read_write_fd[1];

  if (write_all(fd, &display->in_round, sizeof(display->in_round)) < 0)
    return WRITE_PIPE_ERROR;
  if (write_all(fd, &display->in_simulation, sizeof(display->in_simulation)) <
      0)
    return WRITE_PIPE_ERROR;
  if (write_all(fd, &display->current_simulation_time,
                sizeof(display->current_simulation_time)) < 0)
    return WRITE_PIPE_ERROR;
  if (write_all(fd, &display->max_simulation_time,
                sizeof(display->max_simulation_time)) < 0)
    return WRITE_PIPE_ERROR;
  if (write_all(fd, &display->current_round_time,
                sizeof(display->current_round_time)) < 0)
    return WRITE_PIPE_ERROR;
  if (write_all(fd, &display->number_of_rounds_played,
                sizeof(display->number_of_rounds_played)) < 0)
    return WRITE_PIPE_ERROR;
  if (write_all(fd, &display->max_number_of_rounds,
                sizeof(display->max_number_of_rounds)) < 0)
    return WRITE_PIPE_ERROR;
  if (write_all(fd, &display->round_score, sizeof(display->round_score)) < 0)
    return WRITE_PIPE_ERROR;
  if (write_all(fd, &display->simulation_score,
                sizeof(display->simulation_score)) < 0)
    return WRITE_PIPE_ERROR;

  // Send team1
  if (write_all(fd, &display->team1.size, sizeof(display->team1.size)) < 0)
    return WRITE_PIPE_ERROR;
  for (int i = 0; i < display->team1.size; i++) {
    Player *p = &display->team1.players[i];
    if (write_all(fd, p, sizeof(Player)) < 0)
      return WRITE_PIPE_ERROR;
  }

  // Send team2
  if (write_all(fd, &display->team2.size, sizeof(display->team2.size)) < 0)
    return WRITE_PIPE_ERROR;
  for (int i = 0; i < display->team2.size; i++) {
    Player *p = &display->team2.players[i];
    if (write_all(fd, p, sizeof(Player)) < 0)
      return WRITE_PIPE_ERROR;
  }

  if (write_all(fd, &display->team1_sum, sizeof(display->team1_sum)) < 0)
    return WRITE_PIPE_ERROR;
  if (write_all(fd, &display->team2_sum, sizeof(display->team2_sum)) < 0)
    return WRITE_PIPE_ERROR;
  if (write_all(fd, &display->max_consecutive_wins,
                sizeof(display->max_consecutive_wins)) < 0)
    return WRITE_PIPE_ERROR;
  if (write_all(fd, &display->previous_round_result,
                sizeof(display->previous_round_result)) < 0)
    return WRITE_PIPE_ERROR;
  if (write_all(fd, &display->current_win_streak,
                sizeof(display->current_win_streak)) < 0)
    return WRITE_PIPE_ERROR;
  if (write_all(fd, &display->simulation_winning_method,
                sizeof(display->simulation_winning_method)) < 0)
    return WRITE_PIPE_ERROR;
  if (write_all(fd, &display->simulation_winner,
                sizeof(display->simulation_winner)) < 0)
    return WRITE_PIPE_ERROR;
  if (write_all(fd, &display->score_gap_to_win,
                sizeof(display->score_gap_to_win)) < 0)
    return WRITE_PIPE_ERROR;

  if (kill(graphicsCommunicationInfo->pid, SIGUSR1) < 0) {
    perror("Failed to send signal to graphics process");
    return SIGNAL_ERROR;
  }
  return 0;
}

ssize_t write_all(int fd, const void *buffer, size_t count) {
  size_t bytes_written = 0;
  const char *buf = (const char *)buffer;
  while (bytes_written < count) {
    ssize_t result = write(fd, buf + bytes_written, count - bytes_written);
    if (result < 0) {
      return -1;
    }
    bytes_written += result;
  }
  return bytes_written;
}

int destroy_graphics_process(
    GraphicsCommunicationInfo *graphicsCommunicationInfo) {
  // add signal handler to kill all players

  int status;
  if (waitpid(graphicsCommunicationInfo->pid, &status, WNOHANG) == -1) {
    perror("Error waiting for Graphics to pause and terminate");
    return WAIT_ERROR;
  }

  if (WIFSTOPPED(status)) { // The child is paused
    if (kill(graphicsCommunicationInfo->pid, SIGTERM) == -1) {
      perror("Failed to Graphics player");
      return SIGNAL_ERROR;
    }
  }

  close(graphicsCommunicationInfo->read_write_fd[0]);
  close(graphicsCommunicationInfo->read_write_fd[1]);

  return 0;
}
