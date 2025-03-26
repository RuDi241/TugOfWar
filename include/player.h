#ifndef PLAYER_H
#define PLAYER_H
#include "string_buf.h"
#include <sys/types.h>

typedef struct Player {
  pid_t pid;
  int position;
  int energy;
  int fall_timeout;
} Player;

int serialize_player(Player *player, StringBuf *string_buf);
int deserialize_player(Player *player, StringBuf *string_buf);


#endif
