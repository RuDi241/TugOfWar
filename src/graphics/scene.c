#include "scene.h"
#include "renderer.h"
#include <stdio.h>
#include <player.h>
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
           display->max_consecutive_wins, display->max_number_of_rounds,
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

void drawTeam(const Team *team, float direction) {
  for (int i = 0; i < team->size; ++i) {
    // Calculate x position for each player based on the direction and spacing
    float x = direction * (-1.0f + 0.1f + i * PLAYER_SPACING);
    float y = 0; // Vertical position stays the same for all players

    // Draw the player with a number
    drawPlayer(x, y, &team->players[i]);
  }
}

void drawRope(int score, int max_score) {
  // Draw base rope
  Rectangle rope = {0.0f, -0.05, 2.0f, ROPE_THICKNESS, 0.8f, 0.6f, 0.4f, 1.0f};
  drawRectangle(rope);

  // Red marker position along x, normalize score to [-0.5, 0.5]
  float offset = (float)score / max_score;
  if (offset > 1.0f)
    offset = 1.0f;
  if (offset < -1.0f)
    offset = -1.0f;

  Rectangle mark = {offset, -0.05, ROPE_MARK_WIDTH, ROPE_THICKNESS, 1.0f, 0.0f,
                    0.0f,   1.0f};
  drawRectangle(mark);
}

void drawRoundFinishMessage(const Display *display) {
  if (!display->in_round) {
    char buf[64];
    snprintf(buf, sizeof(buf), "Team %d won the round",
             display->previous_round_result + 1);

    renderTextCenter(buf, 0.0f, 0.2f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
  }
}

void drawScene(const Display *display) {
  drawDashboard(display);
  // Players
  drawTeam(&display->team1, 1.0f);  // Left side
  drawTeam(&display->team2, -1.0f); // Right side (mirror)
  // TODO: Set max round score
  drawRope(display->round_score, 10.0f);
  drawRoundFinishMessage(display);
}
