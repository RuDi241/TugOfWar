#include "renderer.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

int main() {
  // Initialize GLFW
  if (!glfwInit()) {
    printf("Failed to initialize GLFW\n");
    return -1;
  }

  // Configure OpenGL context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

  // Initialize our renderers
  initRenderer(windowWidth, windowHeight);
  setWindowSize(windowWidth, windowHeight);

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
    Rectangle rect = {0.0f, 0.5f, 0.3f, 0.2f, 1.0f, 0.0f, 0.0f, 1.0f};
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
    renderText("Hello, World!", 50.0f, 50.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    renderText("OpenGL Text with FreeType", 25.0f, 100.0f, 0.8f, 0.5f, 0.8f,
               0.2f, 1.0f);

    // Show FPS
    static double lastTime = 0;
    static int frameCount = 0;
    double currentTime = glfwGetTime();
    frameCount++;

    if (currentTime - lastTime >= 1.0) {
      char fpsText[32];
      sprintf(fpsText, "FPS: %d", frameCount);
      frameCount = 0;
      lastTime = currentTime;
      renderText(fpsText, 10.0f, 25.0f, 0.7f, 1.0f, 1.0f, 0.0f, 1.0f);
    }

    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  shutdownTextRenderer();
  shutdownRenderer();
  glfwTerminate();

  return 0;
}
