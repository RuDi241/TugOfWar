#include "../../include/error_codes.h"
#include "../../include/game_config.h"
#include "../../include/team_config.h"
#include "../../include/team.h"
#include "../../include/game_state.h"
#include "../../include/player.h"
#include "../../include/game_interface_communication.h"
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int read_configurations(GameConfig *game_config, TeamConfig *team1_config,
                        TeamConfig *team2_config);

int main(int argc, char *argv[]) {
  struct timeval now;

  GameConfig game_config;
  GameState game_state;
  TeamConfig team1_config;
  TeamConfig team2_config;
  Display display;
  GraphicsCommunicationInfo graphicsCommunicationInfo;

  int exit_code =
      read_configurations(&game_config, &team1_config, &team2_config);
  if (exit_code != 0) {
    return exit_code; // Exit with the error code
  }

  // init game state
  init_game_state(&game_state, &game_config, &team1_config, &team2_config);
  init_graphics_process(&graphicsCommunicationInfo, &display, &game_state);

  sleep(1); // Give some time for the graphics process to start

  game_state.in_simulation = '1';
  if (gettimeofday(&game_state.start_simulation_time, NULL) == -1) {
    perror("gettimeofday failed");
    return TIMER_ERROR;
  }

  // SIMULATION LOOP
  time_t max_simulation_time_ms = game_state.max_simulation_time * 1000LL;

  while (game_state.current_simulation_time <= max_simulation_time_ms &&
         game_state.number_of_rounds_played < game_state.max_number_of_rounds &&
         game_state.current_win_streak < game_state.max_consecutive_wins) {

    if (gettimeofday(&now, NULL) == -1) {
      perror("gettimeofday failed");
      return TIMER_ERROR;
    }

    game_state.current_simulation_time =
        (now.tv_sec - game_state.start_simulation_time.tv_sec) * 1000 +
        (now.tv_usec - game_state.start_simulation_time.tv_usec) / 1000;

    // read energies from players
    receive_data_from_team(&game_state.team1);
    receive_data_from_team(&game_state.team2);

    arrange_team(&game_state.team1);
    arrange_team(&game_state.team2);

    send_position_to_team(&game_state.team1);
    send_position_to_team(&game_state.team2);

    if (gettimeofday(&game_state.start_round_time, NULL) == -1) {
      perror("gettimeofday failed");
      return TIMER_ERROR;
    }

    game_state.in_round = '1';

    // update screen
    update_screen(&game_state, STILL_PLAYING, SIM_RES_DRAW,
                  &graphicsCommunicationInfo, &display);

    // ROUND LOOP
    time_t max_round_time_ms = game_state.max_simulation_time * 1000LL;
    time_t round_time_at_last_measure = 0;

    while (game_state.current_round_time <= max_round_time_ms && game_state.current_simulation_time <= max_simulation_time_ms) {

      if (gettimeofday(&now, NULL) == -1) {
        perror("gettimeofday failed");
        return TIMER_ERROR;
      }
      // checks for round time
      game_state.current_round_time =
          (now.tv_sec - game_state.start_round_time.tv_sec) * 1000 +
          (now.tv_usec - game_state.start_round_time.tv_usec) /
              1000; // Seconds to ms
      game_state.current_simulation_time =
          (now.tv_sec - game_state.start_simulation_time.tv_sec) * 1000 +
          (now.tv_usec - game_state.start_simulation_time.tv_usec) / 1000;
      
      if (game_state.current_round_time - round_time_at_last_measure >= 1000) {
        round_time_at_last_measure = game_state.current_round_time;

        for (int i = 0; i < game_state.team1.size; i++) {
          if (kill(game_state.team1.players[i].pid, SIGUSR2) == -1) {
            perror("Error sending SIGUSR2 to team 1 player");
            return SIGNAL_ERROR;
          }
        }

        for (int i = 0; i < game_state.team2.size; i++) {
          if(kill(game_state.team2.players[i].pid, SIGUSR2) == -1) {
            perror("Error sending SIGUSR2 to team 2 player");
            return SIGNAL_ERROR;
          }
        }

        receive_data_from_team(&game_state.team1);
        receive_data_from_team(&game_state.team2);

        for (int i = 0; i < game_state.team1.size; i++)
          game_state.team1_sum += game_state.team1.players[i].energy;

        for (int i = 0; i < game_state.team2.size; i++)
          game_state.team2_sum += game_state.team2.players[i].energy;

        game_state.round_score += (game_state.team1_sum - game_state.team2_sum);

        // update screen
        update_screen(&game_state, STILL_PLAYING, SIM_RES_DRAW,
                      &graphicsCommunicationInfo, &display);

        printf("Round sum: %d\n", game_state.round_score);
        printf("Team 1 total energy: %d\n", game_state.team1_sum);
        printf("Team 2 total energy: %d\n", game_state.team2_sum);

        if (abs(game_state.round_score) >= game_state.score_gap_to_win) {
          sleep(1); // Give some time for the graphics process to finish
          break;
        }
      }
    }

    end_round_protocol(&game_state);
    update_screen(&game_state, STILL_PLAYING, SIM_RES_DRAW,
                  &graphicsCommunicationInfo, &display);
  }

  sleep(1); // Give some time for the graphics process to finish

  end_simulation_protocol(&game_state);
  update_screen(&game_state, game_state.simulation_winning_method,
                game_state.simulation_winner, &graphicsCommunicationInfo,
                &display);

  destroy_game_state(&game_state);
  sleep(1); // Give some time for the graphics process to finish
  destroy_graphics_process(&graphicsCommunicationInfo);

  return 0;
}

int read_configurations(GameConfig *game_config, TeamConfig *team1_config,
                        TeamConfig *team2_config) {
  if (read_game_config("./game_config.txt", game_config)) {
    fprintf(stderr, "Error reading game configuration.\n");
    return CONFIG_ERROR;
  }
  if (read_team_config("./team1_config.txt", team1_config)) {
    fprintf(stderr, "Error reading team 1 configuration.\n");
    return CONFIG_ERROR;
  }
  if (read_team_config("./team2_config.txt", team2_config)) {
    fprintf(stderr, "Error reading team 2 configuration.\n");
    return CONFIG_ERROR;
  }
  fprintf_game_config(stdout, game_config);
  fprintf_team_config(stdout, team1_config);
  fprintf_team_config(stdout, team2_config);
  return 0; // Success
}
