#include "team.h"
#include <unistd.h>
#include <stdlib.h>

int init_interprocess_communication(pid_t **, int (**)[2], int (**)[2],
                                    pid_t **, int (**)[2], int (**)[2],
                                    Team team1, Team team2);

int create_pipes( int (**)[2], int (**)[2], int (**)[2], int (**)[2], Team team1, Team team2);