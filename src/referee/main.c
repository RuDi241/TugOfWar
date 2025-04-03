#include "../../include/error_codes.h"
#include "../../include/game_config.h"
#include "../../include/team_config.h"
#include "../../include/team.h"
#include "../../include/game_state.h"
#include "../../include/player.h"
#include <sys/time.h>
#include <signal.h>

int main(int argc, char *argv[]) {
  struct timeval now, start_simulation;

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
  
  gettimeofday(&game_state.start_simulation_time, NULL); 

  while (game_state.current_simulation_time <= game_state.max_simulation_time || game_state.number_of_rounds_played < game_state.max_number_of_rounds) {

      game_state.current_simulation_time = (now.tv_usec - game_state.start_simulation_time.tv_usec) / 1000;
      
      //read energies from players
      receive_data_from_team(&team1);
      receive_data_from_team(&team2);

      arrange_team(&team1);
      arrange_team(&team2);

      send_position_to_team(&team1);
      send_position_to_team(&team2);

      gettimeofday(&game_state.start_round_time, NULL); 
      
      while(game_state.current_round_time <= game_state.max_simulation_time){
          //checks for round time
          game_state.current_round_time = (now.tv_usec - game_state.start_round_time.tv_usec) / 1000; // Seconds to ms
          
          if(game_state.current_round_time % 1000 == 0){
              for(int i = 0; i< team1.size; i++){
                kill(game_state.team1.players[i].pid, SIGUSR2);
              }

              for(int i = 0; i< team2.size; i++){
                kill(game_state.team2.players[i].pid, SIGUSR2);
              }

              receive_data_from_team(&team1);
              receive_data_from_team(&team2);

              for(int i = 0; i < team1.size; i++)
                  game_state.team1_sum += team1.players[i].energy;

              for(int i = 0; i < team2.size; i++)
                  game_state.team2_sum += team2.players[i].energy;

              game_state.round_score += (game_state.team1_sum - game_state.team2_sum);

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
