#ifndef PLAYER_H
#define PLAYER_H
#include <sys/types.h>

typedef struct Player {
  pid_t pid;
  int position;
  int energy;
} Player;

#endif
