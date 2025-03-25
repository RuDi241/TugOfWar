#include "../include/team_config.h"
#include "../include/team.h"
#include "../include/player.h"
#include <stdlib.h>

int make_team(Team *team, TeamConfig *team_config){
    team->size = team_config->num_players;
    team->players = malloc(team->size * sizeof(Player));    
    return 0;
}

int create_team_processes(  pid_t *team_pid, int (*to_team_fd)[2], int (*to_referee_team_fd)[2],  Team *team, char teamFlag){
    for (int i = 0; i < team->size; i++) {
        team_pid[i] = fork(); //create processes
        if (team_pid[i] < 0) {
            perror("fork failed");
            return 1;;
        }
        else if (team_pid[i] == 0) { // Child process
            // Close unused pipe ends
            close(to_referee_team_fd[i][0]); // player won't read from its to-referee pipe
            close(to_team_fd[i][1]);  // player won't write to its to-team pipe
            for (int j = 0; j < team->size; j++) {
                //close all unused ends (remember there's exec afterward so the process will shut)
                if (j != i) {
                    close(to_referee_team_fd[j][0]);
                    close(to_referee_team_fd[j][1]);
                    close(to_team_fd[j][0]);
                    close(to_team_fd[j][1]);
                }
            }

            // Convert file descriptors to strings for exec
            char write_fd[10], read_fd[10];
            sprintf(write_fd, "%d", to_referee_team_fd[i][1]);
            sprintf(read_fd, "%d", to_team_fd[i][0]);

            // Exec the sum program
            //teamFlag is used to decide which team config file to construct the player from
            execl("./some_child_program", "some_child_program", write_fd, read_fd, teamFlag, NULL); 
            perror("execl failed");
            return 2;
        }
    }

    // referee process
    // Close unused pipe ends
    for (int i = 0; i < team->size; i++) {
    close(to_referee_team_fd[i][1]); // referee won't write to itself
    close(to_team_fd[i][0]);  // referee won't read from where it writes
    }


    return 0;
}