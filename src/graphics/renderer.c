#include "renderer.h"
#include "shader_utils.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Global variables for the renderer
static GLuint shaderProgram;
static GLuint VAO, VBO;

// Variables for shapes
static GLuint circleVAO, circleVBO;
static GLuint triangleVAO, triangleVBO;

// Text rendering variables
static GLuint textShaderProgram;
static GLuint textVAO, textVBO;
static Character characters[128]; // Store ASCII characters
static FT_Library ft;
static FT_Face face;
static int windowWidth = 800;  // Default window size
static int windowHeight = 600; // Default window size

// Helper function prototypes
static void setupShaders();
static void setupBuffers();

void initRenderer(int width, int height) {
  // Store window dimensions
  windowWidth = width;
  windowHeight = height;

  // Setup shaders
  setupShaders();

  // Setup buffers for shapes
  setupBuffers();
}

void setWindowSize(int width, int height) {
  // Store window dimensions for text rendering projection matrix
  windowWidth = width;
  windowHeight = height;
}

static void setupShaders() {
  // Create shader program for basic shapes
  const char *vertexShaderSrc =
      "#version 330 core\n"
      "layout (location = 0) in vec2 aPos;\n"
      "uniform vec2 offset;\n"
      "uniform vec2 scale;\n"
      "void main() {\n"
      "   vec2 scaled = aPos * scale;\n"
      "   gl_Position = vec4(scaled + offset, 0.0, 1.0);\n"
      "}\n";

  const char *fragmentShaderSrc = "#version 330 core\n"
                                  "out vec4 FragColor;\n"
                                  "uniform vec4 color;\n"
                                  "void main() {\n"
                                  "   FragColor = color;\n"
                                  "}\n";

  shaderProgram = createShaderProgram(vertexShaderSrc, fragmentShaderSrc);
}

static void setupBuffers() {
  // Setup rectangle rendering with a unit quad
  GLfloat quadVertices[] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f};
  GLuint indices[] = {0, 1, 2, 2, 3, 0};

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  GLuint EBO;
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
                        (void *)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  // Setup triangle rendering with a dynamic buffer
  glGenVertexArrays(1, &triangleVAO);
  glGenBuffers(1, &triangleVBO);
  glBindVertexArray(triangleVAO);
  glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
                        (void *)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  // Setup circle rendering (vertices will be generated dynamically)
  glGenVertexArrays(1, &circleVAO);
  glGenBuffers(1, &circleVBO);
}

void drawRectangle(Rectangle rect) {
  glUseProgram(shaderProgram);
  glBindVertexArray(VAO);

  // Set uniforms for position, size and color
  GLint offsetLoc = glGetUniformLocation(shaderProgram, "offset");
  GLint scaleLoc = glGetUniformLocation(shaderProgram, "scale");
  GLint colorLoc = glGetUniformLocation(shaderProgram, "color");

  glUniform2f(offsetLoc, rect.x, rect.y);
  glUniform2f(scaleLoc, rect.width, rect.height);
  glUniform4f(colorLoc, rect.r, rect.g, rect.b, rect.a);

  // Draw rectangle using indexed rendering
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
  glUseProgram(0);
}

void drawTriangle(Triangle triangle) {
  glUseProgram(shaderProgram);
  glBindVertexArray(triangleVAO);

  // Create and upload triangle vertices
  GLfloat vertices[] = {triangle.x1, triangle.y1, triangle.x2,
                        triangle.y2, triangle.x3, triangle.y3};

  glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

  // Set color uniform
  GLint colorLoc = glGetUniformLocation(shaderProgram, "color");
  glUniform4f(colorLoc, triangle.r, triangle.g, triangle.b, triangle.a);

  // Use identity transformation for direct vertex coordinates
  GLint offsetLoc = glGetUniformLocation(shaderProgram, "offset");
  GLint scaleLoc = glGetUniformLocation(shaderProgram, "scale");
  glUniform2f(offsetLoc, 0.0f, 0.0f);
  glUniform2f(scaleLoc, 1.0f, 1.0f);

  // Draw triangle
  glDrawArrays(GL_TRIANGLES, 0, 3);

  glBindVertexArray(0);
  glUseProgram(0);
}

void drawCircle(Circle circle) {
  glUseProgram(shaderProgram);

  // Use default segment count if not specified
  int segments = circle.segments > 0 ? circle.segments : 32;

  // Allocate memory for circle vertices (center + points on circumference)
  GLfloat *vertices = (GLfloat *)malloc((segments + 2) * 2 * sizeof(GLfloat));

  // Center vertex
  vertices[0] = 0.0f;
  vertices[1] = 0.0f;

  // Generate vertices around the circle
  for (int i = 0; i <= segments; i++) {
    float angle = 2.0f * M_PI * i / segments;
    // We need to account for the aspect ratio to make it a proper circle
    float aspectRatio = (float)windowWidth / (float)windowHeight;
    vertices[(i + 1) * 2] = cosf(angle);
    vertices[(i + 1) * 2 + 1] = sinf(angle) * aspectRatio;
  }

  // Upload vertices to GPU
  glBindVertexArray(circleVAO);
  glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
  glBufferData(GL_ARRAY_BUFFER, (segments + 2) * 2 * sizeof(GLfloat), vertices,
               GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
                        (void *)0);
  glEnableVertexAttribArray(0);

  // Set uniforms for position, size and color
  GLint offsetLoc = glGetUniformLocation(shaderProgram, "offset");
  GLint scaleLoc = glGetUniformLocation(shaderProgram, "scale");
  GLint colorLoc = glGetUniformLocation(shaderProgram, "color");

  glUniform2f(offsetLoc, circle.x, circle.y);
  glUniform2f(scaleLoc, circle.radius, circle.radius);
  glUniform4f(colorLoc, circle.r, circle.g, circle.b, circle.a);

  // Draw circle as a triangle fan
  glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);

  // Clean up
  free(vertices);
  glBindVertexArray(0);
  glUseProgram(0);
}

int initTextRenderer(const char *fontPath, int fontSize) {
  // Create shader program specifically for text rendering
  const char *textVertexShaderSrc =
      "#version 330 core\n"
      "layout (location = 0) in vec4 vertex;\n" // <vec2 pos, vec2 tex>
      "out vec2 TexCoords;\n"
      "uniform mat4 projection;\n"
      "void main() {\n"
      "    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
      "    TexCoords = vertex.zw;\n"
      "}\n";

  const char *textFragmentShaderSrc =
      "#version 330 core\n"
      "in vec2 TexCoords;\n"
      "out vec4 FragColor;\n"
      "uniform sampler2D text;\n"
      "uniform vec4 textColor;\n"
      "void main() {\n"
      "    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
      "    FragColor = textColor * sampled;\n"
      "}\n";

  textShaderProgram =
      createShaderProgram(textVertexShaderSrc, textFragmentShaderSrc);

  // Initialize FreeType library
  if (FT_Init_FreeType(&ft)) {
    printf("ERROR: Could not initialize FreeType library\n");
    return -1;
  }

  // Load font face
  if (FT_New_Face(ft, fontPath, 0, &face)) {
    printf("ERROR: Failed to load font at %s\n", fontPath);
    return -1;
  }

  // Set font size
  FT_Set_Pixel_Sizes(face, 0, fontSize);

  // Disable byte-alignment restriction for proper texture loading
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Load and store first 128 ASCII characters
  for (unsigned char c = 0; c < 128; c++) {
    // Load character glyph
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      printf("ERROR: Failed to load glyph for character %c\n", c);
      continue;
    }

    // Generate texture for this character
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Copy glyph bitmap to texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                 face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                 face->glyph->bitmap.buffer);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Store character metrics for later use
    Character character = {texture,
                           face->glyph->bitmap.width,
                           face->glyph->bitmap.rows,
                           face->glyph->bitmap_left,
                           face->glyph->bitmap_top,
                           face->glyph->advance.x};
    characters[c] = character;
  }
  glBindTexture(GL_TEXTURE_2D, 0);

  // Set text texture uniform once (doesn't change)
  glUseProgram(textShaderProgram);
  glUniform1i(glGetUniformLocation(textShaderProgram, "text"), 0);
  glUseProgram(0);

  // Create VAO/VBO for text rendering
  glGenVertexArrays(1, &textVAO);
  glGenBuffers(1, &textVBO);
  glBindVertexArray(textVAO);
  glBindBuffer(GL_ARRAY_BUFFER, textVBO);

  // Allocate buffer space for dynamic text rendering
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

  // Configure vertex attributes for text rendering
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return 0;
}

void renderText(const char *text, float x, float y, float scale, float r,
                float g, float b, float a) {
  // Activate shader and set text color
  glUseProgram(textShaderProgram);
  glUniform4f(glGetUniformLocation(textShaderProgram, "textColor"), r, g, b, a);

  // Create orthographic projection matrix for screen-space rendering
  // This maps from pixel coordinates to normalized device coordinates
  float projection[16] = {2.0f / windowWidth,
                          0.0f,
                          0.0f,
                          -1.0f,
                          0.0f,
                          -2.0f / windowHeight,
                          0.0f,
                          1.0f,
                          0.0f,
                          0.0f,
                          -1.0f,
                          0.0f,
                          0.0f,
                          0.0f,
                          0.0f,
                          1.0f};

  // Set projection matrix uniform
  glUniformMatrix4fv(glGetUniformLocation(textShaderProgram, "projection"), 1,
                     GL_FALSE, projection);

  // Activate texture unit for text rendering
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(textVAO);

  // Enable blending for proper text rendering
  GLboolean blendWasEnabled = glIsEnabled(GL_BLEND);
  if (!blendWasEnabled) {
    glEnable(GL_BLEND);
  }

  // Save original blend function
  GLint originalSrcFactor, originalDstFactor;
  glGetIntegerv(GL_BLEND_SRC_ALPHA, &originalSrcFactor);
  glGetIntegerv(GL_BLEND_DST_ALPHA, &originalDstFactor);

  // Set appropriate blend function for text
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Iterate through and render each character in the text
  float xpos = x;
  float ypos = y;

  for (const char *c = text; *c; c++) {
    Character ch = characters[(unsigned char)*c];

    if (ch.textureID == 0)
      continue; // Skip if glyph wasn't loaded

    // Calculate character position and size
    float char_x = xpos + ch.bearingX * scale;
    float char_y = ypos - (ch.height - ch.bearingY) * scale;
    float w = ch.width * scale;
    float h = ch.height * scale;

    // Skip rendering if character has no width or height
    if (w <= 0 || h <= 0) {
      // Advance cursor for space characters and other non-visible glyphs
      xpos += (ch.advance >> 6) * scale;
      continue;
    }

    // Create quad vertices for this character with correct texture coordinates
    float vertices[6][4] = {
        {char_x, char_y + h, 0.0f, 0.0f}, // top left
        {char_x, char_y, 0.0f, 1.0f},     // bottom left
        {char_x + w, char_y, 1.0f, 1.0f}, // bottom right

        {char_x, char_y + h, 0.0f, 0.0f},    // top left
        {char_x + w, char_y, 1.0f, 1.0f},    // bottom right
        {char_x + w, char_y + h, 1.0f, 0.0f} // top right
    };

    // Bind character texture
    glBindTexture(GL_TEXTURE_2D, ch.textureID);

    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Advance cursor for next glyph
    xpos += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels
  }

  // Restore previous blend state
  if (!blendWasEnabled) {
    glDisable(GL_BLEND);
  }
  glBlendFunc(originalSrcFactor, originalDstFactor);

  // Unbind resources
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
}

void shutdownTextRenderer() {
  // Delete character textures
  for (unsigned char c = 0; c < 128; c++) {
    if (characters[c].textureID != 0) {
      glDeleteTextures(1, &characters[c].textureID);
    }
  }

  // Delete VAO, VBO and shader program for text rendering
  glDeleteVertexArrays(1, &textVAO);
  glDeleteBuffers(1, &textVBO);
  glDeleteProgram(textShaderProgram);

  // Free FreeType resources
  FT_Done_Face(face);
  FT_Done_FreeType(ft);
}

void shutdownRenderer() {
  // Clean up shape rendering resources
  glDeleteProgram(shaderProgram);
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &triangleVAO);
  glDeleteBuffers(1, &triangleVBO);
  glDeleteVertexArrays(1, &circleVAO);
  glDeleteBuffers(1, &circleVBO);
}
