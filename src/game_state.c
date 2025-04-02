#include "../include/game_state.h"
#include <stdlib.h>

int init_game_state(GameState *game_state, GameConfig *game_config, TeamConfig *team1_config, TeamConfig *team2_config){
    game_state->in_round = 'o';

    game_state->current_simulation_time = 0;
    game_state->max_simulation_time = game_config->max_simulation_time;

    game_state->current_round_time = 0;

    game_state->number_of_rounds_played = 0;
    game_state->max_number_of_rounds = game_config->max_number_of_rounds;

    game_state->round_score = 0;
    game_state->simulation_score.first = 0;
    game_state->simulation_score.second = 0;

    game_state->team1_sum = 0;
    game_state->team2_sum = 0;

    make_team(&game_state->team1, team1_config);
    make_team(&game_state->team2, team2_config);
    return 0;
}
