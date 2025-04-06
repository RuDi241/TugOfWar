#include "../../include/team.h"
#include "../../include/error_codes.h"
#include "../../include/player.h"
#include "../../include/team_config.h"
#include "../../include/game_state.h"
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int create_pipes(Team *team)
{

  for (int i = 0; i < team->size; i++)
  {
    if (pipe(team->players[i].to_player_fd) == -1 ||
        pipe(team->players[i].to_referee_fd) == -1)
    {
      perror("team1 pipe failed");
      return PIPE_ERROR;
    }
  }

  return 0;
}

int create_team_processes(Team *team, TeamConfig *team_config)
{

  create_pipes(team);

  for (int i = 0; i < team_config->num_players; i++)
  {

    team->players[i].pid = fork(); // create processes
    if (team->players[i].pid < 0)
    {
      perror("fork failed");
      return FORK_ERROR;
    }
    else if (team->players[i].pid == 0)
    { // Child process
      // Close unused pipe ends
      close(team->players[i].to_referee_fd[0]); // player won't read from its to-referee pipe
      close(team->players[i].to_player_fd[1]);  // player won't write to its to-team pipe
      for (int j = 0; j < team_config->num_players; j++)
      {
        // close all unused ends (remember there's exec afterward so the process will shut)
        if (j != i)
        {
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

      // Convert the necessary values to strings
      char initial_energy_first[10], initial_energy_second[10];
      char energy_decay_first[10], energy_decay_second[10];
      char fall_probability_str[10];
      char fall_time_first[10], fall_time_second[10];

      // Convert the PairIntInt values to strings
      int fall_percentage = (int)(team_config->fall_probability * 100); // convert fall_probability to percentage

      sprintf(initial_energy_first, "%d", team_config->initial_energy_range.first);
      sprintf(initial_energy_second, "%d", team_config->initial_energy_range.second);
      sprintf(energy_decay_first, "%d", team_config->energy_decay_range.first);
      sprintf(energy_decay_second, "%d", team_config->energy_decay_range.second);
      sprintf(fall_probability_str, "%d", fall_percentage);
      sprintf(fall_time_first, "%d", team_config->fall_time_range.first);
      sprintf(fall_time_second, "%d", team_config->fall_time_range.second);

      // Exec the player process
      execl("./bin/player", "player",
            write_fd, read_fd,
            initial_energy_first, initial_energy_second,
            energy_decay_first, energy_decay_second,
            fall_probability_str,
            fall_time_first, fall_time_second, NULL);
      perror("execl failed");
      return EXEC_ERROR;
    }
  }

  // referee process
  // Close unused pipe ends
  for (int i = 0; i < team_config->num_players; i++)
  {
    close(team->players[i].to_referee_fd[1]); // referee won't write to itself
    close(team->players[i].to_player_fd[0]);  // referee won't read from where it writes
  }

  return 0;
}

int make_team(Team *team, TeamConfig *team_config)
{
  team->size = team_config->num_players;
  team->players = malloc(team->size * sizeof(Player));
  create_team_processes(team, team_config);
  return 0;
}

int compare_by_energy(const void *a, const void *b)
{
  const Player *p_a = (const Player *)a;
  const Player *p_b = (const Player *)b;
  return p_a->energy - p_b->energy; // Ascending order
}

int arrange_team(Team *team)
{
  qsort(team->players, team->size, sizeof(Player), compare_by_energy);

  for (int i = 0; i < team->size; i++)
  {
    team->players[i].position = i + 1; // Rank starts at 1
  }

  return 0;
}

int send_position_to_team(Team *team)
{
  for (int i = 0; i < team->size; i++)
  {
    int rank = team->players[i].position;
    ssize_t bytes_written = write(team->players[i].to_player_fd[1], &rank, sizeof(rank));
    if (bytes_written != sizeof(rank))
    {
      perror("failed to write position to team1 player");
      close(team->players[i].to_player_fd[1]); // Clean up
      return WRITE_PIPE_ERROR;
    }
    kill(team->players[i].pid, SIGUSR1);
  }

  return 0;
}

int receive_data_from_team(Team *team)
{
  for (int i = 0; i < team->size; i++)
  {
    if (read(team->players[i].to_referee_fd[0], &team->players[i].energy, sizeof(team->players[i].energy)) <= 0)
    {
      perror("Failed to receive data from Player.");
      close(team->players[i].to_referee_fd[0]); // Clean up
      return READ_PIPE_ERROR;
    }
    if (read(team->players[i].to_referee_fd[0], &team->players[i].fall_timeout, sizeof(team->players[i].fall_timeout)) <= 0)
    {
      perror("Failed to receive data from Player.");
      close(team->players[i].to_referee_fd[0]); // Clean up
      return READ_PIPE_ERROR;
    }
    printf("Player %d: Energy: %d, Fall Timeout: %d\n", team->players[i].pid, team->players[i].energy, team->players[i].fall_timeout);
  }

  return 0;
}

int announce_result_to_team(Team *team, int lastRoundOutcome)
{
  for (int i = 0; i < team->size; i++)
  {
    ssize_t bytes_written = write(team->players[i].to_player_fd[1], &lastRoundOutcome, sizeof(lastRoundOutcome));
    if (bytes_written == -1)
    {
      perror("Error writing to pipe");
      return WRITE_PIPE_ERROR;
    }
    else if (bytes_written != sizeof(lastRoundOutcome))
    {
      fprintf(stderr, "Partial write detected (expected %zu, wrote %zd)\n", sizeof(lastRoundOutcome), bytes_written);
      return INCOMPLETE_PIPE_WRITE;
    }

    if (kill(team->players[i].pid, SIGUSR1) == -1)
    {
      perror("Error sending signal to player");
      return SIGNAL_ERROR;
    }
  }

  return 0;
}

int destroy_team(Team *team)
{
  // add signal handler to kill all players
  for (int i = 0; i < team->size; i++)
  {
    int status;
    if (waitpid(team->players[i].pid, &status, WNOHANG) == -1)
    {
      perror("Error waiting for player to pause and terminate");
      return WAIT_ERROR; 
    }

    if (WIFSTOPPED(status))
    { // The child is paused
      if (kill(team->players[i].pid, SIGTERM) == -1)
      {
        perror("Failed to kill player");
        return SIGNAL_ERROR;
      }
    }
  }

  for (int i = 0; i < team->size; i++)
  {
    close(team->players[i].to_player_fd[1]);
    close(team->players[i].to_referee_fd[0]);
  }

  free(team->players);
  return 0;
}