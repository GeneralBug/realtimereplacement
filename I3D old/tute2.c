#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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

void drawAxes();
void drawLinear(float, float);
void drawQuadratic(float, float, float);
void drawTrig(float, float);

typedef struct 
{
	float t, 
	 lastT,
	 dt;
} Global;
Global global;
int SEGMENT_COUNT = 64;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	drawAxes();
	//drawLinear(-2, 0);
	drawQuadratic(1, 0, 0);

	global.dt = global.t - glutGet(GLUT_ELAPSED_TIME);
	global.t = glutGet(GLUT_ELAPSED_TIME);
	//drawTrig(1, 1);
	int err;
	while ((err = glGetError()) != GL_NO_ERROR)
		printf("display: %s\n", gluErrorString(err));

	glutSwapBuffers();
}

/* You can ignore this for now, it just lets you exit when you press 'q' or ESC */
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
	case 'q':
		exit(EXIT_SUCCESS);
		break;
	default:
		break;
	}
}

void init()
{
	/* In this program these OpenGL calls only need to be done once,
	but normally they would go elsewhere, e.g. display */

	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
}

void drawAxes()
{
	/* DRAW AXES */
	//x
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0,0,0);
	glVertex3f(1,0,0);
	glEnd();
	//y
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0,0,0);
	glVertex3f(0,1,0);
	glEnd();
	//z
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(0,0,0);
	glVertex3f(0,0,1);
	glEnd();
}

void drawLinear(float m, float c)
{
	//x = 0,1
	//y = mx+c
	float x1 = (-1-c)/m ;
	float x2 = (1-c)/m;
	/*
	float y1 = (x1 * m) + c;
	float y2 = (x2 * m)+c;
	*/
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);

	glVertex3f(x2, 1, 0.0f);
	glVertex3f(x1, -1 ,0);

	glEnd();
}

void drawQuadratic(float a, float b, float c)
{
	int i;
	float left = -1;
	float right = -0.5;
	float range = right - left;
	float stepSize = range/SEGMENT_COUNT;
	float x;
	float y;
	glColor3f(1, 1, 1);
	glBegin(GL_LINE_STRIP);
	

	for(i = 0; i <= SEGMENT_COUNT; i++)
	{
		x = i*stepSize+left;
		y = (a*x*x) + (b*x) + c;
		glVertex3f(x, y, 0.0f);
	}
	glEnd();
}

void drawTrig(float amp, float wave)
{
		int i;
	float k = (2 * M_PI)/wave;
	float left = -1;
	float right = 1;
	float range = right - left;
	float stepSize = range/SEGMENT_COUNT;
	float x;
	float y;
	glColor3f(1, 1, 1);
	glBegin(GL_LINE_STRIP);
	
	for(i = 0; i <= SEGMENT_COUNT; i++)
	{
		x = i*stepSize+left;
		y = amp * sinf((k * x) + (M_PI/4) * global.dt / 10);
		glVertex3f(x, y, 0.0f);
	}
	glEnd();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Tutorial 1");

	init();
	glutIdleFunc(display);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	
	return EXIT_SUCCESS;
}