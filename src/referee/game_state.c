#include "../../include/game_state.h"
#include <stdlib.h>

int init_game_state(GameState *game_state, GameConfig *game_config, TeamConfig *team1_config, TeamConfig *team2_config){
    game_state->in_round = '0';

    game_state->current_simulation_time = 0;
    game_state->max_simulation_time = game_config->max_simulation_time;

    game_state->current_round_time = 0;

    game_state->number_of_rounds_played = 0;
    game_state->max_number_of_rounds = game_config->max_number_of_rounds;

    game_state->round_score = 0;
    game_state->simulation_score.first = 0;
    game_state->simulation_score.second = 0;
    game_state->max_consecutive_wins = game_config->max_consecutive_wins;

    game_state->team1_sum = 0;
    game_state->team2_sum = 0;

    game_state->score_gap_to_win = game_config->score_gap_to_win;
    game_state->current_win_streak = 0;
    game_state->previous_round_result = DRAW;

    make_team(&game_state->team1, team1_config);
    make_team(&game_state->team2, team2_config);

    return 0;
}

int decide_round_winner(GameState *game_state){
    if(game_state->round_score > 0){
        game_state->simulation_score.first++;
        announce_result_to_team(&game_state->team1, 1);
        announce_result_to_team(&game_state->team2, 0);
    }else if(game_state->round_score < 0){
        game_state->simulation_score.second++;
        announce_result_to_team(&game_state->team1, 0);
        announce_result_to_team(&game_state->team2, 1);
    }else{
        //Draw case-TODO: DISCUSS THE FLAG FOR DRAW STATE 
        game_state->simulation_score.first++;
        game_state->simulation_score.second++;
        announce_result_to_team(&game_state->team1, 2);
        announce_result_to_team(&game_state->team2, 2);
    }
    return 0;
}

int decide_win_streak(GameState *game_state){
    if(game_state->previous_round_result == TEAM1_WIN && game_state->round_score > 0){
        game_state->current_win_streak++;
      }else if(game_state->previous_round_result == TEAM2_WIN && game_state->round_score < 0){
        game_state->current_win_streak++;
      }else{
        game_state->current_win_streak = 0;
      }
      return 0;
}

int choose_prev_round_winner(GameState *game_state){
    if(game_state->round_score > 0)
        game_state->previous_round_result = TEAM1_WIN;
    else if(game_state->round_score < 0)
        game_state->previous_round_result = TEAM2_WIN;
    else
        game_state->previous_round_result = DRAW;
    return 0;
}


int reset_round(GameState *game_state){
    game_state->current_round_time = 0;
    game_state->round_score = 0;
    game_state->team1_sum = 0;
    game_state->team2_sum = 0;
    return 0;
}

int end_round_protocol(GameState *game_state){
    game_state->number_of_rounds_played++;

    //decide who won round
    decide_round_winner(game_state);

    //decide win steak
    decide_win_streak(game_state);
  
    //choose previouse round winner
    choose_prev_round_winner(game_state);

    //reset the round data for the new round
    reset_round(game_state);

    game_state->in_round = '0'; 

    return 0;
}

int end_simulation_protocol(GameState *game_state){
    if (game_state->current_win_streak >= game_state->max_consecutive_wins){
        if (game_state->previous_round_result == TEAM1_WIN)
          printf("Team1 has won the match by winning %d consecutive rounds!\n", game_state->current_win_streak);
        else
          printf("Team2 has won the match by winning %d consecutive rounds!\n", game_state->current_win_streak);
    }else if (game_state->simulation_score.first > game_state->simulation_score.second){
          printf("Team 1 wins the simulation with score %d - %d!\n", game_state->simulation_score.first, game_state->simulation_score.second);
    }else if (game_state->simulation_score.first < game_state->simulation_score.second){
        printf("Team 2 wins the simulation with score %d - %d!\n", game_state->simulation_score.first, game_state->simulation_score.second);
    }else{
        printf("Simulation ended in a draw with sore %d - %d!\n", game_state->simulation_score.first, game_state->simulation_score.second);
    }

    return 0;
}

int destroy_game_state(GameState *game_state){
    destroy_team(&game_state->team1);
    destroy_team(&game_state->team2);

    return 0;
}