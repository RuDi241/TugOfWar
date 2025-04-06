#include "../../include/game_interface_communication.h"
#include <unistd.h>

void receive_data_from_referee(int fd, Display *disp) {
  if (read_all(fd, &disp->in_round, sizeof(disp->in_round)) == -1)
    goto error;
  if (read_all(fd, &disp->in_simulation, sizeof(disp->in_simulation)) == -1)
    goto error;
  if (read_all(fd, &disp->current_simulation_time,
               sizeof(disp->current_simulation_time)) == -1)
    goto error;
  if (read_all(fd, &disp->max_simulation_time,
               sizeof(disp->max_simulation_time)) == -1)
    goto error;
  if (read_all(fd, &disp->current_round_time,
               sizeof(disp->current_round_time)) == -1)
    goto error;
  if (read_all(fd, &disp->number_of_rounds_played,
               sizeof(disp->number_of_rounds_played)) == -1)
    goto error;
  if (read_all(fd, &disp->max_number_of_rounds,
               sizeof(disp->max_number_of_rounds)) == -1)
    goto error;
  if (read_all(fd, &disp->round_score, sizeof(disp->round_score)) == -1)
    goto error;
  if (read_all(fd, &disp->simulation_score, sizeof(disp->simulation_score)) ==
      -1)
    goto error;

  // Receive team1
  if (read_all(fd, &disp->team1.size, sizeof(disp->team1.size)) == -1)
    goto error;
  if (disp->team1.size > 0) {
    if (!disp->team1.players)
      disp->team1.players = malloc(disp->team1.size * sizeof(Player));
    if (!disp->team1.players) {
      perror("malloc team1.players");
      goto error;
    }
    for (int i = 0; i < disp->team1.size; i++) {
      if (read_all(fd, &disp->team1.players[i], sizeof(Player)) == -1)
        goto error;
    }
  } else {
    disp->team1.players = NULL;
  }

  // Receive team2
  if (read_all(fd, &disp->team2.size, sizeof(disp->team2.size)) == -1)
    goto error;
  if (disp->team2.size > 0) {
    if (!disp->team2.players)
      disp->team2.players = malloc(disp->team2.size * sizeof(Player));
    if (!disp->team2.players) {
      perror("malloc team2.players");
      goto error;
    }
    for (int i = 0; i < disp->team2.size; i++) {
      if (read_all(fd, &disp->team2.players[i], sizeof(Player)) == -1)
        goto error;
    }
  } else {
    disp->team2.players = NULL;
  }

  if (read_all(fd, &disp->team1_sum, sizeof(disp->team1_sum)) == -1)
    goto error;
  if (read_all(fd, &disp->team2_sum, sizeof(disp->team2_sum)) == -1)
    goto error;
  if (read_all(fd, &disp->max_consecutive_wins,
               sizeof(disp->max_consecutive_wins)) == -1)
    goto error;
  if (read_all(fd, &disp->previous_round_result,
               sizeof(disp->previous_round_result)) == -1)
    goto error;
  if (read_all(fd, &disp->current_win_streak,
               sizeof(disp->current_win_streak)) == -1)
    goto error;
  if (read_all(fd, &disp->simulation_winning_method,
               sizeof(disp->simulation_winning_method)) == -1)
    goto error;
  if (read_all(fd, &disp->simulation_winner, sizeof(disp->simulation_winner)) ==
      -1)
    goto error;
  if (read_all(fd, &disp->score_gap_to_win, sizeof(disp->score_gap_to_win)) ==
      -1)
    goto error;

  printf("disp->in_round: %c\n", disp->in_round);
  printf("disp->in_simulation: %c\n", disp->in_simulation);
  printf("disp->current_simulation_time: %ld\n", disp->current_simulation_time);
  printf("disp->max_simulation_time: %ld\n", disp->max_simulation_time);
  printf("disp->current_round_time: %ld\n", disp->current_round_time);
  printf("disp->number_of_rounds_played: %d\n", disp->number_of_rounds_played);
  printf("disp->max_number_of_rounds: %d\n", disp->max_number_of_rounds);
  printf("disp->round_score: %d\n", disp->round_score);
  printf("disp->simulation_score: (%d, %d)\n", disp->simulation_score.first,
         disp->simulation_score.second);
  printf("disp->team1.size: %d\n", disp->team1.size);
  for (int i = 0; i < disp->team1.size; i++) {
    printf("disp->team1.players[%d].pid: %d\n", i, disp->team1.players[i].pid);
    printf("disp->team1.players[%d].energy: %d\n", i,
           disp->team1.players[i].energy);
  }
  printf("disp->team2.size: %d\n", disp->team2.size);
  for (int i = 0; i < disp->team2.size; i++) {
    printf("disp->team2.players[%d].pid: %d\n", i, disp->team2.players[i].pid);
    printf("disp->team2.players[%d].energy: %d\n", i,
           disp->team2.players[i].energy);
  }
  printf("disp->team1_sum: %d\n", disp->team1_sum);
  printf("disp->team2_sum: %d\n", disp->team2_sum);
  printf("disp->max_consecutive_wins: %d\n", disp->max_consecutive_wins);
  printf("disp->previous_round_result: %d\n", disp->previous_round_result);
  printf("disp->current_win_streak: %d\n", disp->current_win_streak);
  printf("disp->simulation_winning_method: %d\n",
         disp->simulation_winning_method);
  printf("disp->simulation_winner: %d\n", disp->simulation_winner);
  printf("disp->score_gap_to_win: %d\n", disp->score_gap_to_win);

error:
  perror("receive_data_from_referee: read error");
  // Cleanup allocated memory if any.
  if (disp->team1.players)
    free(disp->team1.players);
  if (disp->team2.players)
    free(disp->team2.players);
}

ssize_t read_all(int fd, void *buffer, size_t count) {
  size_t bytes_read = 0;
  char *buf = (char *)buffer;
  while (bytes_read < count) {
    ssize_t result = read(fd, buf + bytes_read, count - bytes_read);
    if (result < 0) {
      perror("Error reading from pipe");
      return -1;
    }
    if (result == 0)
      break; // EOF
    bytes_read += result;
  }
  return bytes_read;
}

void destroy_display(Display *disp) {
  free(disp->team1.players);
  free(disp->team2.players);
}
