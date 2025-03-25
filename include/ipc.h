#include "../include/team.h"

int init_interprocess_communication(pid_t *t1_pid, int (*to_t1_fd)[2], int (*to_referee_t1_fd)[2],
                                    pid_t *t2_pid, int (*to_t2_fd)[2], int (*to_referee_t2_fd)[2],
                                    Team *team1, Team *team2);

int create_pipes( int (*to_t1_fd)[2], int (*to_referee_t1_fd)[2], int (*to_t2_fd)[2], int (*to_referee_t2_fd)[2], Team *team1, Team *team2);