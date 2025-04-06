#define _XOPEN_SOURCE 500
#include <signal.h>
#include "game_interface_communication.h"
#include "renderer.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "scene.h"
#include <stdio.h>

void SIGUSR1_handler(int signum);

static int read_fd;
static Display display;

int main(int argc, char *argv[]) {

  // Check if the correct number of arguments is provided
  if (argc != 2) {
    printf("Usage: %s <read_fd>\n", argv[0]);
    return -1;
  }

  read_fd = atoi(argv[1]);
  if (read_fd < 0) {
    printf("Invalid read_fd: %d\n", read_fd);
    return -1;
  }

  if (sigset(SIGUSR1, SIGUSR1_handler) == SIG_ERR) {
    perror("signal can not set SIGUSR1");
    exit(SIGUSR1);
  }

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

    drawScene(&display);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  // Clean up
  shutdownTextRenderer();
  shutdownRenderer();
  destroy_display(&display);
  glfwTerminate();

  return 0;
}

void SIGUSR1_handler(int sig_num) {
  receive_data_from_referee(read_fd, &display);
  return;
}
