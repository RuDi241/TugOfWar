#include "../include/game_state.h"
#include <stdlib.h>
#define SEED 42


int init_game_state(GameState *game_state, GameConfig *game_config, Team *team1, Team *team2){

    srand(SEED);
    // LET HANNN DECIDE
    game_state->in_round = 'o';
    game_state->simulation_time = game_config->max_simulation_time/(rand()%5 + 1);
    game_state->number_of_rounds = game_state->simulation_time/(rand()%10 + 1);
    game_state->round_period = game_state->simulation_time / game_state->number_of_rounds;
    game_state->round_score = 0;
    game_state->simulation_score.first = 0;
    game_state->simulation_score.second = 0;
    game_state->simulation_start_time = 0;
    game_state->round_start_time = 0;

    return 0;
}
