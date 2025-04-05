#ifndef GAME_INTERFACE_COMMUNICATION_H
#define GAME_INTERFACE_COMMUNICATION_H

#include "game_state.h"
enum SimulationWinningMethod {
    CONSECUTIVE_WINS,
    WON_MORE_ROUNDS
};

enum SimulationWinner {
    DRAW,
    TEAM_1,
    TEAM_2  
};
typedef struct Display {
    char in_round;

    time_t current_simulation_time; 
    time_t max_simulation_time;

    time_t current_round_time;

    int number_of_rounds_played;
    int max_number_of_rounds;

    int round_score;
    PairIntInt simulation_score;
    
    Team team1;
    Team team2;
  
    int team1_sum;
    int team2_sum;

    int max_consecutive_wins;
    enum PreviousRoundResult previous_round_result;
    int current_win_streak;

    enum SimulationWinningMethod simulation_winning_method;
    enum SimulationWinner simulation_winner;
} Display;
#endif