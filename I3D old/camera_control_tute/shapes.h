/* ===========================================
 * SHAPE DRAWING FUNCTIONS FOR OPENGL GLUT
 * CONTAINS AXES, CAMERA CONTROL
 * AUTHOR: HARRISON ELLEM, S3718372
 * BASED ON TUTORIALS AND STUFF BY GEOFF LEACH
 * =========================================== */
#include "utils.h"

void drawNormals(float, float, float);
//void drawSquare(vec3f, float);
void drawSquare(vec3f, vec3f, vec2f);
void drawSphere(vec3f, float, float, float);
void drawGrid(int, float, bool);

typedef struct {bool normals, wireframe, lit; float normal_scale; } shape_globals;


shape_globals sg;