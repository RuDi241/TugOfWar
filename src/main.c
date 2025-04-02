#include "../include/error_codes.h"
#include "../include/game_config.h"
#include "../include/team_config.h"
#include "../include/team.h"
#include "../include/game_state.h"
#include "../include/player.h"
#include <sys/time.h>
#include <signal.h>

int main(int argc, char *argv[]) {
  struct timeval start_simulation, start_round, now;

  GameConfig game_config;
  GameState game_state;
  TeamConfig team1_config;
  TeamConfig team2_config;
  Team team1;
  Team team2;  

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


  //1- while curr_simulation time less than total_sim_time
  //2- arrange player
  //3- start new round
  //4- wait for a second
  //5- send signal to receive data from each player
  //6- if desired score reached ==> end round ==> announce winner ==> update game_state ==> back to 2
  //7- if time's up ==> end round ==> see which team got the higher score ==> announce winner ==> update game_state ==> back to 2
  //8- if a team won two consecutive games ==> end game ==> that team won!
  //9- 
  
  gettimeofday(&start_simulation, NULL); 
  long elapsed_ms_from_sim_time;
  while (1) {
      elapsed_ms_from_sim_time = (now.tv_sec - start_simulation.tv_sec);
                    
      if (elapsed_ms_from_sim_time >= game_state.simulation_time) {
          break; 
      }
      
      //read energies from players
      receive_data_from_team(&team1);
      receive_data_from_team(&team2);

      arrange_team(&team1);
      arrange_team(&team2);

      send_position_to_team(&team1);
      send_position_to_team(&team2);

      int round_sum = 0;

      gettimeofday(&start_round, NULL); 
      long elapsed_ms_from_round_time;
      
      while(1){
          //checks for round time
          elapsed_ms_from_round_time = (now.tv_sec - start_round.tv_sec) * 1000; // Seconds to ms
          if (elapsed_ms_from_round_time >= game_state.round_period) {
              break; 
          }
          
          if(elapsed_ms_from_round_time%1000 == 0){
              for(int i = 0; i< team1.size; i++){
                kill(game_state.team1.players[i].pid, SIGUSR2);
              }

              for(int i = 0; i< team2.size; i++){
                kill(game_state.team2.players[i].pid, SIGUSR2);
              }

              receive_data_from_team(&team1);
              receive_data_from_team(&team2);

              int sum_t1 = 0;
              int sum_t2 = 0;

              for(int i = 0; i < team1.size; i++)
                  sum_t1 += team1.players[i].energy;

              for(int i = 0; i < team2.size; i++)
                  sum_t2 += team2.players[i].energy;

              round_sum += (sum_t1 - sum_t2);

              /**
               * TODO: 1- IMPLEMENT WINNING LOGIC
               *       2- CHECK MAX SCORE IN GAME-STATE (I SO NOT KNOW WHERE IT IS OR HOW IT SHOULD BE ADDED)
               * 
               */

            }

      }

      


  }


  return 0;
}
