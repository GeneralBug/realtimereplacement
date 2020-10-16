#pragma once
class main
{
	#include <stdbool.h>
	#include <stdio.h>
	#include <math.h>
	#include <list>

	#if _WIN32
	#   include <Windows.h>
	#	include <GL/glew.h>
	#endif
	#if __APPLE__

	#   include <OpenGL/gl.h>
	#   include <OpenGL/glu.h>
	//#   include <GLUT/glut.h>


	#else
	#   include <GL/gl.h>
	#   include <GL/glu.h>
	#   include <GL/glut.h>
	#endif


	#define GLM_FORCE_RADIANS
	#include <glm/glm.hpp>
	#include <glm/gtc/matrix_transform.hpp>
	#include <glm/gtc/type_ptr.hpp>
	#include <glm/gtc/constants.hpp>
	#include <SDL.h>
	#include "shaders.h"

	#define vertecies 0
	#define normals 1
	#define offsetBuffer 2

	typedef enum { line, fill } polygonMode_t;
	typedef enum { grid, wave } shape_t;

	//SHADERS!
	GLuint shaderProgram;
	const char* vert = "shader.vert";
	const char* frag = "shader.frag";

	#define vertecies 0
	#define normals 1
	#define offsetBuffer 2

	#define MILLI 1000
	#define ROAD_HEIGHT 0.1
	#define SCALE 0.05//unit of shift when changing trajectory
	#define MAX_X 1.5
	#define SEGMENT_COUNT 64//used for quadratic
	#define CAR_SIZE 1
	#define CAR_COUNT 3
	#define CAR_HEIGHT CAR_SIZE+(CAR_SIZE/2)+ROAD_HEIGHT
	#define LOG_SIZE 1.5
	#define LOG_COUNT 3
	#define LOG_RADIUS 1
	#define TILE_SIZE 15
	#define DEBUG true;

	typedef struct 
	{ 
		float x, y; 
	} vec2f;
	typedef struct 
	{ 
		float x, y, z; 
	} vec3f;
	typedef struct 
	{ 
		bool debug; 
		float scale, zoom_scale;
		vec2f angle, last;
		float zoom;
		bool rotating; 
	} camera;


	camera c;
	void reshape(int, int);
	void drawAxes(float);
	void mouseMotion(int, int);
	void mouseCB(int, int, int, int);
	void cameraTransform();
	void toggleAxes();


	// s3718372.h
	typedef struct 
	{
		bool debug; 
		int tess; 
		bool lit, wire, stationary, dead;
		float g; 
		float startTime; 
		int frames;
		float frameRate, frameRateInterval, lastFrameRateT, time; 
		bool pause;
		int width, height;
		bool vbo;
	} global;

	typedef struct 
	{ 
		vec3f r0, v0, r, v; 
		float radius; 
		bool isFrog;
	} actor;

	typedef struct 
	{ 
		vec3f low, high;
	} rect;

	// shapes.h
	void drawNormals(float, float, float);
	//void drawSquare(vec3f, float);
	void drawSquare(vec3f, vec3f, vec2f);
	void drawSphere(vec3f, float, float, float);
	void drawCylinder(vec3f, float, float, float);

	//void drawFrog(int);
	void drawLimb(vec2f);
	typedef struct 
	{ 
		bool drawNormals, wireframe, lit; 
		float normal_scale; 
	} shape_globals;

	shape_globals sg;

	void (*displayFunc)();
	void display();

	typedef enum { inactive, rotate, pan, zoom } CameraControl;
	int err;
}

