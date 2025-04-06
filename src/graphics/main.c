#include "renderer.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

int main() {
  // Initialize GLFW
  if (!glfwInit()) {
    printf("Failed to initialize GLFW\n");
    return -1;
  }

  // Configure OpenGL context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create window
  int windowWidth = 800;
  int windowHeight = 600;
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
  initTextRenderer("fonts/arial.ttf", 24);
  printf("Font loaded successfully\n");

  // Enable blending for transparent text rendering
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    // Handle window resize
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    setWindowSize(width, height);

    // Clear the screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw shapes
    Rectangle rect = {-1.0f, -1.0f, 0.3f, 0.2f, 1.0f, 0.0f, 0.0f, 1.0f};
    drawRectangle(rect);

    Circle circle = {-0.5f, 0.0f, 0.2f, 0.0f, 1.0f, 0.0f, 1.0f, 32};
    drawCircle(circle);

    Triangle triangle = {
        0.5f, -0.5f,            // First vertex
        0.7f, -0.2f,            // Second vertex
        0.3f, -0.2f,            // Third vertex
        0.0f, 0.0f,  1.0f, 1.0f // RGBA color
    };
    drawTriangle(triangle);

    // Render text - positions are in screen coordinates (pixels)
    renderText("Fuck OpenGL!", 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    // Swap buffers and poll events
    // debugTextRendering();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  shutdownTextRenderer();
  shutdownRenderer();
  glfwTerminate();

  return 0;
}
