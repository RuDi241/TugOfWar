#include "../../include/error_codes.h"
#include "../../include/game_config.h"
#include "../../include/team_config.h"
#include "../../include/team.h"
#include "../../include/game_state.h"
#include "../../include/player.h"
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  struct timeval now;

  GameConfig game_config;
  GameState game_state;
  TeamConfig team1_config;
  TeamConfig team2_config;

  if (read_game_config("./game_config.txt", &game_config))
    return CONFIG_ERROR;
  if (read_team_config("./team1_config.txt", &team1_config))
    return CONFIG_ERROR;
  if (read_team_config("./team2_config.txt", &team2_config))
    return CONFIG_ERROR;
  fprintf_game_config(stdout, &game_config);
  fprintf_team_config(stdout, &team1_config);
  fprintf_team_config(stdout, &team2_config);

  //init game state
  init_game_state(&game_state, &game_config, &team1_config, &team2_config);
  
  if(gettimeofday(&game_state.start_simulation_time, NULL) == -1){
    perror("gettimeofday failed");
    return TIMER_ERROR;
  } 

  while (game_state.current_simulation_time <= game_state.max_simulation_time || 
    game_state.number_of_rounds_played < game_state.max_number_of_rounds ||
    game_state.current_win_streak >= game_state.max_consecutive_wins) {

      if (gettimeofday(&now, NULL) == -1) {
        perror("gettimeofday failed");
        return TIMER_ERROR;
      }

      game_state.current_simulation_time = (now.tv_sec - game_state.start_simulation_time.tv_sec) * 1000 + (now.tv_usec - game_state.start_simulation_time.tv_usec) / 1000;
      
      //read energies from players
      receive_data_from_team(&game_state.team1);
      receive_data_from_team(&game_state.team2);

      arrange_team(&game_state.team1);
      arrange_team(&game_state.team2);

      send_position_to_team(&game_state.team1);
      send_position_to_team(&game_state.team2);

      if (gettimeofday(&game_state.start_round_time, NULL) == -1){
        perror("gettimeofday failed");
        return TIMER_ERROR;
      } 
      
      game_state.in_round = '1';
      while(game_state.current_round_time <= game_state.max_simulation_time){

          if (gettimeofday(&now, NULL) == -1) {
            perror("gettimeofday failed");
            return TIMER_ERROR;
          }
          //checks for round time
          game_state.current_round_time = (now.tv_sec - game_state.start_round_time.tv_sec) * 1000 + (now.tv_usec - game_state.start_round_time.tv_usec) / 1000; // Seconds to ms
          
          if(game_state.current_round_time % 1000 == 0){
              for(int i = 0; i< game_state.team1.size; i++){
                kill(game_state.team1.players[i].pid, SIGUSR2);
              }

              for(int i = 0; i< game_state.team2.size; i++){
                kill(game_state.team2.players[i].pid, SIGUSR2);
              }

              receive_data_from_team(&game_state.team1);
              receive_data_from_team(&game_state.team2);

              for(int i = 0; i < game_state.team1.size; i++)
                  game_state.team1_sum += game_state.team1.players[i].energy;

              for(int i = 0; i < game_state.team1.size; i++)
                  game_state.team2_sum += game_state.team1.players[i].energy;

              game_state.round_score += (game_state.team1_sum - game_state.team2_sum);

              printf("Round sum: %d\n", game_state.round_score);
              printf("Team 1 total energy: %d\n", game_state.team1_sum);
              printf("Team 2 total energy: %d\n", game_state.team2_sum);


              if(abs(game_state.round_score) >= game_state.score_gap_to_win){
                break;
              }

            }

      }

      end_round_protocol(&game_state);
  }

  end_simulation_protocol(&game_state);

  destroy_game_state(&game_state);


  return 0;
}
