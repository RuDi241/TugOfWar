#include "../../include/ipc.h"

int init_interprocess_communication(pid_t **t1_pid, int (**to_t1_fd)[2], int (**to_referee_t1_fd)[2],
                                    pid_t **t2_pid, int (**to_t2_fd)[2], int (**to_referee_t2_fd)[2],
                                    Team team1, Team team2)
{

    int num_team1_players = team1.size;
    int num_team2_players = team2.size;

    // Allocate memory for t1_pid and t2_pid
    *t1_pid = (pid_t *)malloc(num_team1_players * sizeof(pid_t));
    (*t1_pid)[0] = 123;
    if (*t1_pid == NULL)
    {
        perror("malloc for t1_pid failed");
        return 1;
    }

    *t2_pid = (pid_t *)malloc(num_team2_players * sizeof(pid_t));
    if (*t2_pid == NULL)
    {
        perror("malloc for t2_pid failed");
        return 2;
    }

    // Allocate memory for the file descriptors
    (*to_t1_fd) = (int (*)[2])malloc(num_team1_players * sizeof(int[2]));
    if (*to_t1_fd == NULL)
    {
        perror("malloc for to_t1_fd failed");
        return 3;
    }

    (*to_referee_t1_fd) = (int (*)[2])malloc(num_team1_players * sizeof(int[2]));
    if (*to_referee_t1_fd == NULL)
    {
        perror("malloc for to_referee_t1_fd failed");
        return 4;
    }

    (*to_t2_fd) = (int (*)[2])malloc(num_team2_players * sizeof(int[2]));
    if (*to_t2_fd == NULL)
    {
        perror("malloc for to_t2_fd failed");
        return 5;
    }

    (*to_referee_t2_fd) = (int (*)[2])malloc(num_team2_players * sizeof(int[2]));
    if (*to_referee_t2_fd == NULL)
    {
        perror("malloc for to_referee_t2_fd failed");
        return 6;
    }

    return 0;
}

int create_pipes(int (**to_t1_fd)[2], int (**to_referee_t1_fd)[2], int (**to_t2_fd)[2], int (**to_referee_t2_fd)[2], Team team1, Team team2)
{

    for (int i = 0; i < team1.size; i++)
    {
        if (pipe((*to_t1_fd)[i]) == -1 || pipe((*to_referee_t1_fd)[i]) == -1)
        {
            perror("team1 pipe failed");
            return 1;
        }
    }

    for (int i = 0; i < team2.size; i++)
    {
        if (pipe((*to_t2_fd)[i]) == -1 || pipe((*to_referee_t2_fd)[i]) == -1)
        {
            perror("team2 pipe failed");
            return 2;
        }
    }

    return 0;
}