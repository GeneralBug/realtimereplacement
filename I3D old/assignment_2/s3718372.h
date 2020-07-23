/* ===========================================
 * FROGGER WITH OPENGL/GLUT
 * CONTAINS LOTS OF STUFF
 * AUTHOR: HARRISON ELLEM, S3718372
 * BASED ON TUTORIALS AND STUFF BY GEOFF LEACH
 * =========================================== */
#include "shapes.h"

typedef struct { bool debug; int tesselation; bool lit, wire, stationary, dead; 
	float g; float startTime; int frames; 
	float frameRate, frameRateInterval, lastFrameRateT, time; bool pause; } global;
	
typedef struct { vec3f r0, v0, r, v; float radius; bool isFrog; } actor;
typedef struct { vec3f low, high;} rect;
