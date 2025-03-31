#define _POSIX_C_SOURCE 200809L
#include "../../include/player.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>

void SIGUSR1_handler(int signum);
void SIGUSR2_handler(int signum);

Player player;
int probability_fall, min_recovery_time, max_recovery_time, min_energy_loss, max_energy_loss;
int write_fd, read_fd; 
int received_position = 0; // Flag to indicate if position is received
int energy_if_fall = 0; // Energy if player falls

int main(int argc, char *argv[]){
    if (argc != 10) {
        fprintf(stderr, "Usage: %s <write_fd> <read_fd> <energy_min> <energy_max> <fall_prob> <min_recovery> <max_recovery> <energy_loss_min> <energy_loss_min>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    srand(time(NULL) ^ getpid());

    write_fd = atoi(argv[1]);
    read_fd = atoi(argv[2]);
    int min_energy = atoi(argv[3]);
    int max_energy = atoi(argv[4]);
    probability_fall = atoi(argv[5]);
    min_recovery_time = atoi(argv[6]);
    max_recovery_time = atoi(argv[7]);
    min_energy_loss = atoi(argv[8]); 
    max_energy_loss = atoi(argv[9]);

    player.pid = getpid();
    player.energy = min_energy + rand() % (max_energy - min_energy + 1);
    player.fall_timeout = 0;

    // Send initial energy to the referee
    write(write_fd, &player.energy, sizeof(player.energy));  // Write first integer
    write(write_fd, &player.fall_timeout, sizeof(player.fall_timeout));  // Write second intege

    if ( sigset(SIGUSR1, SIGUSR1_handler) == SIG_ERR ) {
        perror("signal can not set SIGUSR1");
        exit(SIGUSR1);
    }

    if ( sigset(SIGUSR2, SIGUSR2_handler) == SIG_ERR ) {
        perror("signal can not set SIGUSR2");
        exit(SIGUSR2);
    }

    while(!received_position){
        pause(); // Wait for position from referee
    }

    //printf("Player %d position: %d\n", player.pid, player.position);

    while(1){
        pause(); // Wait for game signals from referee
    }

}



void SIGUSR1_handler(int sig_num){    
    if (!received_position){
        read(read_fd, &player.position, sizeof(int)); // read position from referee
        received_position = 1;
        return;
    }
    // round is ended
    received_position = 0; // reset flag for next round

    int is_winner; // flag to indicate if player is winner
    read(read_fd, &is_winner, sizeof(is_winner)); // read if player is winner
    if (is_winner){
        //printf("Player %d is winner\n", player.pid);
    }
    else {
        //printf("Player %d is loser\n", player.pid);
    }

    // set falltime off to zero (all players get up for the new round)
    player.fall_timeout = 0;

    write(write_fd, &player.energy, sizeof(player.energy));  // Write energy to refree
    write(write_fd, &player.fall_timeout, sizeof(player.fall_timeout));  // Write fall timeout to referee
    return;
}

void SIGUSR2_handler(int sig_num){
    if (player.fall_timeout > 0){
        player.fall_timeout--;
        write(write_fd, &energy_if_fall, sizeof(energy_if_fall));  // Write energy to refree
        write(write_fd, &player.fall_timeout, sizeof(player.fall_timeout));  // Write fall timeout to referee
        return;
    }

    // Determine if player falls
    if (rand() % 100 < probability_fall){
        player.fall_timeout = min_recovery_time + rand() % (max_recovery_time - min_recovery_time + 1);
        write(write_fd, &energy_if_fall, sizeof(energy_if_fall));  // Write energy to refree
        write(write_fd, &player.fall_timeout, sizeof(player.fall_timeout));  // Write fall timeout to referee
        return;
    }

    int energy_loss = min_energy_loss + rand() % (max_energy_loss - min_energy_loss + 1);
    player.energy -= energy_loss;
    if (player.energy < 0) player.energy = 0;

    int effort = player.position * player.energy;
    write(write_fd, &effort, sizeof(effort));  // Write energy to refree
    write(write_fd, &player.fall_timeout, sizeof(player.fall_timeout));  // Write fall timeout to referee
    return;
}