#include "../include/team_config.h"
#include "../include/team.h"
#include "../include/player.h"
#include <stdlib.h>
#include <signal.h>


int make_team(Team *team, TeamConfig *team_config){
    team->size = team_config->num_players;
    team->players = malloc(team->size * sizeof(Player));    
    return 0;
}

int create_team_processes(  pid_t *team_pid, int (*to_team_fd)[2], int (*to_referee_team_fd)[2],  TeamConfig *team_config){
    for (int i = 0; i < team_config->num_players; i++) {
        team_pid[i] = fork(); //create processes
        if (team_pid[i] < 0) {
            perror("fork failed");
            return 1;;
        }
        else if (team_pid[i] == 0) { // Child process
            // Close unused pipe ends
            close(to_referee_team_fd[i][0]); // player won't read from its to-referee pipe
            close(to_team_fd[i][1]);  // player won't write to its to-team pipe
            for (int j = 0; j < team_config->num_players; j++) {
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
            execl("./some_child_program", "some_child_program", 
                write_fd, read_fd, team_config->initial_energy_range.first, team_config->initial_energy_range.second, team_config->fall_probability,
                team_config->fall_time_range.first, team_config->fall_time_range.second, team_config->energy_decay_range.first, team_config->energy_decay_range.second, NULL); 
            perror("execl failed");
            return 2;
        }
    }

    // referee process
    // Close unused pipe ends
    for (int i = 0; i < team_config->num_players; i++) {
    close(to_referee_team_fd[i][1]); // referee won't write to itself
    close(to_team_fd[i][0]);  // referee won't read from where it writes
    }


    return 0;
}

int arrange_team(Team *team){

    PlayerEnergyIndexed *temp = malloc(team->size * sizeof(*temp));
    for (int i = 0; i < team->size; i++) {
        temp[i].index = i;
        temp[i].energy = team[i].players->energy;
    }

    qsort(temp, team->size, sizeof(PlayerEnergyIndexed), compare_by_energy);

    for (int i = 0; i < team->size; i++) {
        team->players[temp[i].index].position = i + 1; // Rank starts at 1
    }

    return 0;
}

int compare_by_energy(const void *a, const void *b) {
    const PlayerEnergyIndexed *pe_a = (const PlayerEnergyIndexed *)a;
    const PlayerEnergyIndexed *pe_b = (const PlayerEnergyIndexed *)b;
    return pe_a->energy - pe_b->energy; // Ascending order
}

int send_position_to_team( pid_t *team_pid, int (*to_team_fd)[2], Team *team){
    for(int i = 0; i < team->size; i++){
        int rank = team->players[i].position;
        ssize_t bytes_written = write(to_team_fd[i][1], &rank, sizeof(rank));
        if(bytes_written != sizeof(rank)){
          perror("failed to write position to team1 player");
          close(to_team_fd[i][1]); // Clean up
          return 1; 
        }
        kill(team_pid[i], SIGUSR1);
      }
}

int receive_data_from_team(pid_t *team_pid, int (*to_referee_team_fd)[2], Team *team){
    for(int i = 0; i < team->size; i++){
        int energyAndTimeOut[2];
        if(read(to_referee_team_fd[i][0], energyAndTimeOut, sizeof(energyAndTimeOut)) > 0){
          team->players[i].pid = team_pid[i];
          team->players[i].energy = energyAndTimeOut[0];
          team->players[i].fall_timeout = energyAndTimeOut[1];
        }
      }
}




