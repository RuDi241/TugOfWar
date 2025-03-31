#include "../../include/main.h"

GameConfig game_config;
GameState game_state;
TeamConfig team1_config, team2_config;
Team team1, team2;



int main(int argc, char *argv[])
{
  pid_t *t1_pid, *t2_pid;
  int (*to_t1_fd)[2], (*to_t2_fd)[2];
  int (*to_referee_t1_fd)[2], (*to_referee_t2_fd)[2];

  int exit_code = initialize_configs(&game_config, &team1_config, &team2_config);
  if (exit_code != 0)
  {
    return exit_code; // Exit with the error code
  }

  // create team1,2
  make_team(&team1, &team1_config);
  make_team(&team2, &team2_config);

  // init game state
  init_game_state(&game_state, &game_config, &team1, &team2);

  init_interprocess_communication(&t1_pid, &to_t1_fd, &to_referee_t1_fd, &t2_pid, &to_t2_fd, &to_referee_t2_fd, team1, team2);

  create_pipes(&to_t1_fd, &to_referee_t1_fd, &to_t2_fd, &to_referee_t2_fd, team1, team2);

  // create team1's processes
  create_team_processes(t1_pid, to_t1_fd, to_referee_t1_fd, team1_config, &team1);

  // create team2's processes
  create_team_processes(t2_pid, to_t2_fd, to_referee_t2_fd, team2_config, &team2);

  run_simulation(&to_t1_fd, &to_referee_t1_fd, &to_t2_fd, &to_referee_t2_fd);

  // Free allocated memory and kill all processes

  return 0;
}

void run_simulation(int (**to_t1_fd)[2], int (**to_referee_t1_fd)[2], int (**to_t2_fd)[2], int (**to_referee_t2_fd)[2])
{
  struct timeval start_simulation, now, start_round;
  long elapsed_s_from_sim_time;
  int consecutive_wins_team1 = 0, consecutive_wins_team2 = 0;
  int max_consecutive_wins = game_state.max_consecutive_wins;

  gettimeofday(&start_simulation, NULL);

  while (1)
  {
    gettimeofday(&now, NULL);
    elapsed_s_from_sim_time = now.tv_sec - start_simulation.tv_sec;

    // **Terminate the whole simulation if the time limit is reached**
    if (elapsed_s_from_sim_time >= game_config.max_simulation_time)
    {
      printf("Simulation ended: Time limit reached\n");
      break;
    }

    // **At the start of each round, receive team data and arrange teams**
    receive_data_from_team(&team1, *to_referee_t1_fd);
    arrange_team(&team1);
    receive_data_from_team(&team2, *to_referee_t2_fd);
    arrange_team(&team2);

    send_position_to_team(*to_t1_fd, &team1);
    send_position_to_team(*to_t2_fd, &team2);

    long long round_sum = 0;
    gettimeofday(&start_round, NULL);

    long long elapsed_ms_from_round_time;
    long long eplased_ms_from_last_measurement = 0;
    long long elapsed_ms_at_last_measurement = 0;
    game_state.in_round = 'i';
    game_state.simulation_time = elapsed_s_from_sim_time;
    game_state.round_score = 0;
    // update_screen(game_state);
    // **Start the round**
    printf("Round started\n");
    while (1)
    {
      gettimeofday(&now, NULL);
      elapsed_ms_from_round_time = (now.tv_sec - start_round.tv_sec) * 1000 + (now.tv_usec - start_round.tv_usec) / 1000;
      eplased_ms_from_last_measurement = elapsed_ms_from_round_time - elapsed_ms_at_last_measurement;
      elapsed_s_from_sim_time = now.tv_sec - start_simulation.tv_sec;

      // **Receive data every second**
      if (eplased_ms_from_last_measurement >= 3000) // for testing (this should be 1000)
      {

        // **Check round termination conditions**
        if (elapsed_ms_from_round_time >= game_config.max_round_time * 1000 ||
            elapsed_s_from_sim_time >= game_config.max_simulation_time ||
            abs(round_sum) >= game_state.round_threshold_score)
        {
          break;
        }

        // **Send SIGUSR2 to all players to receive data**
        for (int i = 0; i < team1.size; i++)
        {
          kill(team1.players[i].pid, SIGUSR2);
        }
        for (int i = 0; i < team2.size; i++)
        {
          kill(team2.players[i].pid, SIGUSR2);
        }

        receive_data_from_team(&team1, *to_referee_t1_fd);
        receive_data_from_team(&team2, *to_referee_t2_fd);

        long long sum_t1 = 0, sum_t2 = 0;
        for (int i = 0; i < team1.size; i++)
          sum_t1 += (long long)(team1.players[i].energy);

        for (int i = 0; i < team2.size; i++)
          sum_t2 += (long long)(team2.players[i].energy);

        round_sum += (sum_t1 - sum_t2);
        game_state.round_score = round_sum;
        game_state.simulation_time = elapsed_s_from_sim_time;
        game_state.round_time = elapsed_ms_from_round_time / 1000;
        // update_screen(game_state);

        printf("Round sum: %lld\n", round_sum);
        printf("Team 1 total energy: %lld\n", sum_t1);
        printf("Team 2 total energy: %lld\n", sum_t2);

        elapsed_ms_at_last_measurement = elapsed_ms_from_round_time;
      }
    }

    game_state.in_round = 'o';

    // **Determine round winner**
    if (round_sum > 0)
    {
      game_state.round_winner = 1;
      game_state.round_loser = 2;
      game_state.simulation_score.first += 1;
      // update_screen(game_state);
      consecutive_wins_team1++;
      consecutive_wins_team2 = 0;
      printf("Team 1 wins the round!\n");
      
      announce_winner(&team1, *to_t1_fd);
      announce_loser(&team2, *to_t2_fd);
    }
    else if (round_sum < 0)
    {
      game_state.round_winner = 2;
      game_state.round_loser = 1;
      game_state.simulation_score.second += 1;
      // update_screen(game_state);
      consecutive_wins_team2++;
      consecutive_wins_team1 = 0;
      printf("Team 2 wins the round!\n");
      announce_winner(&team2, *to_t2_fd);
      announce_loser(&team1, *to_t1_fd);
    }
    else
    {
      consecutive_wins_team1 = 0;
      consecutive_wins_team2 = 0;
      printf("Round ended in a draw!\n");
    }

    // **End simulation if a team wins too many rounds in a row**
    if (consecutive_wins_team1 >= max_consecutive_wins)
    {
      printf("Team 1 wins the simulation with %d consective wins!\n", consecutive_wins_team1);
      // update_screen(game_state);
      return;
    }
    if (consecutive_wins_team2 >= max_consecutive_wins)
    {
      printf("Team 2 wins the simulation with %d consictive wins!\n", consecutive_wins_team2);
      // update_screen(game_state);
      return;
    }
  }

  // simulation ended because of time limit
  if (game_state.simulation_score.first > game_state.simulation_score.second)
  {
    printf("Team 1 wins the simulation with score %d - %d!\n", game_state.simulation_score.first, game_state.simulation_score.second);
  }
  else if (game_state.simulation_score.second > game_state.simulation_score.first)
  {
    printf("Team 2 wins the simulation with score %d - %d!\n", game_state.simulation_score.first, game_state.simulation_score.second);
  }
  else
  {
    printf("Simulation ended in a draw with sore %d - %d!\n", game_state.simulation_score.first, game_state.simulation_score.second);
  }
  // update_screen(game_state);

}

void announce_winner(Team *team, int (*to_team_fd)[2])
{
  for (int i = 0; i < team->size; i++)
  {
    int is_winner = 1;
    write(to_team_fd[i][1], &is_winner, sizeof(is_winner));
    kill(team->players[i].pid, SIGUSR1);
  }
}

void announce_loser(Team *team, int (*to_team_fd)[2])
{
  for (int i = 0; i < team->size; i++)
  {
    int is_winner = 0;
    write(to_team_fd[i][1], &is_winner, sizeof(is_winner));
    kill(team->players[i].pid, SIGUSR1);
  }
}