#ifndef PLAYER_H
#define PLAYER_H
#include <sys/types.h>

typedef struct Player {
  pid_t pid;
  int position;
  int energy;
  int fall_timeout;
  int to_player_fd[2];
  int to_referee_fd[2];
} Player;

#endif
