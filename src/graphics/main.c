#include "renderer.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "scene.h"
#include <stdio.h>
#include <error_codes.h>

static int pipe_descriptor;
int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "The program expects one read pipe descriptor.");
    exit(EXIT_FAILURE);
  }
  pipe_descriptor = atoi(argv[1]);

// sample display
#include <time.h> // for time_t

  // Dummy player arrays
  Player team1Players[4] = {
      {1234, 0, 100, 25, {0}, {0}},
      {1235, 1, 80, 0, {0}, {0}},
      {1236, 2, 90, 0, {0}, {0}},
      {1237, 3, 90, 0, {0}, {0}},
  };

  Player team2Players[4] = {
      {2234, 0, 95, 0, {0}, {0}},
      {2235, 1, 85, 0, {0}, {0}},
      {2236, 2, 100, 0, {0}, {0}},
      {223723234, 3, 200, 0, {0}, {0}},
  };

  // Wrap in Team structs
  Team team1 = {4, team1Players};
  Team team2 = {4, team2Players};

  // Fill Display struct
  Display display = {.in_round = 0,

                     .current_simulation_time = 30,
                     .max_simulation_time = 90,

                     .current_round_time = 10,

                     .number_of_rounds_played = 2,
                     .max_number_of_rounds = 5,

                     .round_score = -3, // Rope moves slightly toward team2
                     .simulation_score = {1, 1}, // 1:1

                     .team1 = team1,
                     .team2 = team2,

                     .team1_sum = 270, // Sum of energy or position
                     .team2_sum = 280,

                     .max_consecutive_wins = 2,
                     .previous_round_result = TEAM2_WIN,
                     .current_win_streak = 1,

                     .simulation_winning_method = CONSECUTIVE_WINS,
                     .simulation_winner = DRAW_};

  // Initialize GLFW
  if (!glfwInit()) {
    printf("Failed to initialize GLFW\n");
    return -1;
  }

  // Configure OpenGL context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create window
  int windowWidth = 1280;
  int windowHeight = 720;
  GLFWwindow *window =
      glfwCreateWindow(windowWidth, windowHeight, "Renderer Demo", NULL, NULL);
  if (!window) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  // Initialize GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    return -1;
  }
  setWindowSize(windowWidth, windowHeight);

  // Initialize renderers
  initRenderer(windowWidth, windowHeight);

  // Initialize text renderer with font path - make sure this file exists!
  printf("Loading font...\n");
  initTextRenderer("fonts/arial.ttf", 64);
  printf("Font loaded successfully\n");

  // Enable blending for transparent text rendering
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    setWindowSize(width, height);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    drawScene(&display); // <--- draw the scene here

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  // Clean up
  shutdownTextRenderer();
  shutdownRenderer();
  glfwTerminate();

  return 0;
}
