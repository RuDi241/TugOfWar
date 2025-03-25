#include "../include/ipc.h"

int init_interprocess_communication(pid_t *t1_pid, int (*to_t1_fd)[2], int (*to_referee_t1_fd)[2],
                                    pid_t *t2_pid, int (*to_t2_fd)[2], int (*to_referee_t2_fd)[2],
                                    Team *team1, Team *team2){

    int num_team1_players = team1->size;
    int num_team2_players = team2->size;
    
    
    //initialize pipes for each team
    to_t1_fd = malloc(num_team1_players * sizeof(*to_t1_fd));
    to_referee_t1_fd = malloc(num_team1_players * sizeof(*to_referee_t1_fd)); 
    t1_pid = malloc(num_team1_players * sizeof(*t1_pid));
    
    if (to_t1_fd == NULL || to_referee_t1_fd == NULL || t1_pid == NULL) {
        perror("mallocs of Team1 failed");
        return 1;
    }
    
    to_t2_fd = malloc(num_team2_players * sizeof(*to_t2_fd));
    to_referee_t2_fd = malloc(num_team2_players * sizeof(*to_referee_t2_fd)); 
    t2_pid = malloc(num_team2_players * sizeof(*t2_pid));
    
    if (to_t2_fd == NULL || to_referee_t2_fd == NULL || t2_pid == NULL) {
        perror("mallocs of Team1 failed");
        return 2;
    }
    
    return 0;
}

int create_pipes( int (*to_t1_fd)[2], int (*to_referee_t1_fd)[2], int (*to_t2_fd)[2], int (*to_referee_t2_fd)[2], Team *team1, Team *team2){

    for (int i = 0; i < team1->size; i++) {
        if (pipe(to_t1_fd[i]) == -1 || pipe(to_referee_t1_fd[i]) == -1) {
            perror("team1 pipe failed");
            return 1;
        }
    }

    for (int i = 0; i < team2->size; i++) {
        if (pipe(to_t2_fd[i]) == -1 || pipe(to_referee_t2_fd[i]) == -1) {
            perror("team2 pipe failed");
            return 2;
        }
    }

    return 0;

}