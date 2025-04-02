#ifndef PLAYER_H
#define PLAYER_H
#include "string_buf.h"
#include <sys/types.h>

typedef struct Player {
  pid_t pid;
  int position;
  int energy;
  int fall_timeout;
  int to_player_fd[2];
  int to_referee_fd[2];
} Player;

int serialize_player(Player *player, StringBuf *string_buf);
int deserialize_player(Player *player, StringBuf *string_buf);


#endif
