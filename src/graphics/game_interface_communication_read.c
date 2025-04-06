#include "../../include/game_interface_communication.h"
#include <unistd.h>

int compare_players_by_position(const void *a, const void *b) {
  const Player *player_a = (const Player *)a;
  const Player *player_b = (const Player *)b;

  // Sort by position in descending order
  return player_b->position - player_a->position;
}

void sort_team_by_position(Team *team) {
  if (team == NULL || team->players == NULL || team->size <= 1) {
    return;
  }
  qsort(team->players, team->size, sizeof(Player), compare_players_by_position);
}
ssize_t read_all(int fd, void *buffer, size_t count) {
  size_t bytes_read = 0;
  char *buf = (char *)buffer;
  while (bytes_read < count) {
    ssize_t result = read(fd, buf + bytes_read, count - bytes_read);
    if (result < 0) {
      return -1; // Error occurred
    }
    if (result == 0) {
      return bytes_read; // EOF reached before reading all requested bytes
    }
    bytes_read += result;
  }
  return bytes_read;
}

void receive_data_from_referee(int fd, Display *disp) {
  // Read all fields from the pipe in the same order they were written

  if (read_all(fd, &disp->in_round, sizeof(disp->in_round)) < 0) {
    perror("Error reading in_round");
    return;
  }
  disp->in_round = disp->in_round - '0';

  if (read_all(fd, &disp->in_simulation, sizeof(disp->in_simulation)) < 0) {
    perror("Error reading in_simulation");
    return;
  }

  disp->in_simulation = disp->in_simulation - '0';
  if (read_all(fd, &disp->current_simulation_time,
               sizeof(disp->current_simulation_time)) < 0) {
    perror("Error reading current_simulation_time");
    return;
  }

  if (read_all(fd, &disp->max_simulation_time,
               sizeof(disp->max_simulation_time)) < 0) {
    perror("Error reading max_simulation_time");
    return;
  }

  if (read_all(fd, &disp->current_round_time,
               sizeof(disp->current_round_time)) < 0) {
    perror("Error reading current_round_time");
    return;
  }

  if (read_all(fd, &disp->number_of_rounds_played,
               sizeof(disp->number_of_rounds_played)) < 0) {
    perror("Error reading number_of_rounds_played");
    return;
  }

  if (read_all(fd, &disp->max_number_of_rounds,
               sizeof(disp->max_number_of_rounds)) < 0) {
    perror("Error reading max_number_of_rounds");
    return;
  }

  if (read_all(fd, &disp->round_score, sizeof(disp->round_score)) < 0) {
    perror("Error reading round_score");
    return;
  }

  if (read_all(fd, &disp->simulation_score, sizeof(disp->simulation_score)) <
      0) {
    perror("Error reading simulation_score");
    return;
  }

  // Read team1 data
  int team1_size;
  if (read_all(fd, &team1_size, sizeof(team1_size)) < 0) {
    perror("Error reading team1 size");
    return;
  }

  // Allocate memory for team1 players if needed
  if (disp->team1.players == NULL) {
    disp->team1.players = (Player *)malloc(team1_size * sizeof(Player));
    if (disp->team1.players == NULL) {
      perror("Failed to allocate memory for team1 players");
      return;
    }
  } else if (disp->team1.size != team1_size) {
    // Reallocate if the team size has changed
    Player *new_players =
        (Player *)realloc(disp->team1.players, team1_size * sizeof(Player));
    if (new_players == NULL) {
      perror("Failed to reallocate memory for team1 players");
      return;
    }
    disp->team1.players = new_players;
  }

  disp->team1.size = team1_size;
  for (int i = 0; i < team1_size; i++) {
    if (read_all(fd, &disp->team1.players[i], sizeof(Player)) < 0) {
      perror("Error reading team1 player");
      return;
    }
  }
  sort_team_by_position(&disp->team1);
  // Read team2 data
  int team2_size;
  if (read_all(fd, &team2_size, sizeof(team2_size)) < 0) {
    perror("Error reading team2 size");
    return;
  }

  // Allocate memory for team2 players if needed
  if (disp->team2.players == NULL) {
    disp->team2.players = (Player *)malloc(team2_size * sizeof(Player));
    if (disp->team2.players == NULL) {
      perror("Failed to allocate memory for team2 players");
      return;
    }
  } else if (disp->team2.size != team2_size) {
    // Reallocate if the team size has changed
    Player *new_players =
        (Player *)realloc(disp->team2.players, team2_size * sizeof(Player));
    if (new_players == NULL) {
      perror("Failed to reallocate memory for team2 players");
      return;
    }
    disp->team2.players = new_players;
  }

  disp->team2.size = team2_size;
  for (int i = 0; i < team2_size; i++) {
    if (read_all(fd, &disp->team2.players[i], sizeof(Player)) < 0) {
      perror("Error reading team2 player");
      return;
    }
  }

  sort_team_by_position(&disp->team2);

  if (read_all(fd, &disp->team1_sum, sizeof(disp->team1_sum)) < 0) {
    perror("Error reading team1_sum");
    return;
  }

  if (read_all(fd, &disp->team2_sum, sizeof(disp->team2_sum)) < 0) {
    perror("Error reading team2_sum");
    return;
  }

  if (read_all(fd, &disp->max_consecutive_wins,
               sizeof(disp->max_consecutive_wins)) < 0) {
    perror("Error reading max_consecutive_wins");
    return;
  }

  if (read_all(fd, &disp->previous_round_result,
               sizeof(disp->previous_round_result)) < 0) {
    perror("Error reading previous_round_result");
    return;
  }

  if (read_all(fd, &disp->current_win_streak,
               sizeof(disp->current_win_streak)) < 0) {
    perror("Error reading current_win_streak");
    return;
  }

  if (read_all(fd, &disp->simulation_winning_method,
               sizeof(disp->simulation_winning_method)) < 0) {
    perror("Error reading simulation_winning_method");
    return;
  }

  if (read_all(fd, &disp->simulation_winner, sizeof(disp->simulation_winner)) <
      0) {
    perror("Error reading simulation_winner");
    return;
  }

  if (read_all(fd, &disp->score_gap_to_win, sizeof(disp->score_gap_to_win)) <
      0) {
    perror("Error reading score_gap_to_win");
    return;
  }
}
void printf_display(Display *disp) {

  printf("disp->in_round: %d\n", disp->in_round);
  printf("disp->in_simulation: %d\n", disp->in_simulation);
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
}

void destroy_display(Display *disp) {
  free(disp->team1.players);
  free(disp->team2.players);
}
