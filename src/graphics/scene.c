#include "scene.h"
#include "game_state.h"
#include "renderer.h"
#include <stdio.h>
#include <player.h>
#include <game_interface_communication.h>
// Constants
#define PLAYER_RADIUS 0.1f
#define PLAYER_SPACING 0.1f
#define ROPE_THICKNESS 0.02f
#define ROPE_MARK_WIDTH 0.05f

void drawDashboard(const Display *display) {
  char buffer[128];

  snprintf(buffer, sizeof(buffer), "Score: %d - %d",
           display->simulation_score.first, display->simulation_score.second);
  renderTextCenter(buffer, 0, 0.85f, 1.0f, 1, 1, 1, 1);

  snprintf(buffer, sizeof(buffer), "Time: %lds / %lds",
           display->current_simulation_time, display->max_simulation_time);
  renderTextLeft(buffer, -0.95f, 0.90f, 0.5f, 1, 1, 1, 1);

  snprintf(buffer, sizeof(buffer), "Round: %d / %d",
           display->number_of_rounds_played, display->max_number_of_rounds);
  renderTextLeft(buffer, -0.95f, 0.8f, 0.5f, 1, 1, 1, 1);

  snprintf(buffer, sizeof(buffer), "Consecutive Rounds Won: %d/%d by team %d",
           display->current_win_streak, display->max_consecutive_wins,
           display->previous_round_result);
  renderTextLeft(buffer, -0.95f, 0.7f, 0.5f, 1, 1, 1, 1);
}

void drawPlayer(float x, float y, Player *player) {
  // Draw the player's circle
  Circle playerCircle = {x, y, PLAYER_RADIUS, 0.2f, 0.6f, 1.0f, 1.0f, 32};
  if (player->fall_timeout > 0) {
    playerCircle.r = 1.0f;
    playerCircle.g = 1.0f;
    playerCircle.b = 0.0f;
  }
  drawCircle(playerCircle);

  // Render the player number
  char buf[64];
  snprintf(buf, sizeof(buf), "%d", player->pid);

  renderTextCenter(buf, x, y + PLAYER_RADIUS + 0.02f, 0.3f, 1.0f, 1.0f, 1.0f,
                   1.0f);

  // Player energy
  snprintf(buf, sizeof(buf), "%d", player->energy);
  renderTextCenter(buf, x, y, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f);

  // Fall timeout
  if (player->fall_timeout) {
    snprintf(buf, sizeof(buf), "Ouch, I fell!");
    renderTextCenter(buf, x, y - 0.13f, 0.3f, 1.0f, 1.0f, 1.0f, 1.0f);

    snprintf(buf, sizeof(buf), "Getting up in: %ds", player->fall_timeout);

    renderTextCenter(buf, x, y - 0.17f, 0.3f, 1.0f, 1.0f, 1.0f, 1.0f);
  }
}

void drawTeam(const Team team, float direction) {
  for (int i = 0; i < team.size; ++i) {
    // Calculate x position for each player based on the direction and spacing
    float x = direction * (-1.0f + 0.1f + i * PLAYER_SPACING);
    float y = 0; // Vertical position stays the same for all players

    // Draw the player with a number
    drawPlayer(x, y, &team.players[i]);
  }
}

void drawRope(int score, int max_score) {
  // Draw base rope
  Rectangle rope = {0.0f, -0.05, 2.0f, ROPE_THICKNESS, 0.8f, 0.6f, 0.4f, 1.0f};
  drawRectangle(rope);

  // Red marker position along x, normalize score to [-0.5, 0.5]
  float offset = (float)-score / max_score;
  if (offset > 1.0f)
    offset = 1.0f;
  if (offset < -1.0f)
    offset = -1.0f;

  Rectangle mark = {offset, -0.05, ROPE_MARK_WIDTH, ROPE_THICKNESS, 1.0f, 0.0f,
                    0.0f,   1.0f};
  drawRectangle(mark);
}

void drawRoundFinishMessage(const Display *display) {
  char buf[64];
  if (!display->in_round && display->in_simulation) {
    if (display->previous_round_result == TEAM1_TEAM2_DRAW)
      snprintf(buf, sizeof(buf), "Draw");
    else
      snprintf(buf, sizeof(buf), "Team %d won the round",
               display->previous_round_result);

    renderTextCenter(buf, 0.0f, 0.2f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
  }

  if (!display->in_simulation) {

    if (display->simulation_winner == SIM_RES_DRAW)
      snprintf(buf, sizeof(buf), "Draw");
    else
      snprintf(buf, sizeof(buf), "Team %d won the game",
               display->simulation_winner);

    renderTextCenter(buf, 0.0f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    if (display->simulation_winning_method == CONSECUTIVE_WINS)
      snprintf(buf, sizeof(buf), "by consecutive wins rule.");
    else if (display->simulation_winning_method == WON_MORE_ROUNDS)
      snprintf(buf, sizeof(buf), "by consecutive winning more rounds.");

    renderTextCenter(buf, 0.0f, 0.2f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
  }
}

void drawScene(const Display *display) {
  drawDashboard(display);
  // Players
  drawTeam(display->team1, 1.0f);  // Right side
  drawTeam(display->team2, -1.0f); // Left side
  // TODO: Set max round score
  drawRope(display->round_score, display->score_gap_to_win);
  drawRoundFinishMessage(display);
}
