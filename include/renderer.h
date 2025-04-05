#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdio.h> // For error reporting

// Rectangle shape for 2D rendering
typedef struct {
  GLfloat x, y;          // Center position
  GLfloat width, height; // Dimensions
  GLfloat r, g, b, a;    // Color
} Rectangle;

// Circle shape for 2D rendering
typedef struct {
  GLfloat x, y;       // Center position
  GLfloat radius;     // Circle radius
  GLfloat r, g, b, a; // Color
  int segments;       // Number of segments (detail level)
} Circle;

// Triangle shape for 2D rendering
typedef struct {
  GLfloat x1, y1;     // First vertex
  GLfloat x2, y2;     // Second vertex
  GLfloat x3, y3;     // Third vertex
  GLfloat r, g, b, a; // Color
} Triangle;

// Character information for text rendering
typedef struct {
  GLuint textureID;       // Texture containing glyph image
  int width, height;      // Size of the glyph
  int bearingX, bearingY; // Offset from baseline
  int advance;            // Horizontal advance for next character
} Character;

// Core renderer functions
void initRenderer(int width, int height);
void shutdownRenderer();
void setWindowSize(int width, int height);

// Shape rendering functions
void drawRectangle(Rectangle rect);
void drawCircle(Circle circle);
void drawTriangle(Triangle triangle);

// Text rendering functions
int initTextRenderer(const char *fontPath, int fontSize);
void renderText(const char *text, float x, float y, float scale, float r,
                float g, float b, float a);
void shutdownTextRenderer();

#endif
