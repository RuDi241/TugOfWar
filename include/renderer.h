#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

// Rectangle shape
typedef struct {
  GLfloat x, y;          // Center position
  GLfloat width, height; // Dimensions
  GLfloat r, g, b, a;    // Color
} Rectangle;

// Circle shape
typedef struct {
  GLfloat x, y;       // Center position
  GLfloat radius;     // Circle radius
  GLfloat r, g, b, a; // Color
  int segments;       // Number of segments (detail level)
} Circle;

// Triangle shape
typedef struct {
  GLfloat x1, y1;     // First vertex
  GLfloat x2, y2;     // Second vertex
  GLfloat x3, y3;     // Third vertex
  GLfloat r, g, b, a; // Color
} Triangle;

// Character information for text rendering
typedef struct {
  GLuint textureID;     // ID handle of the glyph texture
  int width;            // Size of glyph (width)
  int height;           // Size of glyph (height)
  int bearingX;         // Bearing X (offset from baseline to left of glyph)
  int bearingY;         // Bearing Y (offset from baseline to top of glyph)
  unsigned int advance; // Advance width for next character
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
void renderTextLeft(const char *text, float x, float y, float scale, float r,
                    float g, float b, float a);
void renderTextCenter(const char *text, float x, float y, float scale, float r,
                      float g, float b, float a);
void debugTextRendering();
void shutdownTextRenderer();

#endif
