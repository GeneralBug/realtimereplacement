/* ===========================================
 * UTILITY FUNCTIONS FOR OPENGL GLUT
 * CONTAINS AXES, CAMERA CONTROL
 * AUTHOR: HARRISON ELLEM, S3718372
 * BASED ON TUTORIALS AND STUFF BY GEOFF LEACH
 * =========================================== */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#if _WIN32
#   include <Windows.h>
#endif
#if __APPLE__
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
#   include <GLUT/glut.h>
#else
#   include <GL/gl.h>
#   include <GL/glu.h>
#   include <GL/glut.h>
#endif

#define DEBUG true;

typedef struct { float x, y; } vec2f;
typedef struct { float x, y, z; } vec3f;
typedef struct {bool debug; float scale, zoom_scale; vec2f angle, last; float zoom; bool rotating; } camera; 


camera c;
void reshape(int, int);
void drawAxes(float);
void mouseMotion(int, int);
void mouseCB(int, int, int, int);
void cameraTransform();
void toggleAxes();