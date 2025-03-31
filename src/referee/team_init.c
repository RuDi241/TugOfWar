#include "../../include/team.h"

int make_team(Team *team, TeamConfig *team_config)
{
    team->size = team_config->num_players;
    team->players = malloc(team->size * sizeof(Player));
    return 0;
}

int create_team_processes(pid_t *team_pid, int (*to_team_fd)[2], int (*to_referee_team_fd)[2], TeamConfig team_config, Team *team)
{
    for (int i = 0; i < team_config.num_players; i++)
    {
        pid_t process_PID = fork(); // create processes
        if (process_PID < 0)
        {
            perror("fork failed");
            return 1;
        }
        else if (process_PID == 0)
        { // Child process
            // Close unused pipe ends
            close(to_referee_team_fd[i][0]); // player won't read from its to-referee pipe
            close(to_team_fd[i][1]);         // player won't write to its to-team pipe
            for (int j = 0; j < team_config.num_players; j++)
            {
                // close all unused ends (remember there's exec afterward so the process will shut)
                if (j != i)
                {
                    close(to_referee_team_fd[j][0]);
                    close(to_referee_team_fd[j][1]);
                    close(to_team_fd[j][0]);
                    close(to_team_fd[j][1]);
                }
            }

            // Convert the necessary values to strings
            char write_fd[10], read_fd[10];
            char initial_energy_first[10], initial_energy_second[10];
            char energy_decay_first[10], energy_decay_second[10];
            char fall_probability_str[10];
            char fall_time_first[10], fall_time_second[10];

            // Convert file descriptors to strings
            sprintf(write_fd, "%d", to_referee_team_fd[i][1]);
            sprintf(read_fd, "%d", to_team_fd[i][0]);

            // Convert the PairIntInt values to strings
            sprintf(initial_energy_first, "%d", team_config.initial_energy_range.first);
            sprintf(initial_energy_second, "%d", team_config.initial_energy_range.second);
            sprintf(energy_decay_first, "%d", team_config.energy_decay_range.first);
            sprintf(energy_decay_second, "%d", team_config.energy_decay_range.second);
            sprintf(fall_probability_str, "%.2f", team_config.fall_probability); // Double to string
            sprintf(fall_time_first, "%d", team_config.fall_time_range.first);
            sprintf(fall_time_second, "%d", team_config.fall_time_range.second);

            // Exec the sum program with the converted values
            execl("./bin/player_debug", "player_debug",
                  write_fd, read_fd,
                  initial_energy_first, initial_energy_second,
                  energy_decay_first, energy_decay_second,
                  fall_probability_str,
                  fall_time_first, fall_time_second, NULL);
            perror("execl failed");
            return 2;
        }
        else {
            team->players[i].pid = process_PID; // Parent process
            team->players[i].read_fd = to_team_fd[i][0]; // Read from the pipe
            team->players[i].write_fd = to_referee_team_fd[i][1]; // Write to the pipe
        }
    }

    // referee process
    // Close unused pipe ends
    for (int i = 0; i < team_config.num_players; i++)
    {
        close(to_referee_team_fd[i][1]); // referee won't write to itself
        close(to_team_fd[i][0]);         // referee won't read from where it writes
    }

    return 0;
}

int arrange_team(Team *team)
{
    ;
    PlayerEnergyIndexed *temp = malloc(team->size * sizeof(*temp));
    for (int i = 0; i < team->size; i++)
    {
        temp[i].index = i;
        temp[i].energy = team->players[i].energy;
    }

    qsort(temp, team->size, sizeof(PlayerEnergyIndexed), compare_by_energy);

    for (int i = 0; i < team->size; i++)
    {
        team->players[temp[i].index].position = i + 1; // Rank starts at 1
    }

    return 0;
}

int compare_by_energy(const void *a, const void *b)
{
    const PlayerEnergyIndexed *pe_a = (const PlayerEnergyIndexed *)a;
    const PlayerEnergyIndexed *pe_b = (const PlayerEnergyIndexed *)b;
    return pe_a->energy - pe_b->energy; // Ascending order
}

int send_position_to_team(int (*to_team_fd)[2], Team *team)
{
    for (int i = 0; i < team->size; i++)
    {
        int rank = team->players[i].position;
        ssize_t bytes_written = write(to_team_fd[i][1], &rank, sizeof(rank));
        if (bytes_written != sizeof(rank))
        {
            perror("failed to write position to team1 player");
            close(to_team_fd[i][1]); // Clean up
            return 1;
        }
        kill(team->players[i].pid, SIGUSR1);
    }
}

int receive_data_from_team(Team *team, int (*to_referee_team_fd)[2])
{
    for (int i = 0; i < team->size; i++)
    {
        int energyAndTimeOut[2];
        if (read(to_referee_team_fd[i][0], energyAndTimeOut, sizeof(energyAndTimeOut)) > 0)
        {
            //printf("Received data from player %d: energy = %d, fall_timeout = %d\n", team->players[i].pid, energyAndTimeOut[0], energyAndTimeOut[1]);
            team->players[i].energy = energyAndTimeOut[0];
            team->players[i].fall_timeout = energyAndTimeOut[1];
        }
    }
}
