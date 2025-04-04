#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500
#include "../../include/game_state.h"
#include "../../include/player.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <time.h>

void SIGUSR1_handler(int signum);
void SIGUSR2_handler(int signum);

Player player;
int probability_fall, min_recovery_time, max_recovery_time, min_energy_loss,
    max_energy_loss;
int received_position = 0; // Flag to indicate if position is received
int energy_if_fall = 0;    // Energy if player falls

int main(int argc, char *argv[]) {
  if (argc != 10) {
    fprintf(stderr,
            "Usage: %s <to_referee_write_fd> <to_player_read_fd> <energy_min> "
            "<energy_max> <energy_loss_min> <energy_loss_max> <fall_prob> "
            "<min_recovery_time> <max_recovery_time>\n",
            argv[0]);
    exit(EXIT_FAILURE);
  }

  // printf("from player %d %s %s %s %s %s %s %s %s %s\n", getpid(), argv[1],
  // argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9]);

  srand(time(NULL) ^ getpid());

  player.to_referee_fd[1] = atoi(argv[1]);
  player.to_player_fd[0] = atoi(argv[2]);
  int min_energy = atoi(argv[3]);
  int max_energy = atoi(argv[4]);
  min_energy_loss = atoi(argv[5]);
  max_energy_loss = atoi(argv[6]);
  probability_fall = atoi(argv[7]);
  min_recovery_time = atoi(argv[8]);
  max_recovery_time = atoi(argv[9]);

  player.pid = getpid();
  player.energy = min_energy + rand() % (max_energy - min_energy + 1);
  player.fall_timeout = 0;

  printf("Player %d started with energy %d\n", player.pid, player.energy);
  // printf("min_energy_loss: %d, max_energy_loss: %d\n", min_energy_loss,
  // max_energy_loss); printf("min_recovery_time: %d, max_recovery_time: %d\n",
  // min_recovery_time, max_recovery_time); printf("probability_fall: %d\n",
  // probability_fall); printf("min_energy: %d, max_energy: %d\n", min_energy,
  // max_energy); printf("\n");

  // Send initial energy to the referee
  write(player.to_referee_fd[1], &player.energy,
        sizeof(player.energy)); // Write first integer
  write(player.to_referee_fd[1], &player.fall_timeout,
        sizeof(player.fall_timeout)); // Write second intege

  if (sigset(SIGUSR1, SIGUSR1_handler) == SIG_ERR) {
    perror("signal can not set SIGUSR1");
    exit(SIGUSR1);
  }

  if (sigset(SIGUSR2, SIGUSR2_handler) == SIG_ERR) {
    perror("signal can not set SIGUSR2");
    exit(SIGUSR2);
  }

  while (!received_position) {
    pause(); // Wait for position from referee
  }

  // Round is started
  while (1) {
    pause(); // Wait for game signals from referee
  }
}

void SIGUSR1_handler(int sig_num) {
  // Blocking SIGTERM during SIGUSR1 handler
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGTERM);
  sigprocmask(SIG_BLOCK, &mask, NULL); // Block SIGTERM signal

  if (!received_position) {
    if (read(player.to_player_fd[0], &player.position, sizeof(int)) == -1) {
      perror("Error reading position from pipe");
      return;
    }
    printf("Player %d received position %d\n", player.pid, player.position);
    received_position = 1;
    return;
  }
  // round is ended
  received_position = 0; // reset flag for next round

  int lastRoundOutcome; // flag to indicate if player is winner
  if (read(player.to_player_fd[0], &lastRoundOutcome,
           sizeof(lastRoundOutcome)) == -1) { // read if player is winner
    perror(
        "Error reading last Round Outcome from pipe"); // Print the error
                                                       // message based on errno
    return;
  }

  if (lastRoundOutcome == WIN) {
    printf("Player %d is winner\n", player.pid);
  } else if (lastRoundOutcome == LOSE) {
    printf("Player %d is loser\n", player.pid);
  } else {
    printf("Player %d is draw\n", player.pid);
  }

  sigprocmask(SIG_UNBLOCK, &mask, NULL);

  // set falltime off to zero (all players get up for the new round)
  player.fall_timeout = 0;

  if (write(player.to_referee_fd[1], &player.energy, sizeof(player.energy)) ==
      -1) {
    perror("Error writing energy to referee");
    return;
  }

  if (write(player.to_referee_fd[1], &player.fall_timeout,
            sizeof(player.fall_timeout)) == -1) {
    perror("Error writing fall timeout to referee");
    return;
  }

  return;
}

void SIGUSR2_handler(int sig_num) {
  if (player.fall_timeout > 0) {
    player.fall_timeout--;
    if (write(player.to_referee_fd[1], &energy_if_fall,
              sizeof(energy_if_fall)) == -1) {
      perror("Error writing energy if fall to referee");
    }

    if (write(player.to_referee_fd[1], &player.fall_timeout,
              sizeof(player.fall_timeout)) == -1) {
      perror("Error writing fall timeout to referee");
    }

    return;
  }

  // Determine if player falls
  if (rand() % 100 < probability_fall) {
    player.fall_timeout = min_recovery_time +
                          rand() % (max_recovery_time - min_recovery_time + 1);
    if (write(player.to_referee_fd[1], &energy_if_fall,
              sizeof(energy_if_fall)) == -1) {
      perror("Error writing energy if fall to referee");
    }

    if (write(player.to_referee_fd[1], &player.fall_timeout,
              sizeof(player.fall_timeout)) == -1) {
      perror("Error writing fall timeout to referee");
    }

    return;
  }

  int energy_loss =
      min_energy_loss + rand() % (max_energy_loss - min_energy_loss + 1);
  player.energy -= energy_loss;
  if (player.energy < 0)
    player.energy = 0;

  int effort = player.position * player.energy;
  if (write(player.to_referee_fd[1], &effort, sizeof(effort)) == -1) {
    perror("Error writing effort to referee");
  }

  if (write(player.to_referee_fd[1], &player.fall_timeout,
            sizeof(player.fall_timeout)) == -1) {
    perror("Error writing fall timeout to referee");
  }

  return;
}
