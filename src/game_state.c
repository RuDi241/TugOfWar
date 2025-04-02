#include "../include/game_state.h"
#include <stdlib.h>

int init_game_state(GameState *game_state, GameConfig *game_config, TeamConfig *team1_config, TeamConfig *team2_config){


    game_state->in_round = 'o';
    game_state->simulation_time = game_config->max_simulation_time;
    game_state->number_of_rounds = game_state->simulation_time;//TODO: add to game_config
    game_state->round_period = game_state->simulation_time / game_state->number_of_rounds; //TODO: DELETE
    game_state->round_score = 0;
    game_state->simulation_score.first = 0;
    game_state->simulation_score.second = 0;
    game_state->simulation_start_time = 0;
    game_state->round_start_time = 0;
    make_team(&game_state->team1, team1_config);
    make_team(&game_state->team2, team2_config);
    return 0;
}
