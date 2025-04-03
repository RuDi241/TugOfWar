#include "../../include/team.h"
#include "../../include/error_codes.h"
#include "../../include/player.h"
#include "../../include/team_config.h"
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int create_pipes(Team *team) {

  for (int i = 0; i < team->size; i++) {
    if (pipe(team->players[i].to_player_fd) == -1 ||
        pipe(team->players[i].to_referee_fd) == -1) {
      perror("team1 pipe failed");
      return PIPE_ERROR;
    }
  }

  return 0;
}

int create_team_processes(Team *team, TeamConfig *team_config) {

  create_pipes(team);

  for (int i = 0; i < team_config->num_players; i++) {
    team->players[i].pid = fork(); // create processes
    if (team->players[i].pid < 0) {
      perror("fork failed");
      return 1;
      ;
    } else if (team->players[i].pid == 0) { // Child process
      // Close unused pipe ends
      close(
          team->players[i]
              .to_referee_fd[0]); // player won't read from its to-referee pipe
      close(team->players[i]
                .to_player_fd[1]); // player won't write to its to-team pipe
      for (int j = 0; j < team_config->num_players; j++) {
        // close all unused ends (remember there's exec afterward so the process
        // will shut)
        if (j != i) {
          close(team->players[j].to_referee_fd[0]);
          close(team->players[j].to_referee_fd[1]);
          close(team->players[j].to_player_fd[0]);
          close(team->players[j].to_player_fd[1]);
        }
      }

      // Convert file descriptors to strings for exec
      char write_fd[10], read_fd[10];
      sprintf(write_fd, "%d", team->players[i].to_referee_fd[1]);
      sprintf(read_fd, "%d", team->players[i].to_player_fd[0]);

      // Exec the sum program
      // teamFlag is used to decide which team config file to construct the
      // player from
      execl("./some_child_program", "some_child_program", write_fd, read_fd,
            team_config->initial_energy_range.first,
            team_config->initial_energy_range.second,
            team_config->fall_probability, team_config->fall_time_range.first,
            team_config->fall_time_range.second,
            team_config->energy_decay_range.first,
            team_config->energy_decay_range.second, NULL);
      perror("execl failed");
      return 2;
    }
  }

  // referee process
  // Close unused pipe ends
  for (int i = 0; i < team_config->num_players; i++) {
    close(team->players[i].to_referee_fd[1]); // referee won't write to itself
    close(team->players[i]
              .to_player_fd[0]); // referee won't read from where it writes
  }

  return 0;
}

int make_team(Team *team, TeamConfig *team_config) {
  team->size = team_config->num_players;
  team->players = malloc(team->size * sizeof(Player));
  create_team_processes(team, team_config);
  return 0;
}

int compare_by_energy(const void *a, const void *b) {
  const Player *p_a = (const Player *)a;
  const Player *p_b = (const Player *)b;
  return p_a->energy - p_b->energy; // Ascending order
}

int arrange_team(Team *team) {
  qsort(team->players, team->size, sizeof(Player), compare_by_energy);

  for (int i = 0; i < team->size; i++) {
    team->players[i].position = i + 1; // Rank starts at 1
  }

  return 0;
}

int send_position_to_team(Team *team) {
  for (int i = 0; i < team->size; i++) {
    int rank = team->players[i].position;
    ssize_t bytes_written =
        write(team->players[i].to_player_fd[1], &rank, sizeof(rank));
    if (bytes_written != sizeof(rank)) {
      perror("failed to write position to team1 player");
      close(team->players[i].to_player_fd[1]); // Clean up
      return WRITE_PIPE_ERROR;
    }
    kill(team->players[i].pid, SIGUSR1);
  }

  return 0;
}

int receive_data_from_team(Team *team) {
  for (int i = 0; i < team->size; i++) {
    int energyAndTimeOut[2];
    if (read(team->players[i].to_referee_fd[0], energyAndTimeOut,
             sizeof(energyAndTimeOut)) > 0) {
      team->players[i].energy = energyAndTimeOut[0];
      team->players[i].fall_timeout = energyAndTimeOut[1];
    }
  }

  return 0;
}
